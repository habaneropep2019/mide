//
//  kjots
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

#include <qapp.h>
#include <qmenubar.h>
#include <qpopmenu.h>
#include <qkeycode.h>
#include <qfile.h>
#include <qtstream.h>
#include <qdir.h>
#include <qlined.h>
#include <qdialog.h>
#include <qmsgbox.h>
#include <qmlined.h>
#include <qfiledlg.h>
#include <qtooltip.h>
#include <qscrbar.h>
#include <qobjcoll.h>
#include <qclipbrd.h>

#include <kapp.h>
#include <ktabctl.h>
#include <kmsgbox.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kaccel.h>
#include <kfiledialog.h>
#include <kkeydialog.h>

extern "C" {
#include <unistd.h>
};

#include "KJotsMain.h"
#include "SubjList.h"
#include "ReadListConf.h"
#include "cpopmenu.h"
#include "confdialog.h"

const unsigned int HOT_LIST_SIZE = 8;
const unsigned int BUTTON_WIDTH = 56;

extern QString exec_http;
extern QString exec_ftp;

extern KIconLoader *global_pix_loader;

//----------------------------------------------------------------------
// ASKFILENAME
//----------------------------------------------------------------------

AskFileName::AskFileName(QWidget* parent, const char* name)
  : QDialog(parent, name, TRUE)
{
  initMetaObject();
  resize(300, 80);
  QLabel *l_name = new QLabel( klocale->translate("Book name:"), this );
  i_name = new QLineEdit( this );
  QPushButton *b_ok = new QPushButton(klocale->translate("OK"), this);
  QPushButton *b_cancel = new QPushButton(klocale->translate("Cancel"), this);

  l_name->setGeometry(10, 10, 80, 24);
  i_name->setGeometry(100, 10, 190, 24);
  b_ok->setGeometry(20, 44, 60, 24);
  b_ok->setDefault(TRUE);
  b_cancel->setGeometry(220, 44, 60, 24);

  i_name->setFocus();
  connect( b_ok, SIGNAL(clicked()), this, SLOT(accept()) );
  connect( b_cancel, SIGNAL(clicked()), this, SLOT(reject()) );
}

//----------------------------------------------------------------------
// MYMULTIEDIT
//----------------------------------------------------------------------

MyMultiEdit::MyMultiEdit (QWidget* parent, const char* name)
  : QMultiLineEdit(parent, name)
{
  initMetaObject();
  web_menu = new CPopupMenu;
  web_menu->insertItem(klocale->translate("Open URL"), this, SLOT(openUrl()) );
}

void MyMultiEdit::keyPressEvent( QKeyEvent *e )
{
  if( e->key() == Key_Tab )
    {
      insertChar('\t');
      return;
    }
  QMultiLineEdit::keyPressEvent(e);
  return;
}

void MyMultiEdit::mousePressEvent( QMouseEvent *e )
{
  if( e->button() == RightButton )
    {
      if( hasMarkedText() )
	{
	  QString marked = markedText();
	  if( marked.left(7) == "http://" || marked.left(6) == "ftp://" )
	    {
	      web_menu->popup(QCursor::pos());
	      web_menu->grabMouse();
	    }
	}
      return;
    }
  QMultiLineEdit::mousePressEvent(e);
  return;
}

void MyMultiEdit::openUrl()
{
  QString command;
  int pos;
  if( hasMarkedText() )
    {
      QString marked = markedText();
      if( marked.left(7) == "http://" )
	{
	  command = exec_http;
	  command.detach();
	  if( (pos = exec_http.find("%u")) == -1 )
	    {
	      command += " " + marked + " &";
	    }
	  else
	    {
	      command.remove(pos, 2);
	      command = command.insert(pos, marked);
	      command += " &";
	    }
	  system((const char *) command);
	  //debug("exec: %s", (const char *) command );
	}
      else if( marked.left(6) == "ftp://" )
	{
	  command = exec_ftp;
	  command.detach();
	  if( (pos = exec_ftp.find("%u")) == -1 )
	    {
	      command += " " + marked + " &";
	    }
	  else
	    {
	      command.remove(pos, 2);
	      command = command.insert(pos, marked);
	      command += " &";
	    }
	  system((const char *) command);
	  //debug("exec: %s", (const char *) command );
	}
    }
}

