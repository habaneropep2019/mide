/* This is LGPLed free software.
 * This code implements a chained hash table where we use binary search trees
 * instead of linked lists as chains.  It has been rudimentarily tested.
 * Let's call this a hash tree.
 * The interface allows for more than one hash tree at a time.
 * This code uses blobs, not 0-terminated strings.
 * Note: The manipulation functions are not thread safe.
 * Should compile without warning on all ANSI C platforms.
 * Author: Felix von Leitner <leitner@math.fu-berlin.de>
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

/* While these routines can be used as associative hash, they are meant
 * as a cache for a larger, disk-base database or mmap'ed file.  And I
 * provide functions to mark a record as deleted, so that this can be
 * used to cache deltas to a constant database like CDB.
 */

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

/* lcc has no inline support */
#ifdef __LCC__
#define inline
#endif
/* In case someone compiles this with -ansi -pedantic */
#ifdef __GNUC__
#define inline __inline__
#endif

/* Primitive, suboptimal, crappy hash function.
 * Feel free to replace this with a better one. */
inline unsigned short hash(const char *c,int len) {
  unsigned i=0;
  while (--len) {
    i=(i*7+(int) *(c++));
  }
  return i;
}

/* One node of the binary search tree behind every entry of the hash
 * table */
struct node {
  struct node *left,*right;
  int klen,vlen;
  char *key,*value;
};

/* The hash table structure */
struct hashtab {
  unsigned int hashtablen;
  struct node **hashtab;
};

/* Initialize a hash table to contain no entries.
 * A length of 1021 works fine in experience. */
int ht_init(struct hashtab *ht,unsigned int len) {
  assert(ht != NULL);
  ht->hashtablen=len;
  ht->hashtab=(struct node **)malloc(sizeof(struct node *)*len);
  if (ht->hashtab)
    memset(ht->hashtab,0,sizeof(struct node *)*len);
  else
    return -1;
  return 0;
}

/* Internal function.  To save code, I use only a single function to do
 * the lookup in the hash table and binary search tree.  Because the
 * insertion routine has to know where to insert the node, this routine
 * has to return a pointer to the NULL pointer to manipulate in that
 * case, so we have to do this nasty indirection. */
struct node **lookup(struct hashtab *ht,unsigned int len,const char *key) {
  struct node **tmp;
  assert(ht);
  tmp=&(ht->hashtab[hash(key,len) % ht->hashtablen]);
  while (*tmp) {
    int result=memcmp(key,(*tmp)->key,min(len,(*tmp)->klen));
    if ((*tmp)->klen == len && result==0)	/* found it */
        return tmp;
    else if (result<0)
        tmp=&((*tmp)->left);
    else
        tmp=&((*tmp)->right);
  }
  return tmp;
}

/* Insert one key/value pair in the hash tree.
 * This routine copies the key and value, so you can pass values from
 * the stack or data from network buffers. */
int ht_insert(struct hashtab *ht,unsigned int len,const char *key,
	       unsigned int vlen,char *value) {
  struct node **tmp=lookup(ht,len,key);
  if (*tmp) {	/* already there */
    char *temp=(char *)malloc(vlen);
    if (!temp)
      return -1;
    memmove(temp,value,vlen);
    free((*tmp)->value);
    (*tmp)->vlen=vlen;
    (*tmp)->value=temp;
  } else {
    char *k=(char *)malloc(len),*v=(char *)malloc(vlen);
    *tmp=(struct node *)malloc(sizeof(struct node));
    if (!*tmp || !k || !v) {
      free(*tmp); free(k); free(v); *tmp=0;
      return -1;
    }
    (*tmp)->left=(*tmp)->right=0;
    (*tmp)->klen=len;
    (*tmp)->key=k;
    memmove((*tmp)->key,key,len);
    (*tmp)->vlen=vlen;
    (*tmp)->value=v;
    memmove((*tmp)->value,value,vlen);
  }
  return 0;
}

/* Insert one key/value pair in the hash tree.
 * This routine does _not_ copy the key and value arguments it gets
 * passed. */
int ht_insert_nocopy(struct hashtab *ht,unsigned int len,const char *key,
		      unsigned int vlen,char *value) {
  struct node **tmp=lookup(ht,len,key);
  if (*tmp) {	/* already there */
    (*tmp)->vlen=vlen;
    (*tmp)->value=value;
  } else {
    *tmp=(struct node *)malloc(sizeof(struct node));
    if (!*tmp)
      return -1;
    (*tmp)->left=(*tmp)->right=0;
    (*tmp)->klen=len;
    (*tmp)->key=(char *)key;
    (*tmp)->vlen=vlen;
    (*tmp)->value=value;
  }
  return 0;
}

/* Look a key up and return the appropriate value. */
int ht_lookup(struct hashtab *ht,unsigned int klen,const char *key,
	      int *vlen,char **value) {
  struct node **tmp=lookup(ht,klen,key);
  if (*tmp) {
    *vlen=(*tmp)->vlen;
    *value=(*tmp)->value;
    return 0;
  } else {
    *vlen=-1;
    return -1;
  }
}

/* Mark a key as deleted. */
int ht_delete_mark(struct hashtab *ht,unsigned int klen,const char *key) {
  struct node **tmp=lookup(ht,klen,key);
  if (*tmp) {
    if ((*tmp)->vlen < 0)
      return -2;
    free((*tmp)->value);
    (*tmp)->vlen=-1;
    (*tmp)->value=0;
    return 0;
  } else
    return -1;
}

