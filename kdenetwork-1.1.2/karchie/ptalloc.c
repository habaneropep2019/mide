/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * $Id: ptalloc.c,v 1.1.1.1 1998/10/03 21:23:24 habenich Exp $
 *
 * v1.3.2 - bpk - for Archie client 1.3.2, except gf ZERO->BZERO
 * v1.1.2 - gf  11/02/91 - renamed ZERO() to BZERO() for X
 */

#include <copyright.h>
#include <stdio.h>

#include <pfs.h>
#include "config.h"	/* gf */
#include "stringdefs.h"	/* for correct definition of bzero used by BZERO */

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>  /* For definition of BSD if applicable */
#endif

#ifdef MSDOS
# define free _pfree   /* otherwise we get conflicts with free() */
#endif

#ifdef BSD
#include <stdlib.h>
#define free _pfree /* According to Kevin Street, this is needed on BSDs too */
#endif
#ifdef KARCHIE
/* I dont know, why stdlib is only for BSD included,
   but if you like it that way...
   I want to get rid of the warnings, so include it */
#include <stdlib.h>
#define free _pfree /* According to Kevin Street, this is needed on BSDs too */
#endif

static PTEXT	free = NULL;
int 		ptext_count = 0;
int		ptext_max = 0;

/*
 * ptalloc - allocate and initialize ptext structure
 *
 *    PTALLOC returns a pointer to an initialized structure of type
 *    PTEXT.  If it is unable to allocate such a structure, it
 *    returns NULL.
 */
PTEXT
ptalloc()
    {
	PTEXT	vt;
	if(free) {
	    vt = free;
	    free = free->next;
	}
	else {
	    vt = (PTEXT) malloc(sizeof(PTEXT_ST));
	    if (!vt) return(NULL);
	    ptext_max++;
	}
	ptext_count++;

	/* nearly all parts are 0 [or NULL] */
	BZERO(vt);
	/* The offset is to leave room for additional headers */
	vt->start = vt->dat + MAX_PTXT_HDR;

	return(vt);
    }

/*
 * ptfree - free a VTEXT structure
 *
 *    VTFREE takes a pointer to a VTEXT structure and adds it to
 *    the free list for later reuse.
 */
void
ptfree(vt)
    PTEXT	vt;
    {
	vt->next = free;
	vt->previous = NULL;
	free = vt;
	ptext_count--;
    }

/*
 * ptlfree - free a VTEXT structure
 *
 *    VTLFREE takes a pointer to a VTEXT structure frees it and any linked
 *    VTEXT structures.  It is used to free an entrie list of VTEXT
 *    structures.
 */
void
ptlfree(vt)
    PTEXT	vt;
    {
	PTEXT	nxt;

	while(vt != NULL) {
	    nxt = vt->next;
	    ptfree(vt);
	    vt = nxt;
	}
    }

