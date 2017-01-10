/* This is LGPLed free software.
 * This code implements a layer over Dan Bernstein's great CDB database
 * library.  Of all the libraries I've ever used, CDB is the only one
 * that never failed me.  I hope this library will be the second one.
 * Author: Felix von Leitner <leitner@math.fu-berlin.de>
 * At least rdb_compact_db partly taken from cdb, thus no copyright
 * claimed on that part of the code.
 */

#ifndef RDBM_H
#define RDBM_H
/*#include <cdb.h>*/
#include "uint32.h"
#include "hash.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


/* the interface is very similar to the gdbm one */
typedef struct {
  char *dptr;
  int   dsize;
} datum;

struct rdb {
  int cdbfd;	/* file descriptor of the cdb file */
  int logfd;	/* file descriptor of the log file */
  struct hashtab ht;	/* hash tree for the db cache */
  char *db,*log;	/* filenames for the database and log file */
  int mode;		/* mode for file creation */
  uint32 eod;		/* length of data area of the cdb file */
  datum key,value;	/* cache for traversal. */
  	/* when we traverse a cdb file, we always get back a (key,value)
	 * tuple, but we give the user only the key.  If the user does a
	 * lookup on all the keys, he will pollute the hash tree cache
	 * with lots of unnecessary entries.  So we keep a (key,value)
	 * tuple here for caching.  Yes, that's a kludge. */
};

/* Open a reliable database.
 * Returns nonzero on error */
int rdb_open(struct rdb *db, const char *filename, const char *logname,
	     int mode, unsigned int htlen);

/* Close a reliable database.
 * Returns nonzero on error */
int rdb_close(struct rdb *db);

/* Write a new cdb incorporating all the logged changes,
 * delete the log and reopen the new cdb.
 * Returns nonzero on error */
int rdb_reorganize(struct rdb *db);

/* Write a key/value pair to a reliable database.
 * If flags is zero, fail if the record is already there.
 * Returns nonzero on error */
int rdb_store(struct rdb *db,datum key,datum content,int flags);

/* Fetch the value for a given key from the database.
 * Returns nonzero on error */
datum rdb_fetch(struct rdb *db, datum key);

/* Delete key/value pair from database.
 * Returns nonzero on error */
int rdb_delete(struct rdb *db, datum key);

/* Find first key in database.
 * Returns datum.dptr==0 on EOF or error. */
datum rdb_firstkey(struct rdb *db);

/* Find next key in database.
 * Returns datum.dptr==0 on EOF or error. */
datum rdb_nextkey(struct rdb *db,datum key);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif



#endif
