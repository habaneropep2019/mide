// -*- C++ -*-

//
//  klpq
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.ivm.de or chris@kde.org
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

extern "C" {
#include <unistd.h>
};

#include <qchkbox.h>
#include <qcombo.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qmenubar.h>
#include <qpushbt.h>
#include <qkeycode.h>
#include <qmsgbox.h>
#include <qobjcoll.h>
#include <qfileinf.h>
#include <qdir.h>
#include <qcursor.h>

#include <kprocess.h>
#include <kmsgbox.h>
#include <kfm.h>
#include <drag.h>
#include "klpq.h"
#include "klpq.moc"
#include "queueview.h"
#include "kheader.h"
#include "klpqspooler.h"
#include "FirstStart.h"

Klpq::Klpq(const char* name=0) 
  : KTopLevelWidget( name )
{
  initMetaObject();

  update_delay = 0;                             // means no auto update at all
  kfm = 0;
  setCaption(KApplication::getKApplication()->getCaption());
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("klpq");
  if( config->hasKey("updateDelay") )
    update_delay = config->readNumEntry("updateDelay");

  conf_auto = new ConfAutoUpdate();
  conf_auto->setFreq(update_delay);

  conf_spooler = new SpoolerConfig();

  menubar = new KMenuBar( this, "MenuBar_1" );
  menubar->setGeometry( 0, 0, 620, 28 );
  menubar->setFrameStyle( 34 );
  menubar->setLineWidth( 2 );
  
  f_main = new QFrame( this, "Frame_1" );
  f_main->move( 0, 28 );
//  f_main->setMinimumSize( 520, 370 );
  
  f_top = new QFrame( f_main, "Frame_2" );
//  f_top->setMinimumSize( 520, 28 );
  f_top->setFrameStyle( 49 );
  
  cb_printer = new QComboBox( f_top, "ComboBox_1" );
//  cb_printer->setFixedSize( 120, 20 );
  cb_printer->setFixedSize( cb_printer->sizeHint() );

  l_printer = new QLabel( f_top, "Label_1" );
//  l_printer->setFixedSize( 48, 20 );
  l_printer->setText( klocale->translate("Printer") );
  l_printer->setFixedSize( l_printer->sizeHint() );

  c_queuing = new QCheckBox( f_top, "CheckBox_1" );
//  c_queuing->setFixedSize( 72, 20 );
  c_queuing->setText( klocale->translate("Queuing") );
  c_queuing->setFixedSize( c_queuing->sizeHint() );

  c_printing = new QCheckBox( f_top, "CheckBox_2" );
//  c_printing->setFixedSize( 72, 20 );
  c_printing->setText( klocale->translate("Printing") );
  c_printing->setFixedSize( c_printing->sizeHint() );

  b_update = new QPushButton( f_top, "PushButton_1" );
//  b_update->setFixedSize( 84, 20 );
  b_update->setText( klocale->translate("Update") );
  b_update->setFixedSize( b_update->sizeHint() );  

  b_quit = new QPushButton( f_top, "PushButton_2" );
//  b_quit->setFixedSize( 56, 20 );
  b_quit->setText( klocale->translate("Quit") );
  b_quit->setFixedSize( b_quit->sizeHint() );
  
  f_list = new QFrame( f_main, "Frame_3" );
//  f_list->setMinimumSize( 520, 84 );
  f_list->setFrameStyle( 50 );
  
  lb_list = new MyRowTable( f_list, "ListBox_1" );
  lb_list->setMinimumSize( 504, 60 );
  lb_list->setFrameStyle( 51 );
  lb_list->setLineWidth( 2 );
  lb_list->setAutoDelete(TRUE);

  h_list = new KHeader( f_list, "Header_1", 5, KHeader::Resizable | KHeader::Buttons );
  h_list->setMinimumSize( 504, 20 );  
  h_list->setHeaderLabel( 0, klocale->translate("Rank"), AlignLeft | AlignVCenter );
  h_list->setHeaderLabel( 1, klocale->translate("Owner"), AlignLeft | AlignVCenter );
  h_list->setHeaderLabel( 2, klocale->translate("Job"), AlignLeft | AlignVCenter );
  h_list->setHeaderLabel( 3, klocale->translate("Files"), AlignLeft | AlignVCenter );
  h_list->setHeaderLabel( 4, klocale->translate("Size"), AlignLeft | AlignVCenter );

  connect( h_list, SIGNAL(sizeChanged(int,int)),
	   lb_list, SLOT(setColumnWidth(int,int)) );
  // This connection makes it jumpy and slow (but it works!)
  //      connect( h_list, SIGNAL(sizeChanging(int,int)),
  //                       lb_list, SLOT(setColumnWidth(int,int)) );
  connect( lb_list, SIGNAL(hSliderMoved(int)),
	   h_list, SLOT(setOrigin(int)) );
  connect( lb_list, SIGNAL(rightButtonClicked()), this, SLOT(popupMenu()) );

  f_bottom = new QFrame( f_main, "f_bottom" );
  f_bottom->setMinimumSize( 100, 52 );

  b_remove = new QPushButton( f_bottom, "PushButton_3" );
//  b_remove->setFixedSize( 64, 24 );
  b_remove->setText( klocale->translate("Remove") );
  b_remove->setFixedSize( b_remove->sizeHint() );

  b_make_top = new QPushButton( f_bottom, "PushButton_4" );
//  b_make_top->setFixedSize( 64, 24 );
  b_make_top->setText( klocale->translate("Make Top") );
  b_make_top->setFixedSize( b_make_top->sizeHint() );
  
  lb_status = new QListBox( f_bottom, "ListBox_2" );
  lb_status->setMinimumSize( 312, 44 );
  lb_status->setFrameStyle( 51 );
  lb_status->setLineWidth( 2 );
  
  b_auto = new QPushButton( f_bottom, "PushButton_6" );
//  b_auto->setFixedSize( 48, 24 );
  b_auto->setText( klocale->translate("Auto") );
  b_auto->setToggleButton( TRUE );
  b_auto->setFixedSize( b_auto->sizeHint() );
  
  top2bottom = new QGridLayout( f_main, 3, 1, 4 );
  top2bottom->addWidget( f_top, 0, 0, AlignCenter );
  top2bottom->addWidget( f_list, 1, 0, AlignCenter );
  top2bottom->setRowStretch( 1, 1 );
  top2bottom->addWidget( f_bottom, 2, 0, AlignCenter );
  top2bottom->activate();

  top_layout = new QBoxLayout( f_top, QBoxLayout::LeftToRight, 4 );
  top_layout->addSpacing(2);
  top_layout->addWidget( l_printer );
  top_layout->addWidget( cb_printer );
  top_layout->addSpacing(10);
  top_layout->addWidget( c_queuing );
  top_layout->addSpacing(6);
  top_layout->addWidget( c_printing );
  top_layout->addSpacing(6);
  top_layout->addWidget( b_update );
  top_layout->addStretch(5);
  top_layout->addWidget( b_quit );
  top_layout->addSpacing(2);
  top_layout->activate();

  list_layout = new QGridLayout( f_list, 2, 1, 4 );
  list_layout->addWidget( h_list, 0, 0, AlignCenter );
  list_layout->addWidget( lb_list, 1, 0, AlignCenter );
  list_layout->setRowStretch( 1, 1 );
  list_layout->activate();

  bottom_layout = new QGridLayout( f_bottom, 1, 4, 4 );
  bottom_layout->addWidget( b_remove, 0, 0, AlignCenter );
  bottom_layout->addWidget( b_make_top, 0, 1, AlignCenter );
  bottom_layout->addWidget( lb_status, 0, 2, AlignCenter );
  bottom_layout->setColStretch( 2, 1 );
  bottom_layout->addWidget( b_auto, 0, 3, AlignCenter );
  bottom_layout->activate();

  QPopupMenu *file = new QPopupMenu;
  file->insertItem(klocale->translate("&Quit"), qApp, SLOT(quit()));

  QPopupMenu *conf_menu = new QPopupMenu;
  conf_menu->insertItem(klocale->translate("&Auto update"), this, SLOT(configureAuto()) );
  conf_menu->insertItem(klocale->translate("&Spooler"), this, SLOT(configureSpooler()) );

  QString about;
  about.sprintf(i18n("Klpq %s\n(C) by Christoph Neerfeld\nchris@kde.org"),
		KLPQ_VERSION);
  QPopupMenu *help = KApplication::getKApplication()->getHelpMenu(FALSE, about );

  menubar->insertItem( klocale->translate("&File"), file );
  menubar->insertItem( klocale->translate("&Config"), conf_menu, ALT+Key_C );
  menubar->insertSeparator();
  menubar->insertItem( klocale->translate("&Help"), help, ALT+Key_H );  

  connect( b_quit, SIGNAL(clicked()), qApp, SLOT(quit()) );
  connect( cb_printer, SIGNAL(activated(int)), this, SLOT(printerSelect(int)) );
  connect( c_queuing, SIGNAL(clicked()), this, SLOT(queuing()) );
  connect( c_printing, SIGNAL(clicked()), this, SLOT(printing()) );
  connect( b_update, SIGNAL(clicked()), this, SLOT(update()) );
  connect( b_remove, SIGNAL(clicked()), this, SLOT(remove()) );
  connect( b_make_top, SIGNAL(clicked()), this, SLOT(makeTop()) );
  connect( b_auto, SIGNAL(clicked()), this, SLOT(setAuto()) );

  popup_menu = new QPopupMenu;
  popup_menu->insertItem( klocale->translate("Remove"), this, SLOT(remove()) );
  popup_menu->insertItem( klocale->translate("Make Top"), this, SLOT(makeTop()) );

  int key_id;
  accel = new QAccel(this);
  accel->connectItem( accel->insertItem( Key_U ), this, SLOT(update()) );
  accel->connectItem( accel->insertItem( Key_R ), this, SLOT(remove()) );
  accel->connectItem( accel->insertItem( Key_T ), this, SLOT(makeTop()) );
  key_id = accel->insertItem( Key_A );
  accel->connectItem( key_id, b_auto, SLOT(toggle()) );
  accel->connectItem( key_id, this, SLOT(setAuto()) );
  accel->connectItem( accel->insertItem( Key_Left ), this, SLOT(prevPrinter()) );
  accel->connectItem( accel->insertItem( Key_Right ), this, SLOT(nextPrinter()) );
  //accel->connectItem( accel->insertItem( Key_E ), this, SLOT(queuing()) );
  //accel->connectItem( accel->insertItem( Key_I ), this, SLOT(printing()) );

  lb_list->setFocus();

  setMenu(menubar);
  setView(f_main, FALSE);

  // set up subprocess
  QString temp;
  FirstStart spooler_dialog;
  if( (temp = config->readEntry("Spooler") ) == "BSD" )
    spooler = new SpoolerBsd;
  else if( temp == "PPR" )
    spooler = new SpoolerPpr;
  else if ( temp == "LPRNG" )
    spooler = new SpoolerLprng;
  else
    {
      spooler_dialog.exec();
      temp = spooler_dialog.getSpooler();
      config->writeEntry("Spooler", temp);
      if( temp == "PPR" )
	spooler = new SpoolerPpr;
      else if ( temp == "LPRNG" )
	spooler = new SpoolerLprng;
      else
	spooler = new SpoolerBsd;
    }
  lpc = new KProcess;
  lpc_com = NotRunning;
  connect( lpc, SIGNAL(receivedStdout(KProcess *, char *, int)), 
	   this, SLOT(recvLpc(KProcess *, char *, int)) );
  connect( lpc, SIGNAL(processExited(KProcess *)), 
	   this, SLOT(exitedLpc(KProcess *)) );

  lpq = new KProcess;
  lpq_running = FALSE;
  connect( lpq, SIGNAL(receivedStdout(KProcess *, char *, int)), 
	   this, SLOT(recvLpq(KProcess *, char *, int)) );
  connect( lpq, SIGNAL(processExited(KProcess *)), 
	   this, SLOT(exitedLpq(KProcess *)) );

  // add dropZone
  KDNDDropZone *dropzone = new KDNDDropZone( f_main, DndURL );
  connect( dropzone, SIGNAL(dropAction(KDNDDropZone *)), this, SLOT(urlDroped(KDNDDropZone *)) );

  // resize window
//  f_main->resize( 620, 370 );
//  setMinimumSize(530, 300);

  int max = f_top->width();
  if (f_bottom->width() > max) max = f_bottom->width();
  setMinimumSize( max+10, 300 );

  int width, height;
  config->setGroup("klpq");
  width = config->readNumEntry("Width");
  height = config->readNumEntry("Height");
  if( width < minimumSize().width() )
    width = minimumSize().width();
  if( height < minimumSize().height() )
    height = minimumSize().height(); 

  h_list->setHeaderSize( 0, config->readNumEntry("rankSize", 40 ) );
  h_list->setHeaderSize( 1, config->readNumEntry("ownerSize", 100 ) );
  h_list->setHeaderSize( 2, config->readNumEntry("jobSize", 30 ) );
  h_list->setHeaderSize( 3, config->readNumEntry("filesSize", 238 ) );
  h_list->setHeaderSize( 4, config->readNumEntry("sizeSize", 100 ) );

  b_auto->setOn( config->readBoolEntry("autoOn", TRUE) );
  setAuto();

  resize(width, height);
}

