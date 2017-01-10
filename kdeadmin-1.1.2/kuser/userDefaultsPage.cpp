/**********************************************************************

	--- Qt Architect generated file ---

	File: userDefaultsPage.cpp
	Last generated: Mon Oct 12 00:56:45 1998

 *********************************************************************/

#include "userDefaultsPage.h"

#define Inherited userDefaultsPageData

#include "userDefaultsPage.moc"
#include "userDefaultsPageData.moc"

#include <kapp.h>

#include "globals.h"

userDefaultsPage::userDefaultsPage
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  shell->clear();
  shell->insertItem(i18n("<Empty>"));

  FILE *f = fopen(SHELL_FILE,"r");
  if (f) {
    while (!feof(f)) {
      char s[200];

      fgets(s, 200, f);
      if (feof(f))
        break;

      s[strlen(s)-1]=0;
      if ((s[0])&&(s[0]!='#'))
        shell->insertItem(s);
    }
    fclose(f);
  }
}


userDefaultsPage::~userDefaultsPage()
{
}

const char *userDefaultsPage::getShell() const {
  return shell->currentText();
}

const char *userDefaultsPage::getHomeBase() const {
  return home->text();
}

bool userDefaultsPage::getCreateHomeDir() const {
  return createHomeDir->isChecked();
}

bool userDefaultsPage::getCopySkel() const {
  return copySkel->isChecked();
}

bool userDefaultsPage::getUserPrivateGroup() const {
  return userPrivateGroup->isChecked();
}

void userDefaultsPage::setShell(const char *data) {
  for (int i=0; i<shell->count(); i++)
    if (strcmp(shell->text(i), data) == 0) {
      shell->setCurrentItem(i);
      return;
    }
  
  shell->insertItem(data, 1);
  shell->setCurrentItem(1);
}

void userDefaultsPage::setHomeBase(const char *data) {
  home->setText(data);
}

void userDefaultsPage::setCreateHomeDir(bool data) {
  createHomeDir->setChecked(data);
}

void userDefaultsPage::setCopySkel(bool data) {
  copySkel->setChecked(data);
}

void userDefaultsPage::setUserPrivateGroup(bool data) {
  userPrivateGroup->setChecked(data);
}

