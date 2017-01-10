/* This is LGPLed free software.
 * This code implements a chained hash table where we use binary search trees
 * instead of linked lists as chains.  It has been rudimentarily tested.
 * Let's call this a hash tree.
 * The interface allows for more than one hash tree at a time.
 * This code uses blobs, not 0-terminated strings.
 * Note: The manipulation functions are not thread safe.
 * Author: Felix von Leitner <leitner@math.fu-berlin.de>
 */

#ifndef RDBM_HASH_H
#define RDBM_HASH_H

/* lcc has no inline support */
#ifdef __LCC__
#define inline
#endif
/* In case someone compiles this with -ansi -pedantic */
#ifdef __GNUC__
#define inline __inline__
#endif

struct node;
struct hashtab {
  unsigned int hashtablen;
  struct node **hashtab;
};

/* Initialize a hash table to contain no entries.
 * A length of 1021 works fine in experience. */
void ht_init(struct hashtab *ht,unsigned int len);

/* Insert one key/value pair in the hash tree.
 * This routine copies the key and value, so you can pass values from
 * the stack or data from network buffers. */
void ht_insert(struct hashtab *ht,unsigned int len,const char *key,
	       unsigned int vlen,char *value);

/* Insert one key/value pair in the hash tree.
 * This routine does _not_ copy the key and value arguments it gets
 * passed. */
void ht_insert_nocopy(struct hashtab *ht,unsigned int len,const char *key,
		      unsigned int vlen,char *value);

/* Look a key up and return the appropriate value.
 * Return nonzero if not found. */
int ht_lookup(struct hashtab *ht,unsigned int klen,const char *key,
	      int *vlen,char **value);

/* Mark a key as deleted.
 * Return nonzero if not found. */
int ht_delete_mark(struct hashtab *ht,unsigned int klen,const char *key);

/* Delete a key/value pair.
 * Return nonzero if not found. */
int ht_delete(struct hashtab *ht,unsigned int klen,const char *key);

/* Find the first entry in the hash tree.
 * Returns nonzero if empty. */
int ht_findfirst(struct hashtab *ht,unsigned int *klen,const char **key);

/* Find the next entry in the hash tree.
 * Returns nonzero if last. */
int ht_findnext(struct hashtab *ht,unsigned int *klen,const char **key);

/* Delete and free() all entries */
void ht_delete_all(struct hashtab *ht);

/* Delete all entries, but don't free() the keys */
void ht_delete_all_nofree(struct hashtab *ht);

#endif
