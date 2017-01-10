/***************************************************************************
 *                              KASettings.cpp                             *
 *                            -------------------                          *
 *                         Source file for KArchie                         *
 *                  -A programm to display archie queries                  *
 *                                                                         *
 *                KArchie is written for the KDE-Project                   *
 *                         http://www.kde.org                              *
 *                                                                         *
 *   Copyright (C) Oct 1997 Jörg Habenicht                                 *
 *                  E-Mail: j.habenicht@europemail.com                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          * 
 *                                                                         *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
 *                                                                         *
 ***************************************************************************/

//#include "KASettings.h"
#include "KASettings.moc"
//#include "KArchie.h"
#include "KASettingsTypes.h"

#include <qframe.h>
#include <qdialog.h>
#include <qcombo.h> 
#include <qlabel.h>
#include <qlayout.h>
#include <qstrlist.h>
#include <qlistbox.h>
#include <kapp.h>
#include <kintegerline.h>
#include <kspinbox.h>
#include <kconfig.h>

#define LINE_EDIT_WIDTH  50
#define LINE_EDIT_HEIGHT 21
#define FRAME_OFFSET 10

KAArchieSettings::KAArchieSettings(const char *title, QWidget *parent, const char *name)
  :QGroupBox( title, parent, name )
{

  //  debug( "set KAArchieSettings Combobox" );
  hostbox = new QGroupBox( this, "hostbox" );
  hostbox->setFrameStyle( QFrame::NoFrame );
  hostname = new QComboBox( hostbox, "hostname" );
  hostnamelabel = new QLabel( hostname, i18n("&Host"), hostbox, "hostnamelabel" );
  
  timeoutbox = new QGroupBox ( this, "timeoutbox" );
  timeoutbox->setFrameStyle( QFrame::NoFrame );
  timeoutline = new KIntegerLine( timeoutbox, "timeoutline" );
  //  timeoutline->hide();
  //  timeoutline->setText("0000");
  // get the size for displaying "0000"
  //  const QSize lineEditSize = timeoutline->sizeHint();
  //  QFontMetrics *fm = &timeoutline->fontMetrics();
  //  int lineEditHeight = fm->boundingRect('8').height();
  //  timeoutline->setFixedHeight( timeoutline->height() );
  //  timeoutline->show();
  timeoutlabel = new QLabel( timeoutline, i18n("&Timeout (seconds)"), timeoutbox, "timeoutlabel" );
  connect(timeoutline, SIGNAL(returnPressed()),
	  this, SLOT(slotRP()) );

  triesbox = new QGroupBox ( this, "triesbox" );
  triesbox->setFrameStyle( QFrame::NoFrame );
  triesline = new KIntegerLine( triesbox, "triesline" );
  //  QString tmp("set KLineEdit height ");
  //  tmp.setNum( lineEditHeight );
  //  debug( tmp );
  //  triesline->setFixedHeight( triesline->height() );
  trieslabel = new QLabel( triesline, i18n("Maximal &retries"), triesbox, "trieslabel" );
  connect(triesline, SIGNAL(returnPressed()),
	  this, SLOT(slotRP()) );

  doLayout();


  // read initial archie server list
  QStrList archiehostlist;
  KConfig *config = KApplication::getKApplication()->getConfig();
  // get the list of hosts
  KConfigGroupSaver saveGroup( config, "HostConfig" );
  //  archiehostlistnumber = 
  config->readListEntry( "Hosts", archiehostlist );
  QString defaulthost = "archie.sura.net" ;
  if ( archiehostlist.isEmpty() ) {
    archiehostlist.append( defaulthost );
    //    currentHostId = 0;
  }
  hostname->insertStrList( &archiehostlist );
  hostname->adjustSize();


  readConfig();
}

KAArchieSettings::~KAArchieSettings()
{
  //debug("delete hostlabel");
  delete hostnamelabel;
  //debug("delete timeoutlabel");
  delete timeoutlabel;
  //debug("delete trieslabel");
  delete trieslabel;

  //debug("delete hostname");
  delete hostname;
  delete timeoutline;
  delete triesline;

  //debug("delete hostbox");
  //  delete mainbox;
  delete hostbox;
  delete timeoutbox;
  delete triesbox;
}

void 
KAArchieSettings::slotResetSettings()
{
  //  debug( "KAArchieSettings reset settings request" );
  readConfig();
}

