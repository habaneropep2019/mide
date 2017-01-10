/*
  Some constants, macros and enums for ksysv
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ENTRY_HEIGHT 20
#define NONE -1
#define MULTI_DRAG "666|Multiple|Files"
#define SPACING 2
#define TRASH_HEIGHT 45
#define VISIBILITY_OFFSET 0
#define BORDERWIDTH 2
#define DELAY 50000

class PopMenu {
 public:
  enum { Cut = 0, Copy = 1, Paste = 2, Properties = 4 };
};

#endif
