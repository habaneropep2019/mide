
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG
#undef DEBUG
#endif
#define NO_DEBUG 1


#include <qapp.h>    
#include <stdio.h>    
#include <stdlib.h>

void myMessageOutput( QtMsgType type, const char *msg );

#endif //__DEBUG_H__