void 
KAArchieSettings::slotApplySettings()
{
  //  debug( "KAArchieSettings apply settings request" );
  writeConfig();
}

void KAArchieSettings::drawContents ( QPainter * )
{
  const QRect contRect  = contentsRect();
  const int contwidth   = contRect.width();
  const int contheight  = contRect.height();
  //const contheight= contentsRect().height();
  //  const int frameoffset = 10;

  //debug( "KAArchieSettings draw event called" );
  // finally resize & move the boxes
  //  hostbox->move( frameoffset, contheight/6 - hostbox->height()/2 );
  //  timeoutbox->move( frameoffset, contheight/2 - timeoutbox->height()/2 );
  //  triesbox->move( frameoffset, contheight*5/6 - triesbox->height()/2 );

  hostbox->move( FRAME_OFFSET, contheight/4 );
  timeoutbox->move( FRAME_OFFSET, contheight*3/4 );
  triesbox->move( contwidth/2 + FRAME_OFFSET, contheight*3/4 );

  //  setGeometry( 10, 10, 30, 30 );
  //  resize( sizeHint() );

  //  hostname->show();
}

void  
KAArchieSettings::doLayout()
{
  const int labelwidgetdistance = 5; // for putting some distance between label <-> widget
  int labelwidth,
    maxlabelwidth = 0;
  const QSize lineEditSize(LINE_EDIT_WIDTH, LINE_EDIT_HEIGHT);

  hostnamelabel->adjustSize();
  //  hostnamelabel->move( 30, 50 );
  if ((labelwidth = hostnamelabel->width()) > maxlabelwidth)
    maxlabelwidth = labelwidth; // get the max labelwidth

  timeoutline->setFixedSize( lineEditSize );
  timeoutlabel->adjustSize();
  if ((labelwidth = timeoutlabel->width()) > maxlabelwidth)
    maxlabelwidth = labelwidth;

  triesline->setFixedSize( lineEditSize );
  trieslabel->adjustSize();
  if ((labelwidth = trieslabel->width()) > maxlabelwidth)
    maxlabelwidth = labelwidth;

  // move the widgets
  maxlabelwidth += labelwidgetdistance; // add some distance to the label
  hostname->move( maxlabelwidth, 0 );
  timeoutline->move( maxlabelwidth, 0 );
  triesline->move( maxlabelwidth, 0 );

  // move the labels to the widgets
  hostnamelabel->move(hostname->x()-hostnamelabel->width()-labelwidgetdistance,
		      hostname->height()/2 - hostnamelabel->height()/2 );

  timeoutlabel->move( timeoutline->x()-timeoutlabel->width()-labelwidgetdistance,
		      timeoutline->height()/2 - timeoutlabel->height()/2 );
  trieslabel->move( triesline->x()-trieslabel->width()-labelwidgetdistance,
		    triesline->height()/2 - trieslabel->height()/2 );

  // justify the boxes
  hostbox->adjustSize();
  timeoutbox->adjustSize();
  triesbox->adjustSize();

}

void 
KAArchieSettings::readConfig()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  KConfigGroupSaver saveGroup( config, "HostConfig" );
  uint currentHostId = config->readUnsignedNumEntry( "CurrentHostId", 0 );

  if ((uint)hostname->count() < currentHostId-1) {
    currentHostId = 0;
  }    

  hostname->setCurrentItem( currentHostId );

  timeoutline->setText( config->readEntry( "Timeout", "4" ) );
  triesline->setText( config->readEntry( "Retries", "3" ) );

}

void 
KAArchieSettings::writeConfig()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  KConfigGroupSaver saveGroup( config, "HostConfig" );

  //  debug("write hostnum to %i",hostname->currentItem() );
  config->writeEntry( ConfigEntries::CurrentHostNumEntry,
		      hostname->currentItem(), TRUE );
  //  debug("write current host to %s",hostname->currentText() );
  config->writeEntry( ConfigEntries::CurrentHostEntry,
		      hostname->currentText(), TRUE );
  //  debug("write timeout to %i",timeoutline->value() );
  config->writeEntry( "Timeout",
		      timeoutline->value(), TRUE );
  //  debug("write retries to %i",triesline->value() );
  config->writeEntry( "Retries",
		      triesline->value(), TRUE );
  config->sync();
}

