/*
 * archie.h : Definitions for the programmatic Prospero interface to Archie
 *
 *     Written by Brendan Kehoe (brendan@cygnus.com),
 *                George Ferguson (ferguson@cs.rochester.edu), and
 *                Clifford Neuman (bcn@isi.edu).
 */

/* You can't touch this.  */
#ifndef XARCHIE
# define ARCHIES	"archie.ans.net (USA [NY])","archie.rutgers.edu (USA [NJ])","archie.sura.net (USA [MD])","archie.unl.edu (USA [NE])","archie.mcgill.ca (Canada)","archie.funet.fi (Finland/Mainland Europe)","archie.au (Australia)","archie.doc.ic.ac.uk (Great Britain/Ireland)","archie.wide.ad.jp (Japan)","archie.ncu.edu.tw (Taiwan)"
# define NARCHIES	10
#endif

/*
 * Default value for max hits.  Note that this is normally different
 * for different client implementations.  Doing so makes it easier to
 * collect statistics on the use of the various clients.
 */
#ifdef VMS
# define	MAX_HITS	98	/* VMS Client */
#else
# ifdef XARCHIE
#  define	MAX_HITS	99	/* X Client */
# else
#  define	MAX_HITS	95	/* Normal client */
# endif
#endif

/*
 * CLIENT_VERSION may be used to identify the version of the client if 
 * distributed separately from the Prospero distribution.  The version
 * command should then identify both the client version and the Prospero
 * version identifiers.   
 */
#ifdef XARCHIE
# define CLIENT_VERSION	"1.3-X"
#else
# define CLIENT_VERSION "1.4.1"
#endif

/* The different kinds of queries we can make.  */
typedef enum query_type
{
  NONE = '\0', EXACT = '=', REGEXP = 'R',
  SUBSTRING = 'S', SUBSTRING_CASE = 'C'
} Query;

/* Procedures from user/aquery.c */
extern VLINK archie_query (); 

/* defcmplink(p,q) and invdatecmplink(p,q)                             */
extern int defcmplink();	/* Compare by host then by filename    */
extern int invdatecmplink();	/* Compare links inverted by date      */

/* Definitions for the comparison procedures                           */
#define AQ_DEFCMP	defcmplink
#define AQ_INVDATECMP	invdatecmplink

/* Flags                                                               */
#define AQ_NOSORT	0x01	/* Don't sort                          */
#define AQ_NOTRANS	0x02	/* Don't translate Archie responses    */

/* To keep the code clean (hah!). */
#ifdef VMS
# define ERROR_EXIT	SS$_NORMAL
#else
# define ERROR_EXIT	1
#endif