//----------------------------------------------------------------------
// MYBUTTONGROUP
//----------------------------------------------------------------------

MyButtonGroup::MyButtonGroup (QWidget* parent, const char* name)
  : QButtonGroup(parent, name)
{
  initMetaObject();
}

void MyButtonGroup::resizeEvent( QResizeEvent * )
{
  int x = width()-2;
  QObjectList  *list = queryList( "QPushButton" );
  QObjectListIt it( *list );
  while ( it.current() ) {  
    x -= (BUTTON_WIDTH+4);
    ((QPushButton *) it.current())->move(x, 4);
    ++it;
  }
  delete list;  
}

//----------------------------------------------------------------------
// KJOTSMAIN
//----------------------------------------------------------------------

KJotsMain::KJotsMain(const char* name)
  : KTopLevelWidget( name )
{
  //create widgets
  f_main = new QFrame( this, "Frame_0" );
  f_main->move(0, 28);
  f_main->setMinimumSize( 500, 180 );
  f_main->setFrameStyle( 0 );

  f_text = new QFrame( f_main, "Frame_1" );
  f_text->setGeometry( 8, 72, 452, 45 );
  f_text->setFrameStyle( 50 );
  
  f_labels = new QFrame( f_main, "Frame_2" );
  f_labels->setMinimumSize( 436, 24 );
  f_labels->setFrameStyle( 0 );
  
  menubar = new KMenuBar( this, "MenuBar_1" );
  // KMenubar is not a FRAME!!! (sven)
  //menubar->setFrameStyle( 34 );
  //menubar->setLineWidth( 2 );

  s_bar = new QScrollBar( f_main, "ScrollBar_1" );
  s_bar->setMinimumSize( 452, 16 );
  s_bar->setOrientation( QScrollBar::Horizontal );
  
  me_text = new MyMultiEdit( f_text, "me_text" );
  me_text->setMinimumSize( 436, 30 );
  me_text->insertLine( "" );

  l_folder = new QLabel( f_labels, "Label_4" );
  l_folder->setMinimumSize( 68, 20 );
  l_folder->setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
  l_folder->setText( "" );
  
  le_subject = new QLineEdit( f_labels, "le_subject" );
  le_subject->setMinimumSize( 56, 20 );
  le_subject->setText( "" );
  le_subject->setFocusPolicy(QWidget::ClickFocus);
  
  bg_top = new MyButtonGroup( f_main, "ButtonGroup_2" );
  bg_top->setMinimumSize( 452, 32 );
  bg_top->setFrameStyle( 49 ); 
  bg_top->setTitle( "" );
  bg_top->setAlignment( 1 );
  bg_top->lower();
  
  this->setMinimumSize(500, 211);
  
  KConfig *config = KApplication::getKApplication()->getConfig();

  config->setGroup("kjots");
  entrylist.setAutoDelete(TRUE);
  button_list.setAutoDelete(TRUE);
  folderOpen = FALSE;
  entrylist.append(new TextEntry);
  confdiag = NULL;
  subj_list = new SubjList;
  connect( this, SIGNAL(folderChanged(QList<TextEntry> *)), subj_list,
	   SLOT(rebuildList( QList<TextEntry> *)) );
  connect( this, SIGNAL(entryMoved(int)), subj_list, SLOT( select(int)) );
  connect( subj_list, SIGNAL(entryMoved(int)), this, SLOT( barMoved(int)) );
  connect( le_subject, SIGNAL(textChanged(const char *)), subj_list, 
	   SLOT(entryChanged(const char*)) );
  me_text->setEnabled(FALSE);
  le_subject->setEnabled(FALSE);
  current = 0;
  connect( s_bar, SIGNAL(valueChanged(int)), this, SLOT(barMoved(int)) );
 
  top2bottom = new QGridLayout( f_main, 4, 1, 4 );
  top2bottom->addWidget( f_text, 0, 0, AlignCenter );
  top2bottom->setRowStretch( 0, 1 );
  top2bottom->addWidget( s_bar, 1, 0, AlignCenter );
  top2bottom->addWidget( bg_top, 2, 0, AlignCenter );
  top2bottom->addWidget( f_labels, 3, 0, AlignCenter );
  top2bottom->activate();

  labels_layout = new QGridLayout( f_labels, 1, 2, 0 );
  labels_layout->addWidget( l_folder, 0, 0, AlignVCenter | AlignLeft );
  labels_layout->addWidget( le_subject, 0, 1, AlignVCenter | AlignLeft );
  labels_layout->setColStretch( 1, 1 );
  labels_layout->activate();

  QFont font_label(l_folder->fontInfo().family());
  font_label.setBold(TRUE);
  l_folder->setFont(font_label);

  f_text_layout = new QGridLayout( f_text, 2, 1, 4 );
  f_text_layout->addWidget( me_text, 0, 0, AlignCenter );
  f_text_layout->setRowStretch( 0, 1 );
  f_text_layout->activate();

  s_bar->setRange(0,0);
  s_bar->setValue(0);
  s_bar->setSteps(1,1);

  bg_top->setExclusive(TRUE);
  me_text->setFocusPolicy(QWidget::StrongFocus);

  // read hotlist
  config->readListEntry( "Hotlist", hotlist );
  while( hotlist.count() > HOT_LIST_SIZE )
    hotlist.removeLast();
  // read list of folders
  config->readListEntry( "Folders", folder_list );

  QString temp;
  folders = new QPopupMenu;
  int i = 0;
  QPushButton *temp_button;
  for( temp = folder_list.first(); !temp.isEmpty(); temp = folder_list.next(), i++ )
    { 
      folders->insertItem(temp, i); 
      if( hotlist.contains(temp) )
	{
	  temp_button = new QPushButton(temp, bg_top);
	  temp_button->setFocusPolicy(QWidget::ClickFocus);
	  temp_button->setToggleButton(TRUE);
	  temp_button->setFixedSize(BUTTON_WIDTH,24);
	  bg_top->insert(temp_button, i);
	  button_list.append(temp_button);
	}
    }
  unique_id = i+1;
  connect( folders, SIGNAL(activated(int)), this, SLOT(openFolder(int)) );
  connect( bg_top, SIGNAL(clicked(int)), this, SLOT(openFolder(int)) );

  updateConfiguration();

  // creat keyboard shortcuts
  // CTRL+Key_J := previous page
  // CTRL+Key_K := next page
  // CTRL+Key_L := show subject list
  // CTRL+Key_A := add new page
  // CTRL+Key_M := move focus

  keys = new KAccel( this ); 


  keys->insertStdItem( KAccel::New, klocale->translate("New Book") ); 


  keys->connectItem( KAccel::New, this, SLOT(createFolder()) );
  keys->connectItem( KAccel::Save , this, SLOT(saveFolder()) );
  keys->connectItem( KAccel::Quit, qApp, SLOT(quit()) );
  keys->connectItem( KAccel::Cut , me_text, SLOT(cut()) );
  keys->connectItem( KAccel::Copy , me_text, SLOT(copyText()) );
  keys->connectItem( KAccel::Paste , me_text, SLOT(paste()) );

  keys->insertItem(i18n("PreviousPage"),    "PreviousPage",    CTRL+Key_J);
  keys->insertItem(i18n("NextPage"),        "NextPage",        CTRL+Key_K);
  keys->insertItem(i18n("ShowSubjectList"), "ShowSubjectList", CTRL+Key_L);
  keys->insertItem(i18n("AddNewPage"),      "AddNewPage",      CTRL+Key_A);
  keys->insertItem(i18n("MoveFocus"),       "MoveFocus",       CTRL+Key_M);
  keys->insertItem(i18n("CopySelection"),   "CopySelection",   CTRL+Key_Y);
  keys->connectItem( "PreviousPage", this, SLOT(prevEntry()) );
  keys->connectItem( "NextPage", this, SLOT(nextEntry()) );
  keys->connectItem( "ShowSubjectList", this, SLOT(toggleSubjList()) );
  keys->connectItem( "AddNewPage", this, SLOT(newEntry()) );
  keys->connectItem( "MoveFocus", this, SLOT(moveFocus()) );
  keys->connectItem( "CopySelection", this, SLOT(copySelection()) );
  keys->readSettings();

  // create menu
  int id;
  QPopupMenu *file = new QPopupMenu;
  id = file->insertItem(klocale->translate("&New Book"), this, SLOT(createFolder()));
  keys->changeMenuAccel(file, id, KAccel::New); 

  file->insertSeparator();
  id = file->insertItem(klocale->translate("Save current book"), this, SLOT(saveFolder()) );
  keys->changeMenuAccel(file, id, KAccel::Save); 
  id = file->insertItem(klocale->translate("Save book to ascii file"), this, SLOT(writeBook()) );
  id = file->insertItem(klocale->translate("Save page to ascii file"), this, SLOT(writePage()) );
  file->insertSeparator();
  id = file->insertItem(klocale->translate("Delete current book"), this, SLOT(deleteFolder()) );
  file->insertSeparator();
  id = file->insertItem(klocale->translate("&Quit"), qApp, SLOT( quit() ));
  keys->changeMenuAccel(file, id, KAccel::Quit); 

  QPopupMenu *edit_menu = new QPopupMenu;

  id = edit_menu->insertItem(klocale->translate("C&ut"),me_text, SLOT(cut()));
  keys->changeMenuAccel(edit_menu, id, KAccel::Cut); 
  id = edit_menu->insertItem(klocale->translate("&Copy") , me_text, SLOT(copyText()) );
  keys->changeMenuAccel(edit_menu, id, KAccel::Copy); 
  id = edit_menu->insertItem(klocale->translate("&Paste"), me_text, SLOT(paste()));
  keys->changeMenuAccel(edit_menu, id, KAccel::Paste); 
  edit_menu->insertSeparator();
  id = edit_menu->insertItem(klocale->translate("&New Page"), this, SLOT(newEntry()) );
  keys->changeMenuAccel(edit_menu, id, "AddNewPage"); 
  id = edit_menu->insertItem(klocale->translate("&Delete Page"), this, SLOT(deleteEntry()) );

  QPopupMenu *options = new QPopupMenu;
  options->insertItem(klocale->translate("&Config"), this, SLOT(configure()) );
  options->insertItem(klocale->translate("Configure &Keys"), this, SLOT(configureKeys()) );

  QPopupMenu *hotlist = new QPopupMenu;
  hotlist->insertItem(klocale->translate("Add current book to hotlist"), 
		      this, SLOT(addToHotlist()) );
  hotlist->insertItem(klocale->translate("Remove current book from hotlist"),
		      this, SLOT(removeFromHotlist()) );

  menubar->insertItem( klocale->translate("&File"), file );
  menubar->insertItem( klocale->translate("&Edit"), edit_menu );
  menubar->insertItem( klocale->translate("Hot&list"), hotlist );
  menubar->insertItem( klocale->translate("&Options"), options );
  menubar->insertItem( klocale->translate("&Books"), folders );
  menubar->insertSeparator();
  QString about = "KJots 0.3.1\n\r(C) ";
  about += (QString) klocale->translate("by") +
    " Christoph Neerfeld\n\rChristoph.Neerfeld@home.ivm.de";
  menubar->insertItem( klocale->translate("&Help"),
		       KApplication::getKApplication()->getHelpMenu(TRUE, about ) );


  config->setGroup("kjots");
  // create toolbar
  toolbar = new KToolBar(this);
  QPixmap temp_pix;
  temp_pix = global_pix_loader->loadIcon("filenew.xpm");
  toolbar->insertButton(temp_pix, 0, SIGNAL(clicked()), this,
		      SLOT(newEntry()), TRUE, i18n("New"));
  temp_pix = global_pix_loader->loadIcon("filedel.xpm");
  toolbar->insertButton(temp_pix, 1, SIGNAL(clicked()), this,
		      SLOT(deleteEntry()), TRUE, i18n("Delete"));
  temp_pix = global_pix_loader->loadIcon("back.xpm");
  toolbar->insertButton(temp_pix, 2, SIGNAL(clicked()), this,
		      SLOT(prevEntry()), TRUE, i18n("Previous"));
  temp_pix = global_pix_loader->loadIcon("forward.xpm");
  toolbar->insertButton(temp_pix, 3, SIGNAL(clicked()), this,
		      SLOT(nextEntry()), TRUE, i18n("Next"));
  toolbar->insertSeparator();
  temp_pix = global_pix_loader->loadIcon("openbook.xpm");
  toolbar->insertButton(temp_pix, 4, SIGNAL(clicked()), this,
		      SLOT(toggleSubjList()), TRUE, i18n("Subject List"));
  toolbar->insertSeparator();
  temp_pix = global_pix_loader->loadIcon("exit.xpm");
  toolbar->setBarPos( (KToolBar::BarPosition) config->readNumEntry("ToolBarPos") );
  addToolBar(toolbar);
  setView(f_main, FALSE);
  setMenu(menubar);
  enableToolBar(KToolBar::Show);

  QString last_folder = config->readEntry("LastOpenFolder");
  int nr;
  if( (nr = folder_list.find(last_folder)) >= 0 )
    openFolder(nr);
  int width, height;
  width = config->readNumEntry("Width");
  height = config->readNumEntry("Height");
  if( width < minimumSize().width() )
    width = minimumSize().width();
  if( height < minimumSize().height() )
    height = minimumSize().height();
  resize(width, height);
}