Klpq::~Klpq()
{
  delete lpc;
  delete lpq;
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("klpq");
  config->writeEntry("Width", width());
  config->writeEntry("Height", height());
  config->writeEntry("updateDelay", update_delay);
  config->writeEntry("rankSize", h_list->getHeaderSize(0) );
  config->writeEntry("ownerSize", h_list->getHeaderSize(1) );
  config->writeEntry("jobSize", h_list->getHeaderSize(2) );
  config->writeEntry("filesSize", h_list->getHeaderSize(3) );
  config->writeEntry("sizeSize", h_list->getHeaderSize(4) );
  config->writeEntry("autoOn", b_auto->isOn() );
  config->writeEntry("lastQueue", cb_printer->currentText() );
  config->sync();
}

void Klpq::setLastPrinterCurrent()
{
  if( cb_printer->count() )
    cb_printer->setCurrentItem( cb_printer->count() - 1 );
}

void Klpq::update()
{
  //debug("lpq = %i / lpc = %i", lpq->isRunning(), lpc->isRunning() );
  if( !isEnabled() )
    return;
  lpq_in_buffer.resize(0);
  lpq->clearArguments();

  spooler->updateCommand( lpq, cb_printer->text( cb_printer->currentItem() ) );
  if( (lpq_running = lpq->start( KProcess::NotifyOnExit, KProcess::Stdout )) )
    disable();
}