KASearchSettings::KASearchSettings(const char *title, QWidget *parent, const char *name)
  :QGroupBox( title, parent, name )
{
  // setup the main organizer
  //  mainlayout = new QGridLayout( this, 2, 2, 15, 0 );

  // setup the searchlevel
  searchbox = new QGroupBox( this, "searchbox" );
  searchbox->setFrameStyle( QFrame::NoFrame );
  searchmode = new QComboBox( searchbox, "searchmode" );
  //  searchmode->insertStrList(&SearchMode::fullList());
  searchlabel = new QLabel( searchmode, "S&earch Mode", searchbox, "searchlabel" );
  searchlabel->adjustSize();

  // setup the sorttype
  //  sortbox = new QGroupBox( this, "sortbox" );
  //  sortbox->setFrameStyle( QFrame::NoFrame );
  //  sortmode = new QComboBox( sortbox, "sortmode" );
  //  sortlabel = new QLabel( sortmode, "S&ort Mode", sortbox, "sortlabel" );
  //  sortlabel->adjustSize();

  // setup the weightsbox, belongs to sorttype
  //  weightbox = new QGroupBox( this, "weightbox" );
  //  weightbox->setFrameStyle( QFrame::NoFrame );
  //  weightlist = new QListBox( weightbox, "weightlist" );
  //  weightlabel = new QLabel( weightlist, "&Weight", weightbox, "weightlabel" );
  //  weightlabel->adjustSize();

  // setup the nicelevel
  nicebox = new QGroupBox( this, "nicebox" );
  nicebox->setFrameStyle( QFrame::NoFrame );
  nicelevel = new QComboBox( nicebox, "nicelevel" );
  nicelabel = new QLabel( nicelevel, i18n("&Nice Level"), nicebox, "nicelabel" );
  nicelabel->adjustSize();

  // setup the hitslevel
  hitsbox = new QGroupBox( this, "hitsbox" );
  hitsbox->setFrameStyle( QFrame::NoFrame );
  maxhits = new KIntegerLine( hitsbox, "maxhits" );
  hitslabel = new QLabel( maxhits, i18n("max. &Hits"), hitsbox, "hitslabel" );
  hitslabel->adjustSize();
  connect(maxhits, SIGNAL(returnPressed()),
	  this, SLOT(slotRP()) );

  initWidgets();

  doLayout();

  readConfig();
}

KASearchSettings::~KASearchSettings()
{
  delete maxhits;
  delete nicelevel;
  delete searchmode;

  delete hitslabel;
  delete nicelabel;
  delete searchlabel;

  delete hitsbox;
  delete nicebox;
  delete searchbox;
}


void 
KASearchSettings::slotResetSettings()
{
  //  debug( "KASearchSettings reset settings request" );
  readConfig();
}

void 
KASearchSettings::slotApplySettings()
{
  //  debug( "KASearchSettings apply settings request" );
  writeConfig();
}

void 
KASearchSettings::drawContents ( QPainter * )
{
  const QRect contRect  = contentsRect();
  const int contwidth   = contRect.width();
  const int contheight  = contRect.height();
  const int frameoffset = 10;

  // move the boxes around
  searchbox->move( frameoffset, contheight/4 /*- searchbox->height()*/ );
  //  sortbox->move( contwidth/2 + frameoffset, contheight/4 /*- sortbox->height()*/ );
  nicebox->move( frameoffset, contheight*3/4 /*- nicebox->height()*/ );
  hitsbox->move( contwidth/2 + frameoffset, contheight*3/4 /*- hitsbox->height()*/ );
  //  weightbox->move( contwidth/2 + frameoffset, contheight/2 );
}