KJotsMain::~KJotsMain()
{
  saveProperties( (KConfig *) 0 );
}

void KJotsMain::saveProperties(KConfig*)
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("kjots");
  button_list.clear();
  if( folderOpen )
    {
      QFileInfo fi(current_folder_name);
      config->writeEntry("LastOpenFolder", fi.fileName());
    }
  saveFolder();
  config->writeEntry("Width", width());
  config->writeEntry("Height", height());
  config->writeEntry("ToolBarPos", (int) toolbar->barPos() );
  config->sync();
}

void KJotsMain::resizeEvent( QResizeEvent *e )
{
  KTopLevelWidget::resizeEvent( e );
}

int KJotsMain::readFile( QString name )
{
  int pos;
  QString buf, subj;
  TextEntry *entry;
  entrylist.clear();
  QFile folder(name);
  if( !folder.open(IO_ReadWrite) )
    return -1;
  QTextStream st( (QIODevice *) &folder);
  buf = st.readLine();
  if( buf.left(9) != "\\NewEntry" )
    return -1;
  entry = new TextEntry;
  entrylist.append( entry );
  subj = buf.mid(10, buf.length() );
  entry->subject = subj.isNull() ? (QString) "" : (QString) subj;
  while( !st.eof() )
    {
      buf = st.readLine();
      if( buf.left(9) == "\\NewEntry" )
	{
	  entry = new TextEntry;
	  entrylist.append(entry);
	  subj = buf.mid(10, buf.length() );
	  entry->subject = subj.isNull() ? (QString) "" : (QString) subj;
	  buf = st.readLine();
	}
      pos = 0;
      while( (pos = buf.find( '\\', pos )) != -1 )
	if( buf[++pos] == '\\' )
	  buf.remove( pos, 1 );
      entry->text.append( buf );
      entry->text.append("\n");
    }
  folder.close();
  while( entry->text.right(1) == "\n" )
    entry->text.truncate(entry->text.length() - 1);
  entry->text.append("\n");
  return 0;
}

