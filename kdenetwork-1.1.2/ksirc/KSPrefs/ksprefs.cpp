#include "ksprefs.h"

#include <qlayout.h>
#include <kapp.h>

KSPrefs::KSPrefs(QWidget * parent, const char * name)
  : QDialog(parent, name)
{
  setCaption(i18n("kSirc Prefrences"));

  pTab = new("QTabDialog") QTabDialog(this, "prefstabs");

  // Start Sub Dialog items.
  pGeneral = new("general") general(pTab);
  pStart = new("StartUp") StartUp(pTab);
  pServerChannel = new("ServerChannel") ServerChannel(pTab);
  pMenu = new("UserMenuRef") UserMenuRef(pTab);
  pFilters = new("DefaultFilters") DefaultFilters(pTab);

  pTab->setCancelButton(i18n("Cancel"));
  pTab->setOkButton(i18n("OK"));
  pTab->addTab(pGeneral, i18n("&General"));
  pTab->addTab(pStart, i18n("&StartUp"));
  pTab->addTab(pServerChannel, i18n("Servers/&Channels"));
  pTab->addTab(pMenu, i18n("&User Menu"));
  pTab->addTab(pFilters, i18n("&Default Filters"));

  connect(pTab, SIGNAL(applyButtonPressed()),
          pGeneral, SLOT(slot_apply()));
  connect(pTab, SIGNAL(applyButtonPressed()),
	  pStart, SLOT(slot_apply()));
  connect(pTab, SIGNAL(applyButtonPressed()),
	  pServerChannel, SLOT(slot_apply()));
  connect(pTab, SIGNAL(applyButtonPressed()),
	  pFilters, SLOT(slot_apply()));


  // Connect this one last since it deletes the widget.
  connect(pTab, SIGNAL(applyButtonPressed()),
	  this, SLOT(slot_apply()));
  connect(pTab, SIGNAL(cancelButtonPressed()),
	  this, SLOT(slot_cancel()));
  connect(pTab, SIGNAL(defaultButtonPressed()),
	  this, SLOT(slot_cancel()));

}

KSPrefs::~KSPrefs(){
  delete pTab;
  pTab = 0;
}

void KSPrefs::resizeEvent ( QResizeEvent * )
{
  pTab->setGeometry(0, 0, width(), height());
  pGeneral->resize(pTab->width(),pTab->height());
}

void KSPrefs::slot_apply()
{
  emit update();
  delete this;
}

void KSPrefs::slot_cancel()
{
  emit update();
  delete this;
}

void KSPrefs::hide()
{
  delete this;
}