void 
KASearchSettings::doLayout()
{
  const int labelwidgetdistance = 5; // for putting some distance between label <-> widget
  int labelwidth; // for adjusting the label->Buddy()
  maxlabelwidth = 0;

  const QSize lineEditSize(LINE_EDIT_WIDTH, LINE_EDIT_HEIGHT);
  maxhits->setFixedSize( lineEditSize );

  if ((labelwidth = searchlabel->width()) > maxlabelwidth) // get the max labelwidth
    maxlabelwidth = labelwidth;
  //  if ((labelwidth = sortlabel->width()) > maxlabelwidth)
  //    maxlabelwidth = labelwidth;
  //  if ((labelwidth = weightlabel->width()) > maxlabelwidth)
  //    maxlabelwidth = labelwidth;
  if ((labelwidth = nicelabel->width()) > maxlabelwidth)
    maxlabelwidth = labelwidth;
  if ((labelwidth = hitslabel->width()) > maxlabelwidth)
    maxlabelwidth = labelwidth;
   
  //finally move the label->buddy widgets
  maxlabelwidth += labelwidgetdistance; // add some distance to the label
  searchmode->move( maxlabelwidth, 0 );
  //  sortmode->move( maxlabelwidth, 0 );
  nicelevel->move( maxlabelwidth, 0 );
  maxhits->move( maxlabelwidth, 0 );
  //  weightlist->move( maxlabelwidth, 0 );

  // and the labels relative to the widgets
  searchlabel->move( searchmode->x()-searchlabel->width()-labelwidgetdistance,
		     searchmode->height()/2 - searchlabel->height()/2 );
  //  sortlabel->move( sortmode->x()-sortlabel->width()-labelwidgetdistance,
  //		   sortmode->height()/2 - sortlabel->height()/2 );
  nicelabel->move( nicelevel->x()-nicelabel->width()-labelwidgetdistance,
		   nicelevel->height()/2 - nicelabel->height()/2 );
  hitslabel->move( maxhits->x()-hitslabel->width()-labelwidgetdistance,
		   maxhits->height()/2 - hitslabel->height()/2 );
  //  weightlabel->move( weightlist->x()-weightlabel->width()-labelwidgetdistance,
  //		     weightlabel->height()/2 );

  // put minimumsize for all
  /*  searchbox->setMinimumSize( maxlabelwidth + searchmode->width(),
			     searchmode->height() );
  sortbox->setMinimumSize( maxlabelwidth + sortmode->width(),
			   sortmode->height() );
  nicebox->setMinimumSize( maxlabelwidth + nicelevel->width(),
			   nicelevel->height() );
  hitsbox->setMinimumSize( maxlabelwidth + maxhits->width(),
			   maxhits->height() );*/
  //add all to the window
  /*mainlayout->addWidget( searchbox, 2, 2 );
  mainlayout->addWidget( sortbox, 1, 2 );
  mainlayout->addWidget( nicebox, 2, 1 );
  mainlayout->addWidget( hitsbox, 1, 1 );*/

  //  setMinimumSize( searchbox->minimumSize() + hitsbox->minimumSize() );

  // justify the boxes
  searchbox->adjustSize();
  nicebox->adjustSize();
  hitsbox->adjustSize();
  //  sortbox->adjustSize();
  //  weightbox->adjustSize();
}

void 
KASearchSettings::initWidgets()
{
  searchmode->insertItem(i18n("Exact"));
  searchmode->insertItem(i18n("Substring"));
  searchmode->insertItem(i18n("Substring case insensitive"));
  searchmode->insertItem(i18n("Regular expression"));
  searchmode->insertItem(i18n("Exact, then substring"));
  searchmode->insertItem(i18n("Exact, then substring case insensitive"));
  searchmode->insertItem(i18n("Exact, then regular expression"));
  searchmode->adjustSize();

  nicelevel->insertItem(i18n("Normal"));
  nicelevel->insertItem(i18n("Nice"));
  nicelevel->insertItem(i18n("Very nice"));
  nicelevel->insertItem(i18n("Even more nice"));
  nicelevel->insertItem(i18n("Nicest"));
  nicelevel->adjustSize();

}

void 
KASearchSettings::readConfig()
{
  int tmp;
  KConfig *config = KApplication::getKApplication()->getConfig();
  KConfigGroupSaver saveGroup( config, "SearchConfig" );
  
  maxhits->setValue(config->readNumEntry("hits", 95));

  switch (SearchMode::toMode( ((const char *)config->readEntry("Search", "="))[0] )) {
  case SearchMode::exactregexp: tmp=6 ; break;
  case SearchMode::exactsubcase: tmp=5 ; break;
  case SearchMode::exactsubstr: tmp=4 ; break;
  case SearchMode::regexp: tmp=3 ; break;
  case SearchMode::subcase: tmp=2 ; break;
  case SearchMode::substr: tmp=1 ; break;
  case SearchMode::exact: // fall through
  default: tmp=0 ; break;
  }
  searchmode->setCurrentItem(tmp);

  switch(NiceLevel::toLevel(config->readNumEntry("Nice", 0))) {
  case NiceLevel::nice : tmp=1 ; break;
  case NiceLevel::nicer : // fall through
  case NiceLevel::verynice : tmp=2 ; break;
  case NiceLevel::extremlynice : tmp=3 ; break;
  case NiceLevel::nicest : tmp=4 ; break;
  case NiceLevel::norm : // fall through
  default: tmp=0 ; break;
  }
  nicelevel->setCurrentItem(tmp);

}