int KJotsMain::writeFile( QString name )
{
  int pos;
  TextEntry *entry;
  QString buf;
  QFile folder(name);
  if( !folder.open(IO_WriteOnly | IO_Truncate) )
    return -1;
  QTextStream st( (QIODevice *) &folder);
  for( entry = entrylist.first(); entry != NULL; entry = entrylist.next() )
    {
      st << "\\NewEntry ";
      st << entry->subject;
      st << "\n";
      buf = entry->text;
      buf.detach();
      pos = 0;
      while( (pos = buf.find( '\\', pos )) != -1 )
	{
	  buf.insert( ++pos, '\\' );
	  pos++;
	}
      st << buf;
      if( buf.right(1) != "\n" )
	st << '\n';
    }
  folder.close();
  return 0;
}

void KJotsMain::openFolder(int id)
{
  QPushButton *but;
  for( but = button_list.first(); but != NULL; but = button_list.next() )
    but->setOn(FALSE);
  but = (QPushButton *) bg_top->find(id);
  if( but )
    but->setOn(TRUE);
  //QDir dir = QDir::home();
  //dir.cd(".kde/share/apps/kjots");
  QDir dir = QDir( KApplication::localkdedir().data() );
  dir.cd("share/apps/kjots");
  QString file_name = dir.absPath();
  file_name += '/';
  file_name += folder_list.at( folders->indexOf(id) );
  if( current_folder_name == file_name )
    return;
  if( folderOpen )
    saveFolder();
  current_folder_name = file_name;
  if( readFile(current_folder_name) < 0)
    {
      folderOpen = FALSE;
      debug("Kjots: Unable to open folder");
      return;
    }
  current = 0;
  me_text->deselect();
  me_text->setText(entrylist.first()->text);
  emit folderChanged(&entrylist);
  emit entryMoved(current);
  le_subject->setText(entrylist.first()->subject);
  folderOpen = TRUE;
  l_folder->setText( folder_list.at(folders->indexOf(id)) );
  me_text->setEnabled(TRUE);
  le_subject->setEnabled(TRUE);
  me_text->setFocus();
  s_bar->setRange(0,entrylist.count()-1);
  s_bar->setValue(0);
}

