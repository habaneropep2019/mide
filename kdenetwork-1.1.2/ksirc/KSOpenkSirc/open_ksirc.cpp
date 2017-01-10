/**********************************************************************

	--- Qt Architect generated file ---

	File: open_ksirc.cpp
	Last generated: Wed Jul 29 16:41:26 1998

 *********************************************************************/

#include "open_ksirc.h"
#include "serverDataType.h"
#include "serverFileParser.h"
#include <qstrlist.h>
#include <qlistbox.h>
#include <stdlib.h>
#include <unistd.h>

#include <qmessagebox.h>
#include <qevent.h>
#include <qkeycode.h>

// #include <qapplication.h>
#include <iostream.h>
#include <kapp.h>

QList<Server> Groups;

#define Inherited open_ksircData

open_ksirc::open_ksirc
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  setCaption( i18n("Connect to Server") );

  // TODO
  // check for existance of ~/.kde/share/apps/ksirc/servers.txt
  // if it don't exist use $KDEDIR/share/apps/ksirc/servers.txt
  // changes are written to ~/.kde/share/apps/ksirc/servers.txt

  QString filename = KApplication::localkdedir()+"/share/apps/ksirc/servers.txt";
  if(access(filename, R_OK) == 0)
    serverFileParser::readDatafile( filename );
  else{
    filename = KApplication::kde_datadir()+"/ksirc/servers.txt";
    serverFileParser::readDatafile( filename );
  }

  Groups.setAutoDelete(TRUE);
  
  // TODO add "Recent" to global listing servers here..
  // Now we read in the Recent group from the config file
  // remove all recent servers first

  for(Server *s = Groups.first(); s != 0x0; s = Groups.next()){
    if(s->group() == QString("Recent")){
      Groups.remove();
    }
  }

  // Add current ones
  KConfig *conf = kapp->getConfig();
  QStrList recent;
  conf->setGroup("ServerList");
  conf->readListEntry("RecentServers", recent);
  char *rs;
  for(rs = recent.first(); rs != 0; rs=recent.next()){
    char *name = strtok(rs, ":");
    char *p =    strtok(NULL, ":");
    if(p == 0x0)
      p = "6667";
    QList<port> rp;
    rp.inSort(new("port") port(p));
    Groups.insert(0, new("Server") Server(QString("Recent"), name, rp,
				QString("Recent Server"), ""));
  }


  ComboB_ServerName->setAutoCompletion( TRUE );
  ComboB_ServerPort->setAutoCompletion( TRUE );

  insertGroupList();

  setGroup("Recent");

  connect(ComboB_ServerGroup, SIGNAL(activated( const char * )),
          this, SLOT(setGroup( const char * )));
  connect(ComboB_ServerName,  SIGNAL(activated( const char * )),
          this, SLOT(setServer( const char * )));

  connect(PB_Connect, SIGNAL(pressed()), this, SLOT(clickConnect()));
  connect(PB_Edit, SIGNAL(pressed()), this, SLOT(clickEdit()));
  connect(PB_Cancel, SIGNAL(pressed()), this, SLOT(clickCancel()));

  PB_Connect->setDefault(TRUE);
  PB_Connect->setAutoDefault(TRUE);

  ComboB_ServerName->setFocus();
  connect(ComboB_ServerName, SIGNAL(enterPressed()), this, SLOT(clickConnect()));
}

// insert a sorted list of groups into ComboB_ServerGroup, note that 
// we want to get the recent servers in first so we insert "Recent" 
// in first, then we want Random.

void open_ksirc::insertGroupList()
{
  QStrList tempgroups;
  Server *serv; 
 
  for ( serv=Groups.first(); serv != 0; serv=Groups.next() ) { 
    if (tempgroups.find(serv->group()) == -1)
      tempgroups.inSort( serv->group() );
  }

  ComboB_ServerGroup->insertItem("Recent");
  ComboB_ServerGroup->insertItem("Random");
  for (const char* t = tempgroups.first(); t; t = tempgroups.next()) {
    ComboB_ServerGroup->insertItem( t );
  }
}

// insert a sorted list of servers from the group passed as an arg
// into ComboB_ServerName, if a list is already there delete it.
// note this does not check for multiple occurrances of the same server