void Klpq::queuing()
{
  if( !isEnabled() )
    {
      if( c_queuing->isChecked() )
	c_queuing->setChecked(FALSE);
      else
	c_queuing->setChecked(TRUE);
      return;
    }
  lpc->clearArguments();
  spooler->setQueuingCommand( lpc, cb_printer->text( cb_printer->currentItem() ),
			      c_queuing->isChecked() );
  if( lpc->start( KProcess::NotifyOnExit, KProcess::Stdout ) )
    {
      lpc_com = Queuing;
      disable();
    }
}

void Klpq::printing()
{
  if( !isEnabled() )
    {
      if( c_printing->isChecked() )
	c_printing->setChecked(FALSE);
      else
	c_printing->setChecked(TRUE);
      return;
    }
  lpc->clearArguments();
  spooler->setPrintingCommand( lpc, cb_printer->text( cb_printer->currentItem() ), 
			       c_printing->isChecked() );
  if( lpc->start( KProcess::NotifyOnExit, KProcess::Stdout ) )
    {
      lpc_com = Printing;
      disable();
    }
}

void Klpq::remove()
{
  if( !isEnabled() )
    return;
  int i = lb_list->currentItem();
  if ( i < 0 ) return;
  int job = lb_list->jobId(i);
  QString word;
  word.setNum(job);
  QString ask;
  ask.sprintf( klocale->translate("Do you really want to remove job No %d"), job );
  if( KMsgBox::yesNo( this, "Klpq", (const char *) ask ) == 1 )
    {
      QString progname;
      progname = spooler->removeCommand( cb_printer->text( cb_printer->currentItem() ), word );
      system( (const char *) progname);
      update();
    }
}