void KJotsMain::createFolder()
{
  AskFileName *ask = new AskFileName(this);
  if( ask->exec() == QDialog::Rejected )
    return;
  QString name = ask->getName();
  delete ask;
  if( folder_list.contains(name) )
    {
      QMessageBox::message(klocale->translate("Warning"), 
			   klocale->translate("A book with this name already exists."), 
			   klocale->translate("OK"), this);
      return;
    }
  saveFolder();
  entrylist.clear();
  folderOpen = TRUE;
  me_text->setEnabled(TRUE);
  le_subject->setEnabled(TRUE);
  me_text->setFocus();
  me_text->clear();
  me_text->deselect();
  TextEntry *new_entry = new TextEntry;
  entrylist.append(new_entry);
  new_entry->subject = "";
  current = 0;
  s_bar->setRange(0,0);
  s_bar->setValue(0);
  emit folderChanged(&entrylist);
  emit entryMoved(current);
  le_subject->setText(entrylist.first()->subject);

  folder_list.append(name);
  if( folders->text(folders->idAt(0)) == 0 )
    folders->removeItemAt(0);
  folders->insertItem(name, unique_id++);
  //QDir dir = QDir::home();
  //dir.cd(".kde/share/apps/kjots");
  QDir dir = QDir( KApplication::localkdedir().data() );
  dir.cd("share/apps/kjots");
  current_folder_name = dir.absPath();
  current_folder_name += '/';
  current_folder_name += name;
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("kjots");
  config->writeEntry( "Folders", folder_list );
  config->sync();
  l_folder->setText(name);
  QPushButton *but;
  for( but = button_list.first(); but != 0; but = button_list.next() )
    but->setOn(FALSE);
}

