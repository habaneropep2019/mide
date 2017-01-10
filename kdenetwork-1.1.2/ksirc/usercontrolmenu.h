#ifndef USERCONTROLLERMENU_H
#define USERCONTROLLERMENU_H

#include <qstring.h>
#include <qlist.h>

#include <kconfig.h>

class UserControlMenu {
 public:
  /** 
      Basic constructor, everything defaults to a Seperator.  All data
      is public so there is no problems changing it in the future.
      Data is copied internally and deleted when finished.
  */
  UserControlMenu(char *_title = 0, 
		  char *_action = 0, 
		  int _accel = 0, 
		  int _type = 0, 
		  bool _op_only = FALSE);

  ~UserControlMenu();

  static QList<UserControlMenu> UserMenu;

  static QList<UserControlMenu> *parseKConfig();
  static void writeKConfig();

  /**
    * title in the popup menu
  */
  char *title;
  /** 
    * Action to preform, must be of the format "blah lbah %s blah"
    * where %s will be expanded into the selected nick.
    */
  char *action;

  /**
    * Accelerator key, currently does nothing.
    */
  int accel;
  /** 
    * is this function only available to ops?
    */
  bool op_only;
  /**
    * What type of menu item is this? a Seperator of Text?
    */
  enum itype { Seperator, Text } type;
};


#endif