void Klpq::makeTop()
{
  if( !isEnabled() )
    return;
  int i = lb_list->currentItem();
  if ( i < 0) return;
  int job = lb_list->jobId(i);
  QString word;
  word.setNum(job);
  QString ask;
  ask.sprintf( klocale->translate("Do you really want to move job No\n%d\non top of queue"), job);
  if( KMsgBox::yesNo( this, "Klpq", (const char *) ask) == 1 )
    {
      lpc->clearArguments();
      spooler->makeTopCommand( lpc, cb_printer->text( cb_printer->currentItem() ), word );
      if( lpc->start( KProcess::NotifyOnExit, KProcess::Stdout ) )
	{
	  lpc_com = MakeTop;
	  disable();
	}
    }
}

void Klpq::setAuto()
{
  if( update_delay == 0 )
    {
      b_auto->setOn( FALSE );
      return;
    }
  if( b_auto->isOn() )
    {
      if( cb_printer->count() )
	update();
      QTimer::singleShot(update_delay * 1000, this, SLOT(setAuto()) );
    }
}

void Klpq::startHelp()
{
  KApplication::getKApplication()->invokeHTMLHelp( "", "" );
}

void Klpq::prevPrinter()
{
  if( !isEnabled() )
    return;
  int cur = cb_printer->currentItem();
  if( cur == 0 )
    return;
  cb_printer->setCurrentItem( cur - 1 );
  update();
}