void KJotsMain::deleteFolder()
{
  if( !folderOpen )
    return;
  if( !QMessageBox::query(klocale->translate("Delete current book"),
			  klocale->translate("Are you sure you want to delete the current book ?"),
			  klocale->translate("Yes"), klocale->translate("No"), this) )
    return;
  QFileInfo fi(current_folder_name);
  QDir dir = fi.dir(TRUE);
  QString name = fi.fileName();
  int index = folder_list.find(name);
  if( index < 0 )
    return;
  dir.remove(current_folder_name);
  folder_list.remove(index);
  int id = folders->idAt(index);
  folders->removeItemAt(index);
  if( hotlist.contains(name) )
    {
      hotlist.remove(name);
      QButton *but = bg_top->find(id);
      bg_top->remove(but);
      button_list.remove( (QPushButton *) but );
      resize(size());
    }
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("kjots");
  config->writeEntry( "Folders", folder_list );
  config->writeEntry( "Hotlist", hotlist );
  config->sync();
  entrylist.clear();
  current_folder_name = "";
  folderOpen = FALSE;
  me_text->setEnabled(FALSE);
  me_text->clear();
  me_text->deselect();
  le_subject->setEnabled(FALSE);
  le_subject->setText("");
  emit folderChanged(&entrylist);
  s_bar->setRange(0,0);
  s_bar->setValue(0);
  l_folder->setText("");
  subj_list->repaint(TRUE);
}