/* Delete a key/value pair.
 * This is a little complex because we have to delete from the binary
 * search tree */
int ht_delete(struct hashtab *ht,unsigned int klen,const char *key) {
  struct node **tmp=lookup(ht,klen,key);
  if (*tmp) {
    free((*tmp)->value);
    free((*tmp)->key);
    if ((*tmp)->left) {
      if ((*tmp)->right) {	/* two children, replace with biggest smaller */
	struct node **rightmost=&((*tmp)->left);
	while ((*rightmost)->right)
	  rightmost=&((*rightmost)->right);
	(*tmp)->klen=(*rightmost)->klen;
	(*tmp)->key=(*rightmost)->key;
	(*tmp)->vlen=(*rightmost)->vlen;
	(*tmp)->value=(*rightmost)->value;
	if ((*rightmost)->left) {
	  struct node *temp=(*rightmost);
	  *rightmost=(*rightmost)->left;
	  free(temp);
	} else {
	  free(*rightmost);
	  *rightmost=0;
	}
      } else {	/* only left child */
	struct node *onlyson=(*tmp)->left;
	free(*tmp);
	*tmp=onlyson;
      }
    } else {
      if ((*tmp)->right) {	/* only right child */
	struct node *onlyson=(*tmp)->right;
	free(*tmp);
	*tmp=onlyson;
      } else {	/* no children */
	free(*tmp);
	*tmp=0;
      }
    }
    return 0;
  } else
    return -1;
}

/* Find the first entry in the hash tree.
 * Returns nonzero if empty. */
int ht_findfirst(struct hashtab *ht,unsigned int *klen,const char **key) {
  int i;
  for (i=0; i<ht->hashtablen; i++) 
    if (ht->hashtab[i])
      break;
  if (i<ht->hashtablen && ht->hashtab[i]) {
    *klen=ht->hashtab[i]->klen;
    *key=ht->hashtab[i]->key;
    return 0;
  } else
    return -1;
}

/* Find the next entry in the hash tree.
 * Returns nonzero if last. */
int ht_findnext(struct hashtab *ht,unsigned int *klen,const char **key) {
  int hashnum=hash(*key,*klen) % ht->hashtablen;
  struct node *tmp=ht->hashtab[hashnum];
  struct node *right=0;
  while (tmp) {
    int result=memcmp(*key,tmp->key,min(*klen,tmp->klen));
    if (tmp->klen == *klen) {
      if (result==0) {	/* found it */
	if (tmp->left) {
	  *klen=tmp->left->klen;
	  *key=tmp->left->key;
	  return 0;
	} else if (tmp->right) {
	  *klen=tmp->right->klen;
	  *key=tmp->right->key;
	  return 0;
	}
	if (right==tmp || right==0) {	/* was last element, proceed */
	  while (++hashnum<ht->hashtablen)
	    if (ht->hashtab[hashnum]) {
	      *klen=ht->hashtab[hashnum]->klen;
	      *key=ht->hashtab[hashnum]->key;
	      return 0;
	    }
	  return -1;
	} else {
	  *klen=right->klen;
	  *key=right->key;
	  return 0;
	}
      }
      if (tmp->right)
	right=tmp->right;
      if (result<0)
	tmp=tmp->left;
      else
	tmp=tmp->right;
    }
  }
  return -1;
}

void ht_delete_tree(struct node *n) {
  if (n->left) ht_delete_tree(n->left);
  if (n->right) ht_delete_tree(n->right);
  free(n->key);
  if (n->vlen!=-1) free(n->value);
  free(n);
}

/* Delete and free() all entries */
void ht_delete_all(struct hashtab *ht) {
  int i;
  for (i=0; i<ht->hashtablen; i++) 
    if (ht->hashtab[i])
      ht_delete_tree(ht->hashtab[i]);
  free(ht->hashtab);
}

void ht_delete_tree_nofree(struct node *n) {
  if (n->left) ht_delete_tree(n->left);
  if (n->right) ht_delete_tree(n->right);
  free(n);
}

/* Delete all entries, but don't free() the keys */
void ht_delete_all_nofree(struct hashtab *ht) {
  int i;
  for (i=0; i<ht->hashtablen; i++) 
    if (ht->hashtab[i])
      ht_delete_tree_nofree(ht->hashtab[i]);
  free(ht->hashtab);
}

#ifdef MAIN
int main() {
  int fubar;
  struct hashtab ht;
  char *key;
  ht_init(&ht,2);
  ht_insert(&ht,5,"Fefe",7,"foobar");
  ht_insert(&ht,5,"Test",7,"barbaz");
  ht_insert(&ht,5,"Amen",7,"barbaz");
  ht_lookup(&ht,5,"Fefe",&fubar,&key);
  if (fubar>=0)
    puts(key);
  {
    int klen;
    const char *key=0;
    if (!ht_findfirst(&ht,&klen,&key)) {
      puts(key);
      while (!ht_findnext(&ht,&klen,&key)) {
	puts(key);
      }
    }
  }
  ht_delete(&ht,5,"Fefe");
  ht_lookup(&ht,5,"Fefe",&fubar,&key);
  if (fubar>=0)
    puts(key);
  return 0;
}
#endif
