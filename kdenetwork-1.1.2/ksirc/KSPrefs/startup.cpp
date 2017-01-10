/**********************************************************************

	--- Qt Architect generated file ---

	File: startup.cpp
	Last generated: Sat Jan 17 13:28:04 1998

 *********************************************************************/

#include "startup.h"

#include <kconfig.h>

extern KConfig *kConfig;

#define Inherited startupdata

StartUp::StartUp
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  QString qsNick, qsRealname;
  kConfig->setGroup("StartUp");
  qsNick = kConfig->readEntry("Nick", "");
  qsRealname = kConfig->readEntry("RealName", "");

  SLE_Nick->setText(qsNick);
  SLE_RealName->setText(qsRealname);

  QString nick, cindex;
  int items;

  B_AddNicks->setEnabled(FALSE);

  kConfig->setGroup("NotifyList");
  items = kConfig->readNumEntry("Number");
  for(int i = 0; i < items; i++){
    cindex.setNum(i);
    nick = "Notify-" + cindex;
    LB_Nicks->insertItem(kConfig->readEntry(nick));
  }
  LB_Nicks->setCurrentItem(0);


}


StartUp::~StartUp()
{
}

void StartUp::slot_apply()
{
  QString qsNick, qsRealname;
  kConfig->setGroup("StartUp");
  qsNick = SLE_Nick->text();
  qsRealname = SLE_RealName->text();

  kConfig->writeEntry("Nick", qsNick);
  kConfig->writeEntry("RealName", qsRealname);

  QString nick, cindex;
  int items;
  kConfig->setGroup("NotifyList");
  items = LB_Nicks->count();
  kConfig->writeEntry("Number", items);
  for(int i = 0; i < items; i++){
    cindex.setNum(i);
    nick = "Notify-" + cindex;
    kConfig->writeEntry(nick, LB_Nicks->text(i));
  }
  kConfig->sync();
}

void StartUp::nicks_delete()
{
  int item = LB_Nicks->currentItem();
  LB_Nicks->removeItem(item);
  if(item < (int) LB_Nicks->count())
    LB_Nicks->setCurrentItem(item);
  else
    LB_Nicks->setCurrentItem(LB_Nicks->count()-1);
}

void StartUp::nicks_add()
{
  int item = LB_Nicks->currentItem()+1;
  LB_Nicks->insertItem(SLE_Nicks->text(), item);
  LB_Nicks->setCurrentItem(item);
  SLE_Nicks->setText("");
}

void StartUp::sle_add_update(const char *)
{
  if(strlen(SLE_Nicks->text()) > 0)
    B_AddNicks->setEnabled(TRUE);
  else
    B_AddNicks->setEnabled(FALSE);
}