void KJotsMain::saveFolder()
{
  if( !folderOpen )
    return;
  entrylist.at(current)->text = me_text->text();  
  entrylist.at(current)->subject = le_subject->text();  
  writeFile(current_folder_name);
}

void KJotsMain::nextEntry()
{
  if( !folderOpen )
    return;
  if( current+1 >= (int) entrylist.count() )
    return;
  entrylist.at(current)->text = me_text->text();
  entrylist.at(current)->subject = le_subject->text();
  me_text->setText( entrylist.at(++current)->text );
  me_text->deselect();
  me_text->repaint();
  emit entryMoved(current);
  le_subject->setText( entrylist.at(current)->subject );
  s_bar->setValue(current);
}

void KJotsMain::prevEntry()
{
  if( !folderOpen )
    return;
  if( current-1 < 0 )
    return;
  entrylist.at(current)->text = me_text->text();
  entrylist.at(current)->subject = le_subject->text();
  me_text->setText( entrylist.at(--current)->text );
  me_text->deselect();
  me_text->repaint();
  emit entryMoved(current);
  le_subject->setText( entrylist.at(current)->subject );
  s_bar->setValue(current);
}

void KJotsMain::newEntry()
{
  if( !folderOpen )
    return;
  entrylist.at(current)->text = me_text->text();  
  entrylist.at(current)->subject = le_subject->text();
  me_text->clear();
  le_subject->setText("");
  TextEntry *new_entry = new TextEntry;
  entrylist.append(new_entry);
  new_entry->subject = "";
  current = entrylist.count()-1;
  s_bar->setRange(0,current);
  s_bar->setValue(current);
  emit folderChanged(&entrylist);
  emit entryMoved(current);
}

void KJotsMain::deleteEntry()
{
  if( !folderOpen )
    return;
  if( entrylist.count() == 0 )
    return;
  else if( entrylist.count() == 1 )
    {
      entrylist.at(0)->text = "";
      entrylist.at(0)->subject = "";
      s_bar->setValue(0);
      me_text->clear();
      le_subject->setText("");
      return;
    }
  entrylist.remove(current);
  if( current >= (int) entrylist.count() - 1 )
    {
      if( current )
      current--;
      s_bar->setValue(current);
      s_bar->setRange(0, entrylist.count()-1 );
    }
  me_text->setText( entrylist.at(current)->text );
  emit entryMoved(current);
  le_subject->setText( entrylist.at(current)->subject );
  s_bar->setRange(0, entrylist.count()-1 );
  emit folderChanged(&entrylist);
}

void KJotsMain::barMoved( int new_value )
{
  if( !folderOpen )
    return;
  if( current == new_value )
    return;
  entrylist.at(current)->text = me_text->text();
  entrylist.at(current)->subject = le_subject->text();
  current = new_value;
  me_text->setText( entrylist.at(current)->text );
  me_text->deselect();
  me_text->repaint();
  emit entryMoved(current);
  le_subject->setText( entrylist.at(current)->subject );
  s_bar->setValue(new_value);
}

void KJotsMain::addToHotlist()
{
  if( hotlist.count() == HOT_LIST_SIZE )
    return;
  QFileInfo fi(current_folder_name);
  QString name = fi.fileName();
  if( hotlist.contains(name) )
    return;
  hotlist.append(name);
  int index = folder_list.find(name);
  if( index < 0 )
    return;
  int id = folders->idAt(index);
  QPushButton *but = new QPushButton(name, bg_top);
  button_list.append(but);
  bg_top->insert(but, id);
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("kjots");
  config->writeEntry( "Hotlist", hotlist );
  config->sync();
  but->setFocusPolicy(QWidget::ClickFocus);
  but->setToggleButton(TRUE);
  but->setFixedSize(BUTTON_WIDTH,24);
  but->show();
  but->setOn(TRUE);
  bg_top->forceResize();
}

