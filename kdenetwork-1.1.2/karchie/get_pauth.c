/*
 * Copyright (c) 1989, 1990 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * gf: xarchie v2.0 - Sync with archie v1.4.1 - changes marked with "gf"
 */

#include <copyright.h>
#include <stdio.h>
#include "config.h"				/* gf */
#include "stringdefs.h"				/* gf */

#ifndef VMS
# if defined(MSDOS) && !defined(OS2) && !defined(PCNFS)
#  ifndef CUTCP
#   include <rwconf.h>
#  endif
# else
#  include <pwd.h>
# endif
#else
# include <jpidef.h>
# include <vms.h>
#endif

#include <pcompat.h>
#include <pauthent.h>

#ifdef KARCHIE
/* I want to get rid of the warnings, so include it */
#include <unistd.h>
#endif

/*gf*/
/*ARGSUSED*/
PAUTH
get_pauth(type)
    int		type;
    {
	static PAUTH_ST   no_auth_st;
	static PAUTH		  no_auth = NULL;
#if !defined(VMS) && !defined(MSDOS) || defined(OS2) || defined(PCNFS)
	struct passwd *whoiampw;
#else
	char username[13];
	unsigned short usernamelen;
	struct {
	    unsigned short buflen;
	    unsigned short itmcod;
	    char *bufadr;
	    unsigned short *retlenadr;
	    unsigned long null;
	} jpi_itemlist;
#endif

	if(no_auth == NULL) {
	    no_auth = &no_auth_st;
	    strcpy(no_auth->auth_type,"UNAUTHENTICATED");

	    /* find out who we are */
#ifndef VMS
#if defined(MSDOS) && !defined(OS2) && !defined(PCNFS)
#ifndef CUTCP
	    if (!getconf("general", "user", no_auth->authenticator, 250)
		|| (strlen (no_auth->authenticator) == 0))
#endif
	      strcpy(no_auth->authenticator,"nobody");
#else /* not MSDOS */
	    DISABLE_PFS(whoiampw = getpwuid(getuid()));
	    if (whoiampw == 0) strcpy(no_auth->authenticator,"nobody");
	    else strcpy(no_auth->authenticator, whoiampw->pw_name);
#endif /* not MSDOS */
#else
	    jpi_itemlist.buflen = sizeof(username);
	    jpi_itemlist.itmcod = JPI$_USERNAME;
	    jpi_itemlist.bufadr = &username;
	    jpi_itemlist.retlenadr = &usernamelen;
	    jpi_itemlist.null = 0;
	    if (SYS$GETJPI(0, 0, 0, &jpi_itemlist, 0, 0, 0) & 0x1)
	    {
		username[usernamelen] = 0;
		strcpy(no_auth->authenticator, username);
	    } else
		strcpy(no_auth->authenticator, "nobody");
#endif
	}
	return(no_auth);
    }
