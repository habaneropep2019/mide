#ifndef _VERSION_H


#define VERSION_NUM 1.7
#define VERSION_SHORTTXT "1.7"
#define VERSION_TXT "KMid 1.7" 

// Undefine WORKING for release versions
//#define WORKING

// I want to keep some special variables defined at home for personal 
// adjustments
//#define AT_HOME


// The next definition makes kmid display some general debug messages
// that may be helpful to locate any bug, but without making lots of
// messages. Please when reporting a bug, be sure that this is set and
// send me the output.
//#define GENERAL_DEBUG_MESSAGES 

// Define MODE_DEMO_ONLYVISUAL for kmid to work on systems with no soundcard,
// or when it is not well configured. You will only get visual feedback, that
// is, you will see the letter changing of colors, but no music. Really, you
// would get music if it is possible, but by defining this, many synchronization
// routines will be turned off.
//#define MODE_DEMO_ONLYVISUAL


// And now, DEBUG messages :
#ifdef WORKING
#define KMidDEBUG
//#define KDISPTEXTDEBUG
//#define COLLECTDLGDEBUG
//#define DEVICEMANDEBUG
//#define FMOUTDEBUG
//#define GUSOUTDEBUG
//#define MIDFILEDEBUG
//#define MIDIOUTDEBUG
//#define MIDISTATDEBUG
//#define MIDIMAPPERDEBUG
//#define SYNTHOUTDEBUG
//#define VOICEMANDEBUG
//#define PLAYERDEBUG
//#define TRACKDEBUG
#endif

#endif
