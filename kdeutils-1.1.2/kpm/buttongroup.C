#include "buttongroup.h"

ButtonGroup::ButtonGroup(const char *title, QWidget *parent) :
  QButtonGroup(title, parent) 
{
  QFont f = font();
  f.setBold(TRUE);
  setFont(f);
}