void 
KASearchSettings::writeConfig()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  KConfigGroupSaver saveGroup( config, "SearchConfig" );

  config->writeEntry("hits", maxhits->value());

  SearchMode::mode tmpmode;
  switch(searchmode->currentItem()) {
  case 6: tmpmode=SearchMode::exactregexp ; break;
  case 5: tmpmode=SearchMode::exactsubcase ; break;
  case 4: tmpmode=SearchMode::exactsubstr ; break;
  case 3: tmpmode=SearchMode::regexp ; break;
  case 2: tmpmode=SearchMode::subcase ; break;
  case 1: tmpmode=SearchMode::substr ; break;
  case 0: // fall through
  default: tmpmode=SearchMode::exact ; break;
  }
  char input[2];
  input[0] = SearchMode::toChar(tmpmode);
  input[1] = '\0';
  config->writeEntry("Search", input, true, false, false);

  NiceLevel::mode tmpnice;
  switch (nicelevel->currentItem()) {
  case 4: tmpnice=NiceLevel::nicest; break;
  case 3: tmpnice=NiceLevel::extremlynice; break;
  case 2: tmpnice=NiceLevel::verynice; break;
  case 1: tmpnice=NiceLevel::nice; break;
  case 0: // fall through
  default: tmpnice=NiceLevel::norm; break;
  }
  NiceLevel nicelevel(tmpnice);
  config->writeEntry("Nice", nicelevel.toLevel(), true, false, false);

  config->sync();
}


/*
KAUserSettings::KAUserSettings( const char *title, QWidget *parent, const char *name )
  :QGroupBox( title, parent, name )
{

}
*/

