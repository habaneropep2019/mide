/**********************************************************************

 User Menu Controller

 $$Id: UserMenuRef.cpp,v 1.4 1998/12/15 04:58:41 asj Exp $$

 Setups up and allow you to configure and change the user menu.  

 QTArch file!

 Function:

   Called with by UserMenuRef(QList<UserControlMenu>, ...) where the
   QList is a menu items on by one.  It allows you update/add/delete
   all the diffrent menu items.


 Interface:

 signals:

   updateMenu, emitted when the menu is changed to the parent can
   update.

 Implementation:

 protected slots:

   newHighlight(int):
     connected to the list box highlight(int) signal.  Updates all
     required line edits and combo menus etc when a highligh changes.

   insertMenu():
     read the the line edits and insert a new("menu") menu into the current
     list box location.

   deleteMenu():
     delete the current item.

   modifyMenu():
     update the currrent menu item to conents of the line edits.

   typeSetActive():
      connects to the combo box so when the type is changed all needed
      updates are done.

   terminate():
      close the window forefully.

 private:

    updateMainListBox():
      "draws" the menu into the QListBox representation.
        
 Varibles:
   *user_menu: a ptr to the user menu being edited.  after changes are
   made that you want to keep emit updateMenu for the parent to
   update.
    

 *********************************************************************/

#include "UserMenuRef.h"

extern KConfig *kConfig;

#define Inherited UserMenuRefData

UserMenuRef::UserMenuRef
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  user_menu = UserControlMenu::parseKConfig();

  updateMainListBox();

  MainListBox->setCurrentItem(0);
  newHighlight(0);

}


UserMenuRef::~UserMenuRef()
{
}

void UserMenuRef::newHighlight(int index)
{

  UserControlMenu *ucm;

  ucm = user_menu->at(index);

  if(ucm->type == UserControlMenu::Text){
    MenuName->setEnabled(TRUE);
    MenuCommand->setEnabled(TRUE);
    MenuOpOnly->setEnabled(TRUE);
    MenuName->setText(ucm->title);
    MenuCommand->setText(ucm->action);
    MenuType->setCurrentItem(UserControlMenu::Text);
    MenuOpOnly->setChecked(ucm->op_only);
  }
  else if(ucm->type == UserControlMenu::Seperator){
    MenuName->setText("");
    MenuCommand->setText("");
    MenuType->setCurrentItem(UserControlMenu::Seperator);
    MenuOpOnly->setChecked(FALSE);

    MenuName->setEnabled(FALSE);
    MenuCommand->setEnabled(FALSE);
    MenuOpOnly->setEnabled(FALSE);
  }
      

}

void UserMenuRef::insertMenu()
{

  int newitem = MainListBox->currentItem() + 1;

  if(MenuType->currentItem() == UserControlMenu::Text){
    user_menu->insert(newitem,
		      new("UserControlMenu") UserControlMenu(qstrdup(MenuName->text()),
					  qstrdup(MenuCommand->text()),
					  -1,
					  (int) UserControlMenu::Text,
					  MenuOpOnly->isChecked()));
  }
  else if(MenuType->currentItem() == UserControlMenu::Seperator){
    user_menu->insert(newitem,
			  new("UserControlMenu)") UserControlMenu);
  }
      
  updateMainListBox();
  MainListBox->setCurrentItem(newitem);
  newHighlight(newitem);
}

void UserMenuRef::updateMainListBox()
{
  UserControlMenu *ucm;

  MainListBox->setAutoUpdate(FALSE);
  MainListBox->clear();

  for(ucm = user_menu->first(); ucm != 0; ucm = user_menu->next()){
    if(ucm->type == UserControlMenu::Text)
      MainListBox->insertItem(ucm->title);
    else if(ucm->type == UserControlMenu::Seperator)
      MainListBox->insertItem("--------");
  }

  MainListBox->setAutoUpdate(TRUE);
  MainListBox->repaint(TRUE);

  UserControlMenu::writeKConfig();  
  emit updateMenu();

}

void UserMenuRef::typeSetActive(int index)
{


  if(index == UserControlMenu::Text){
    MenuName->setEnabled(TRUE);
    MenuCommand->setEnabled(TRUE);
    MenuOpOnly->setEnabled(TRUE);
  }
  else if(index == UserControlMenu::Seperator){
    MenuName->setText("");
    MenuCommand->setText("");
    MenuOpOnly->setChecked(FALSE);
    MenuName->setEnabled(FALSE);
    MenuCommand->setEnabled(FALSE);
    MenuOpOnly->setEnabled(FALSE);
  }
      
}

void UserMenuRef::deleteMenu()
{

  int currentitem = MainListBox->currentItem();

  user_menu->remove(currentitem);

  updateMainListBox();

  MainListBox->setCurrentItem(currentitem);


}

void UserMenuRef::modifyMenu()
{
  int newitem = MainListBox->currentItem();

  user_menu->remove(newitem);

  if(MenuType->currentItem() == UserControlMenu::Text){
    user_menu->insert(newitem,
		      new("UserControlMenu") UserControlMenu(qstrdup(MenuName->text()),
					  qstrdup(MenuCommand->text()),
					  -1,
					  (int) UserControlMenu::Text,
					  MenuOpOnly->isChecked()));
  }
  else if(MenuType->currentItem() == UserControlMenu::Seperator){
    user_menu->insert(newitem,
			  new("UserControlMenu)") UserControlMenu);
  }
      
  updateMainListBox();
  MainListBox->setCurrentItem(newitem);
}
