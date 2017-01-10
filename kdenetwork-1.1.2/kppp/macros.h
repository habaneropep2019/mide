// helper macros for layouting

#ifndef __MACROS__H__
#define __MACROS__H__

#include <qlayout.h>

#define MIN_WIDTH(w) w->setMinimumWidth(w->sizeHint().width());
#define MIN_HEIGHT(w) w->setMinimumHeight(w->sizeHint().height());
#define MIN_SIZE(w) w->setMinimumSize(w->sizeHint());
#define FIXED_SIZE(w) w->setFixedSize(w->sizeHint());
#define FIXED_WIDTH(w) w->setFixedWidth(w->sizeHint().width());
#define FIXED_HEIGHT(w) w->setFixedHeight(w->sizeHint().height());

#endif
