/**********************************************************************

	--- Qt Architect generated file ---

	File: UserMenuRef.h
	Last generated: Fri Nov 21 13:01:21 1997

 *********************************************************************/

#ifndef UserMenuRef_included
#define UserMenuRef_included

#include "UserMenuRefData.h"

#include "../usercontrolmenu.h"

class UserMenuRef : public UserMenuRefData
{
    Q_OBJECT

public:

    UserMenuRef
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~UserMenuRef();

signals:
     virtual void updateMenu();

protected slots:
     virtual void newHighlight(int);
     virtual void insertMenu();
     virtual void deleteMenu();
     virtual void modifyMenu();
     virtual void typeSetActive(int index);
     virtual void terminate() { close(TRUE); }

private:
      QList<UserControlMenu> *user_menu;

      void updateMainListBox();

};
#endif // UserMenuRef_included
