#ifdef MEM_DEBUG
#ifndef _NEW_ASJ_H
#define _NEW_ASJ_H
#include <stdlib.h>

void* operator new(size_t sizeofX, char *id);
void* operator new(size_t sizeofX);
void* operator new[](size_t sizeofX);
void* operator new[](size_t sizeofX, char *id);
void operator delete(void *p);
void operator delete[](void *p);

#endif
#endif

#ifndef MEM_DEBUG
#ifndef _NEW_ASJ_H
#define _NEW_ASJ_H
#include <stdlib.h>

inline void* operator new(size_t sizeofX, char *) {return operator new(sizeofX);}
inline void* operator new[](size_t sizeofX, char *) { return operator new[](sizeofX); }
#endif
#endif


