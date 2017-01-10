/**********************************************************************

	--- Qt Architect generated file ---

	File: serverchannel.cpp
	Last generated: Sat Jan 17 14:21:43 1998

 *********************************************************************/

#include "serverchannel.h"

#include <kconfig.h>

#define Inherited serverchanneldata

extern KConfig *kConfig;

ServerChannel::ServerChannel
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  QString server, cindex;
  int items;

  B_AddServers->setEnabled(FALSE);
  B_AddChannels->setEnabled(FALSE);

  QStrList recent;
  kConfig->setGroup("ServerList");
  kConfig->readListEntry("RecentServers", recent);
  LB_Servers->insertStrList(&recent, 0);
  LB_Servers->setCurrentItem(0);

  kConfig->setGroup("ChannelList"); 
  items = kConfig->readNumEntry("Number");
  for(int i = 0; i < items; i++){
    cindex.setNum(i);
    server = "Channel-" + cindex;
    LB_Channels->insertItem(kConfig->readEntry(server), 0);
  }
  LB_Channels->setCurrentItem(0);
}


ServerChannel::~ServerChannel()
{
}


void ServerChannel::slot_apply(){
  QString server, cindex;
  int items;

  QStrList recent;
  kConfig->setGroup("ServerList");
  items = LB_Servers->count();
  for(int i = 0; i < items; i++){
    recent.insert(0, LB_Servers->text(i));
  }
  kConfig->writeEntry("RecentServers", recent);

  kConfig->setGroup("ChannelList");
  items = LB_Channels->count();
  kConfig->writeEntry("Number", items);
  for(int i = 0; i < items; i++){
    cindex.setNum(i);
    server = "Channel-" + cindex;
    kConfig->writeEntry(server, LB_Channels->text(i));
  }
  kConfig->sync();
}

void ServerChannel::servers_delete()
{
  int item = LB_Servers->currentItem();
  LB_Servers->removeItem(item);
  if(item < (int) LB_Servers->count())
    LB_Servers->setCurrentItem(item);
  else
    LB_Servers->setCurrentItem(LB_Servers->count()-1);
}

void ServerChannel::channels_delete()
{
  int item = LB_Channels->currentItem();
  LB_Channels->removeItem(item);
  if(item < (int) LB_Channels->count())
    LB_Channels->setCurrentItem(item);
  else
    LB_Channels->setCurrentItem(LB_Channels->count()-1);
}

void ServerChannel::servers_add()
{
  int item = LB_Servers->currentItem()+1;
  LB_Servers->insertItem(SLE_Servers->text(), item);
  LB_Servers->setCurrentItem(item);
  SLE_Servers->setText("");
}


void ServerChannel::channels_add()
{
  int item = LB_Channels->currentItem()+1;
  LB_Channels->insertItem(SLE_Channels->text(), item);
  LB_Channels->setCurrentItem(item);
  SLE_Channels->setText("");
}


void ServerChannel::servers_sle_update(const char *)
{
  if(strlen(SLE_Servers->text()) > 0)
    B_AddServers->setEnabled(TRUE);
  else
    B_AddServers->setEnabled(FALSE);
}

void ServerChannel::channels_sle_update(const char *)
{
  if(strlen(SLE_Channels->text()) > 0)
    B_AddChannels->setEnabled(TRUE);
  else
    B_AddChannels->setEnabled(FALSE);
}