void KJotsMain::removeFromHotlist()
{
  QFileInfo fi(current_folder_name);
  QString name = fi.fileName();
  if( !hotlist.contains(name) )
    return;
  hotlist.remove(name);
  int index = folder_list.find(name);
  if( index < 0 )
    return;
  int id = folders->idAt(index);
  QButton *but = bg_top->find(id);
  bg_top->remove(but);
  button_list.remove( (QPushButton *) but );
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("kjots");
  config->writeEntry( "Hotlist", hotlist );
  config->sync();
  bg_top->forceResize();
}

void KJotsMain::toggleSubjList()
{
  if( subj_list->isVisible() )
    {
      subj_list->hide();
    }
  else
    {
      subj_list->resize(width() / 2, height() );
      subj_list->show();
    }
}

void KJotsMain::configure()
{
  if( confdiag )
    return;
  confdiag = new ConfDialog;
  connect( confdiag, SIGNAL(confHide()), this, SLOT(configureHide()) );
  connect( confdiag, SIGNAL(commitChanges()), this, SLOT(updateConfiguration()) );
  confdiag->show();
}

void KJotsMain::configureHide()
{
  if( !confdiag )
    return;
  delete confdiag;
  confdiag = NULL;
}

void KJotsMain::updateConfiguration()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("kjots");
  exec_http = config->readEntry("execHttp");
  exec_ftp = config->readEntry("execFtp");
  QFont font( config->readEntry("EFontFamily"), config->readNumEntry("EFontSize"),
	      config->readNumEntry("EFontWeight"), config->readNumEntry("EFontItalic"),
	      (QFont::CharSet) config->readNumEntry("EFontCharset") );
  me_text->setFont(font);
}

void KJotsMain::writeBook()
{
  saveFolder();
  QString name;
  while( name.isNull() )
    {
      name =  KFileDialog::getSaveFileName();
      if ( name.isNull() )
	return;
      QFileInfo f_info(name);
      if( f_info.exists() )
	{
	  if( KMsgBox::yesNo(this, klocale->translate("File exists !"),
			     klocale->translate("File already exists. \n Do you want to overwrite it ?")) == 2 )
	    name = "";
	}
    }
  QFile ascii_file(name);
  if( !ascii_file.open(IO_WriteOnly | IO_Truncate) )
    return;
  QTextStream st( (QIODevice *) &ascii_file);
  TextEntry *entry;
  for( entry = entrylist.first(); entry != NULL; entry = entrylist.next() )
    {
      writeEntry( st, entry );
    }
  ascii_file.close();
}

void KJotsMain::writePage()
{
  saveFolder();
  QString name;
  while( name.isNull() )
    {
      name =  KFileDialog::getSaveFileName();
      if ( name.isNull() )
	return;
      QFileInfo f_info(name);
      if( f_info.exists() )
	{
	  if( KMsgBox::yesNo(this, klocale->translate("File exists !"),
			     klocale->translate("File already exists. \n Do you want to overwrite it ?")) == 2 )
	    name = "";
	}
    }
  QFile ascii_file(name);
  if( !ascii_file.open(IO_WriteOnly | IO_Truncate) )
    return;
  QTextStream st( (QIODevice *) &ascii_file);
  writeEntry( st, entrylist.at(current) );
  ascii_file.close();
}

void KJotsMain::writeEntry( QTextStream &st, TextEntry *entry )
{
  QString line, buf;
  line.fill('#', entry->subject.length()+2 );
  st << line << '\n';
  st << "# ";
  st << entry->subject << '\n';
  st << line << '\n';
  buf = entry->text;
  st << buf;
  if( buf.right(1) != "\n" )
    st << '\n';
  st << '\n';
}

void KJotsMain::moveFocus( )
{
  if( me_text->hasFocus() )
    le_subject->setFocus();
  else
    me_text->setFocus();
}

void KJotsMain::configureKeys( )
{
  //keys->configureKeys(this);
  if( KKeyDialog::configureKeys( keys ) ) 
    {
      keys->writeSettings();
    }
}

void KJotsMain::copySelection( )
{
  me_text->copyText();
  le_subject->setText(QApplication::clipboard()->text());
}
