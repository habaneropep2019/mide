#ifndef OPTIONS_H
#define OPTIONS_H
/** This struct handles global options. Not user ones */

#include <sys/utsname.h>
#ifndef SYS_NMLN
#define SYS_NMLN  65             /* max hostname size */
#endif

struct sOptions
{ 
    int answmach; /* used in talkd.cpp */
    int time_before_answmach; /* in machines/answmach.cpp */
    /* used in announce.cpp : */
    int sound;
    int XAnnounce;
    char soundfile [S_CFGLINE];
    char soundplayer [S_CFGLINE];
    char soundplayeropt [S_CFGLINE];
    char announce1 [S_CFGLINE];
    char announce2 [S_CFGLINE];
    char announce3 [S_CFGLINE];
    char invitelines [S_INVITE_LINES];/* used in machines/answmach.cpp */
    char mailprog [S_CFGLINE]; /* used in machines/answmach.cpp */
    int NEU_behaviour;
    char NEU_user[S_CFGLINE];
    char NEUBanner1 [S_CFGLINE];
    char NEUBanner2 [S_CFGLINE];
    char NEUBanner3 [S_CFGLINE];
    char NEU_forwardmethod [5];
    char extprg [S_CFGLINE];
    // No really an option, but it's useful to have it here :
    char hostname[SYS_NMLN];
    int debug_mode;
};

extern struct sOptions Options;

#endif