void open_ksirc::insertServerList( const char * group )
{
  QListBox *newListBox = new("QListBox") QListBox();
  Server *serv;

  for ( serv=Groups.first(); serv != 0; serv=Groups.next() ) {
    if (strcmp(serv->group(), group) == 0)
      newListBox->inSort( serv->server() );
  }
  ComboB_ServerName->setListBox(newListBox);
//  ComboB_ServerName->setCurrentItem( 0 ); // this don't work =(
  if (newListBox->count() > 0)
    ComboB_ServerName->setEditText( newListBox->text( 0 ) );
}

// insert a sorted list of ports from the server passed as an arg
// into ComboB_ServerPort, if a list is already there delete it.
// note that this only takes the first occurrance if there is two
// entiies with the same server.

void open_ksirc::setServer( const char * serveraddress )
{
  QListBox *newListBox  = new("QListBox") QListBox();
  Server *serv;
  QList<port> portlist; 
  port *p;
  bool defaultport = FALSE; 

  for ( serv=Groups.first(); serv != 0; serv=Groups.next() ) {
    if (strcmp(serv->server(), serveraddress) == 0) {
      setServerDesc( serv->serverdesc() );
      portlist = serv->ports();
      for ( p=portlist.last(); p != 0; p=portlist.prev() ) {
        newListBox->insertItem( p->portnum() );
        if (strcmp(p->portnum(), "6667") == 0) 
          defaultport = TRUE;
      }
      break;
    }
  }
  ComboB_ServerPort->setListBox(newListBox);
//  ComboB_ServerPort->setCurrentItem( 0 ); // doesn't work
  if (defaultport) {
    ComboB_ServerPort->setEditText("6667");
  } else {
    if (newListBox->count() > 0)
      ComboB_ServerPort->setEditText( newListBox->text( 0 ) );
  }
}

// Sets the server description if the isn't one set it to "Not Available"

void open_ksirc::setServerDesc( QString description )
{
  if (description.isNull() || description.isEmpty()) {
    Label_ServerDesc->setText( i18n("Not Avalable"));
  } else {
    Label_ServerDesc->setText( description );
  }
}

void open_ksirc::setGroup( const char * group )
{
  insertServerList( group );
  if (ComboB_ServerName->count() > 0) {
    setServer( ComboB_ServerName->text( 0 ));
  } else {
    setServerDesc( "" );
    ComboB_ServerPort->setEditText("6667");
    ComboB_ServerPort->insertItem("6667");
  }
  if(ComboB_ServerPort->currentText() == 0x0){
      ComboB_ServerPort->setEditText("6667");
      ComboB_ServerPort->insertItem("6667");
  }
}

void open_ksirc::clickConnect()
{
  QString server;
  int port;
  QString script;
  Server *serv;
  QStrList recent;
  KConfig *conf = kapp->getConfig();

  hide();

  server = ComboB_ServerName->currentText();
  port = atoi(ComboB_ServerPort->currentText());

  for ( serv=Groups.first(); serv != 0; serv=Groups.next() ) {
    if (strcmp(serv->server(), server) == 0) {
      script = serv->script();
    }
    break;
  }

  if(server.length() == 0)
    reject();

  if(port == 0)
    port = 6667;

  QString str = server + ":" + ComboB_ServerPort->currentText();
  conf->setGroup("ServerList");
  conf->readListEntry("RecentServers", recent);
  int found = recent.find(str);
//  debug("Str: %s", str.data());
  if(found == -1){
    recent.insert(0, str);
    conf->writeEntry("RecentServers", recent);
    conf->sync();
  }
  else {
    recent.remove(found);
    recent.insert(0, str);
    conf->writeEntry("RecentServers", recent);
    conf->sync();
  }
  
  emit open_ksircprocess( server, port, script );
  emit open_ksircprocess(str);
  
  accept();
}

void open_ksirc::clickCancel()
{
  reject();
}

void open_ksirc::clickEdit()
{
  // TODO open new("server") server editor
  QMessageBox::information(this, "kSirc - Open Server",
                           "Feature not implemented",
                           "&Ok", "", "", 0, 0);

}

open_ksirc::~open_ksirc()
{
  Groups.clear();
}

#include "open_ksirc.moc"