void Klpq::nextPrinter()
{
  if( !isEnabled() )
    return;
  int cur = cb_printer->currentItem();
  if( ++cur == cb_printer->count() )
    return;
  cb_printer->setCurrentItem( cur );
  update();
}

void Klpq::configureAuto()
{
  if( conf_auto->exec() )
    update_delay = conf_auto->getFreq();
  else
    conf_auto->setFreq(update_delay);
}

void Klpq::configureSpooler()
{
  QString temp;
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("klpq");
  conf_spooler->initConfig( config->readEntry("Spooler", "BSD") );
  if( conf_spooler->exec() )
    {
      temp = conf_spooler->saveConfig();
      delete spooler;
      if( temp == "BSD" )
	spooler = new SpoolerBsd;
      else if( temp == "PPR" )
	spooler = new SpoolerPpr;
      else if ( temp == "LPRNG" )
	spooler = new SpoolerLprng;
    }
}

void Klpq::enable()
{
  if( lpc_com != NotRunning || lpq_running )
    return;
  setEnabled(TRUE);
  QApplication::restoreOverrideCursor();
}

void Klpq::disable()
{
  QApplication::setOverrideCursor( waitCursor );
  setEnabled(FALSE);
}

void Klpq::updateList()
{
  QString temp;
  lb_list->setUpdatesEnabled(FALSE);
  lb_status->setUpdatesEnabled(FALSE);
  lb_list->clear();
  lb_status->clear();
  spooler->parseUpdate( lb_list, lb_status, lpq_in_buffer );
  lb_status->setUpdatesEnabled(TRUE);
  lb_list->setUpdatesEnabled(TRUE);
  if( lb_list->count() )
    lb_list->setCurrentItem( 0 );
  lb_list->repaint(FALSE); 
  lb_status->repaint(TRUE); 
  // --- exec lpc
  if( lpc_com == NotRunning )
    {
      lpc->clearArguments();
      spooler->statusCommand( lpc, cb_printer->text( cb_printer->currentItem() ) );
      if( lpc->start( KProcess::NotifyOnExit, KProcess::Stdout ) )
	lpc_com = Status;
    }
}

void Klpq::recvLpq( KProcess *, char *buffer, int len )
{
  if( !lpq_running )
    {
      debug("lpq not running");
      return;
    }
  int i;
  for( i = 0; i < len; i++ )
    {
      lpq_in_buffer += *(buffer+i);
    }
}

