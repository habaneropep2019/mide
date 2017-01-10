/**********************************************************************

	--- Dlgedit generated file ---

	File: editGroup.cpp
	Last generated: Tue Nov 25 21:00:34 1997

 *********************************************************************/

#include <kapp.h>

#include "maindlg.h"
#include "editGroup.moc"
#include "editGroupData.moc"

#define Inherited editGroupData

editGroup::editGroup(KGroup *akg, QWidget* parent, const char* name) 
: Inherited(parent, name) {
  uint i;

  kg = akg;

  for (i = 0; i<kg->count(); i++)
    m_Group->insertItem(kg->user(i));

  for (i = 0; i<users->count(); i++)
    if (!kg->lookup_user(users->user(i)->getName()))
      m_Users->insertItem(users->user(i)->getName());
  
  if (m_Users->count() != 0)
    m_Users->setCurrentItem(0);

  if (m_Group->count() != 0)
    m_Group->setCurrentItem(0);

  setCaption(i18n("Group properties"));
}

editGroup::~editGroup() {
}

void editGroup::ok() {
  kg->clear();

  for (uint i=0; i<m_Group->count(); i++)
    kg->addUser(m_Group->text(i));

  accept();
}

void editGroup::cancel() {
  reject();
}

void editGroup::add() {
  int cur = m_Users->currentItem();

  if (cur == -1)
    return;

  m_Group->insertItem(m_Users->text(cur));
  m_Users->removeItem(cur);

  if (((uint)cur) == m_Users->count())
    m_Users->setCurrentItem(cur-1);
  else
    m_Users->setCurrentItem(cur);

  m_Group->setCurrentItem(m_Group->count()-1);
  m_Group->centerCurrentItem();
}

void editGroup::del() {
  int cur = m_Group->currentItem();

  if (cur == -1)
    return;

  m_Users->insertItem(m_Group->text(cur));
  m_Group->removeItem(cur);

  if (((uint)cur) == m_Group->count())
    m_Group->setCurrentItem(cur-1);
  else
    m_Group->setCurrentItem(cur);
  
  m_Users->setCurrentItem(m_Users->count()-1);
  m_Users->centerCurrentItem();
}