KASettings::KASettings(QWidget *parent, const char *name, bool modal, WFlags f )
  //  :QTabDialog( parent, name, modal, f )
  :QDialog( parent, name, modal, f )
{
  //  const int frameoffset = 10;

  //  debug( "setup settingswidget" );
  setCaption( i18n("KArchie Settings") );
  setMinimumSize( 440, 300 );

  frameLayout = new QBoxLayout( this, QBoxLayout::Down, FRAME_OFFSET );

  settingsFrame = new QFrame(this);
  settingsFrame->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
  widgetLayout = new QBoxLayout( settingsFrame, QBoxLayout::Down, FRAME_OFFSET );
  //  debug( "setup archiehost frame" );
  //  archiedialog = new QDialog(this, "archiehost" );
  //  archiebox = new QBoxLayout( archiedialog, QBoxLayout::Down, frameoffset );
  archiehost = new KAArchieSettings(i18n("Archie server settings"), settingsFrame, "archiehost");
  widgetLayout->addWidget( archiehost );
  //  archiehost->show();

  //  debug( "setup searchterms frame" );
  //  searchdialog  = new QDialog(this, "searchterms" );
  //  searchbox = new QBoxLayout( searchdialog, QBoxLayout::Down, frameoffset );
  searchterms  = new KASearchSettings(i18n("Search init"), settingsFrame, "searchterms");
  widgetLayout->addWidget( searchterms );

  frameLayout->addWidget(settingsFrame, 1);

  //  debug( "setup user frame" );
  //  userdialog = new QDialog(this, "user" );
  //  userbox = new QBoxLayout( userdialog, QBoxLayout::Down, frameoffset );
  //  user = new KAUserSettings( "User settings", userdialog, "user" );
  //  userbox->addWidget( user );

  //  debug( "adding tabs" );
  //  addTab( archiedialog, "&Archie server" );
  //  addTab( searchdialog, "&Search" );
  //  addTab( userdialog, "local / &User" );
  //  addTab( archiedialog, "" );
  //  addTab( searchdialog, "" );

  buttonFrame = new QFrame(this);
  //  setCancelButton();
  cancelButton = new QPushButton(i18n("Cancel"), buttonFrame);
  cancelButton->setFixedSize(cancelButton->sizeHint());
  connect ( cancelButton, SIGNAL(clicked()), SLOT(reject()) );
  //  setDefaultButton(i18n("Reset"));
  restoreButton = new QPushButton(i18n("Reset"), buttonFrame);
  restoreButton->setFixedSize(restoreButton->sizeHint());
  connect (restoreButton, SIGNAL(clicked()),
	   archiehost, SLOT( slotResetSettings() ));
  connect (restoreButton, SIGNAL(clicked()),
	   searchterms, SLOT( slotResetSettings() ));
  acceptButton = new QPushButton(i18n("OK"), buttonFrame);
  acceptButton->setFixedSize(acceptButton->sizeHint());
  //  connect ( acceptButton, SIGNAL(clicked()),
  //	    archiehost, SLOT( slotApplySettings() ));
  //  connect ( acceptButton, SIGNAL(clicked()),
  //	    searchterms, SLOT( slotApplySettings() ));
  connect ( acceptButton, SIGNAL(clicked()), SLOT(slotReturnPressed()) );
  //  connect( archiehost, SIGNAL(returnPressed()),
  //	   archiehost, SLOT(slotApplySettings()) );
  //  connect( archiehost, SIGNAL(returnPressed()),
  //	   searchterms, SLOT(slotApplySettings()) );
  //  connect( searchterms, SIGNAL(returnPressed()),
  //	   archiehost, SLOT(slotApplySettings()) );
  //  connect( searchterms, SIGNAL(returnPressed()),
  //	   searchterms, SLOT(slotApplySettings()) );
  connect( archiehost, SIGNAL(returnPressed()), SLOT(slotReturnPressed()) );
  connect( searchterms, SIGNAL(returnPressed()), SLOT(slotReturnPressed()) );

  buttonLayout = new QBoxLayout(buttonFrame, QBoxLayout::RightToLeft /*, FRAME_OFFSET*/ );
  //  widgetLayout->addStretch();
  //  widgetLayout->addLayout(buttonLayout);
  buttonLayout->addWidget(cancelButton, 0, AlignTop);
  buttonLayout->addSpacing(10);
  buttonLayout->addWidget(restoreButton, 0, AlignTop);
  buttonLayout->addSpacing(10);
  buttonLayout->addWidget(acceptButton, 0, AlignTop);
  buttonLayout->addStretch();

  buttonFrame->setFixedHeight(acceptButton->height() /*+ 2*FRAME_OFFSET*/);
  //  buttonFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
  //  frameLayout->addSpacing(1);
  frameLayout->addWidget(buttonFrame);
  //  frameLayout->addSpacing(1);

  //  debug( "connecting tabs" );
  //  connect(this , SIGNAL( applyButtonPressed() ),
  //	  archiehost, SLOT( slotApplySettings() ));
  //  connect(this , SIGNAL( applyButtonPressed() ),
  //	  searchterms, SLOT( slotApplySettings() ));
  //  connect(this , SIGNAL( defaultButtonPressed() ),
  //	  archiehost, SLOT( slotResetSettings() ));
  //  connect(this , SIGNAL( defaultButtonPressed() ),
  //	  searchterms, SLOT( slotResetSettings() ));

  //  connect(archiehost, SIGNAL(returnPressed()),
  //	  this, SLOT(slotReturnPressed()) );
  //  connect(searchterms, SIGNAL(returnPressed()),
  //	  this, SLOT(slotReturnPressed()) );

  //  debug( "setup ready" );
  //  QWidget *tmpwidget = qApp->focusWidget();
  //  debug ("current keyboard focus: %s", tmpwidget?tmpwidget->name():"0");
}

KASettings::~KASettings()
{
  //debug("delete b-layout");
  delete buttonLayout;
  //debug ("delete w-layout");
  delete widgetLayout;

  //debug("delete host-wg");
  delete archiehost;
  //debug("delete search-wg");
  delete searchterms;
  //  delete user;

  //debug("delete a-button");
  delete acceptButton;
  //debug("delete r-button");
  delete restoreButton;
  //debug("delete c-button");
  delete cancelButton;

  //debug("delete b-frame");
  delete buttonFrame;
  //debug("delete s-frame");
  delete settingsFrame;
  //  delete archiebox;
  //  delete searchbox;
  //  delete userbox;

  //  delete archiedialog;
  //  delete searchdialog;
  //  delete userdialog;
  //debug("delete ready");
}

/*void KASettings::slotDisplaySettings()
{
}*/

/*void KASettings::okButton()
{
  debug( "KASettings okButton pressed" );
}

void KASettings::ResetButton()
{
  debug( "KASettings ResetButton pressed" );
}*/

void 
KASettings::slotReturnPressed()
{
  archiehost->slotApplySettings();
  searchterms->slotApplySettings();
  accept();
}