void Klpq::recvLpc( KProcess *, char *buffer, int len )
{
  unsigned char c;
  int j = 0;
  QString temp;
  switch(lpc_com) {
  case NotRunning:
    debug("lpc not running");
    return;
  case Status:
    spooler->parseStatus( buffer, len, c_queuing, c_printing );
    break;
  case Queuing:
    if( spooler->parseQueuing( buffer, len ) < 0 )
      QMessageBox::information( this, klocale->translate("Permission denied"),
				klocale->translate("This is a privileged command\n"
"and you do not have permission to execute it.") );
    break;
  case Printing:
    if( spooler->parsePrinting( buffer, len) < 0 )
      QMessageBox::information( this, klocale->translate("Permission denied"),
				klocale->translate("This is a privileged command\n"
"and you do not have permission to execute it.") );
    break;
  case MakeTop:
    while( (c = *(buffer+j)) && j < len )
      {
	j++;
	if ( c >= 128) break;
	if ( c != '\n' )
	  temp += c;
	else
	  {
	    if(temp.contains("Privileged") || temp.contains("permission") )
	      {
		KMsgBox::message( this, klocale->translate("Permission denied"),
				  klocale->translate("This is a privileged command\n"
"and you do not have permission to execute it.") );
		return;
	      }
	    temp = "";
	  }
      }
    break;
  };
}

void Klpq::exitedLpc(KProcess *)
{
  if( lpc_com == Status )
    {
      lpc_com = NotRunning;
      enable();
      return;
    }
  lpc_com = NotRunning;
  enable();
  update();
}

void Klpq::urlDroped(KDNDDropZone *zone)
{
  if( zone->getDataType() != DndURL )
    return;
  QString name;
  QStrListIterator it(zone->getURLList());
  for( ; it.current(); ++it )
    {
      name = (QString) it.current();
      if( name.left(5) != "file:" )
	{
	  remote_files.append( (QString) cb_printer->text(cb_printer->currentItem()) + ":" + name);
	}
      else
	{
	  name = name.remove(0,5);
	  QString progname;
	  progname = (QString) "lpr -P " + cb_printer->text( cb_printer->currentItem() ) + " "
	    + name;
	  system( (const char *) progname);
	}
    }
  update();
  if( !kfm && remote_files.count() )
    {
      // start download of files
      QString pid, current_file;
      pid.setNum(::getpid());
      current_file = "file:/tmp/klpq_download.";
      current_file += pid;
      kfm = new KFM;
      connect( kfm, SIGNAL(finished()), this, SLOT(slotKFMJobDone()) );
      QString temp = remote_files.first();
      temp = temp.remove(0, temp.find(':')+1);
      kfm->copy( temp, current_file );
    }
}

void Klpq::slotKFMJobDone()
{
  QString pid, current_file;
  pid.setNum(::getpid());
  current_file = "/tmp/klpq_download.";
  current_file += pid;
  QString queue = remote_files.first();
  QFileInfo fi(current_file);
  if( fi.exists() )
    {
      queue = queue.left( queue.find(':') );
      QString progname;
      if( queue.isEmpty() )
	progname = (QString) "lpr " + current_file;
      else
	progname = (QString) "lpr -P " + queue + " " + current_file;
      system( (const char *) progname);
      QDir dir("/tmp");
      dir.remove(current_file);
    }
  remote_files.removeFirst();
  if( remote_files.count() != 0 )
    {
      QString temp = remote_files.first();
      temp = temp.remove(0, temp.find(':')+1);
      kfm->copy( temp, "file:" + current_file );
    }
  else
    {
      delete kfm;
      kfm = 0;
      if( !isVisibleToTLW() )
	QApplication::exit();
      QTimer::singleShot( 2000, this, SLOT(update()) );
    }
}

void Klpq::printRemote( QStrList file_list )
{
  remote_files = file_list;
  QString pid, current_file;
  pid.setNum(::getpid());
  current_file = "file:/tmp/klpq_download.";
  current_file += pid;
  kfm = new KFM;
  connect( kfm, SIGNAL(finished()), this, SLOT(slotKFMJobDone()) );
  QString temp = remote_files.first();
  temp = temp.remove(0, temp.find(':')+1);
  kfm->copy( temp, current_file );  
}

void Klpq::popupMenu()
{
  popup_menu->popup(QCursor::pos());
}




