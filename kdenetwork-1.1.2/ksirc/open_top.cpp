/**********************************************************************

	--- Qt Architect generated file ---

	File: open_top.cpp
	Last generated: Sun Nov 23 17:55:22 1997

 *********************************************************************/

#include "open_top.h"
#include <iostream.h>


#define Inherited open_topData

extern KConfig *kConfig;

open_top::open_top
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( i18n("New Window For") );
	nameSLE->setFocus();
	kConfig->setGroup("ChannelList");
	okButton->setDefault(TRUE);
	okButton->setAutoDefault(TRUE);
	int items = kConfig->readNumEntry("Number");
	QString server;
	QString cindex;
	for(int i = 0; i < items; i++){
	  cindex.setNum(i);
	  server = "Channel-" + cindex;
	  nameSLE->insertItem(kConfig->readEntry(server), -1);
	}
        nameSLE->setCurrentItem(0);
}


open_top::~open_top()
{
  kConfig->setGroup("ChannelList");
  int items = nameSLE->count();
  kConfig->writeEntry("Number", items);
  QString server;
  QString cindex;
  for(int i = 0; i < items; i++){
    cindex.setNum(i);
    server = "Channel-" + cindex;
    kConfig->writeEntry(server, nameSLE->text(i));
  }
}

void open_top::create_toplevel()
{
  hide();
  int item_found = 0;
  for(int i = 0; i < nameSLE->count() ; i++){
    if(strcasecmp(nameSLE->currentText(), nameSLE->text(i)) == 0){
      item_found = 1;
      //cerr << "Found one!\n";
    }
    //cerr << "Checking!\n";
  }
  if(item_found == 0)
    nameSLE->insertItem(nameSLE->currentText(), 0);

  if(strlen(nameSLE->currentText()) > 0)
    emit open_toplevel( ((QString) nameSLE->currentText()).lower());

//  close(TRUE);

}
#include "open_top.moc"
