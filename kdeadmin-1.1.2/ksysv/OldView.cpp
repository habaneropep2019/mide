/*

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/*****************************************
 **                                     **
 **            KSysV                    **
 **            Main Widget              **
 **                                     **
 *****************************************/

#include "IOCore.h"
#include <qtooltip.h>
#include <qpopmenu.h>
#include <qprogdlg.h>
#include <qkeycode.h>
#include <qmsgbox.h>
#include <qgrpbox.h>
#include <qaccel.h>
#include <qscrbar.h>
#include <qlabel.h>
#include <qlayout.h>
#include <unistd.h>
#include <kiconloader.h>
#include <kprocess.h>
#include <kquickhelp.h>
#include "Data.h"
#include "DragList.h"
#include "ksv_core.h"
#include "PropDlg.h"
#include "ReadOnlyDragList.h"
#include "Content.h"
#include "OldView.h"
#include <kapp.h>

// include the meta-object file
#include "OldView.moc"

// define macros
#include "Constants.h"
#define MAX QMAX
#define HALF 5
#define FULL 2*HALF
#define MIN_SIZE(A) A->setMinimumSize(A->sizeHint())

KSVGui::KSVGui(QWidget* parent, const char* name)
  : QWidget(parent, name),
    normalPM(0),
    backPM(0),
    scriptPM(0),
    kil(kapp->getIconLoader()),
    conf(KSVCore::getConfig())
{
  // main content window
  panner = new KNewPanner(this, "Panner", KNewPanner::Horizontal,
			  KNewPanner::Percent, conf->getPanningFactor());
  content = new KSVContent(panner);
  // check panner when content is resized
  connect(content, SIGNAL(sizeChanged()), this, SLOT(updatePanningFactor()));

  KDNDDropZone* dz = new KDNDDropZone(this, DndText);
  connect( dz, SIGNAL(dropAction(KDNDDropZone*)),
	   this, SLOT(slotDropAction(KDNDDropZone*)));

  trash = new KSVTrash(content, "Trash");

  initLList();
  initPopupMenus();
  initLayout();

  calcSizeOnce();
}

KSVGui::~KSVGui() {
  delete normalPM;
  delete backPM;
  delete scriptPM;
}

void KSVGui::slotDropAction( KDNDDropZone* ) {
  // nothing
}

void KSVGui::initLList() {

  avaiL = new QLabel( i18n("Available"), content );
  servL = new QLabel( i18n("&Services"), content );

  // provide quickhelp
  KQuickHelp::add(avaiL,
		  i18n("These are the <bold>available services</bold>\n" \
		       "To start a service, drag it to the <bold>\"Start\"</bold>" \
		       "section of a runlevel.\n\n" \
		       "To stop one, do the same for the <bold>\"Stop\"</bold> section."));
  KQuickHelp::add(servL,
		  i18n("These are the <bold>available services</bold>\n" \
		       "To start a service, drag it to the <bold>\"Start\"</bold>" \
		       "section of a runlevel.\n\n" \
		       "To stop one, do the same for the <bold>\"Stop\"</bold> section."));

  QFont bold_font = QFont(kapp->generalFont);
  bold_font.setBold(TRUE);

  servL->setFont(bold_font);

  scripts = new ROWidgetList(content, "Scripts");
  scripts->setDefaultIcon("exec");

  // setBuddy
  servL->setBuddy(scripts);

  // doubleclick
  connect( scripts, SIGNAL(sigDoubleClick(KSVDragData*, QPoint)),
	   this, SLOT(slotScriptProperties(KSVDragData*, QPoint)));

  // for cut & copy
  connect( scripts, SIGNAL(selected(const KSVDragData*)),
	   this, SLOT(fwdSelectedScripts(const KSVDragData*)));

  for(int i = 0; i<7; ++i) {

    // create QString for label
    QString _label;
    _label.sprintf(i18n("Runlevel &%i"), i);
    // and for the name
    QString _name;
    _name.sprintf(i18n("Runlevel %i"), i);

    rlL[i] = new QLabel(_label, content);
    startL[i] = new QLabel(i18n("Start"), content);

    rlL[i]->setFont(bold_font);

    // create the "START" list:
    startRL[i] = new KSVDragList(content, (_name + " START").data());
    startRL[i]->setDefaultIcon("start");

    stopL[i] = new QLabel(i18n("Stop"), content);

    // create the "STOP" list:
    stopRL[i] = new KSVDragList(content, (_name + " STOP").data());
    stopRL[i]->setDefaultIcon("stop");

    // set the buddy widget for the "Runlevel %i" label... => the corresponding runlevel
    rlL[i]->setBuddy(startRL[i]);

    // when a DragWidget is moved to another WidgetList, it gets removed & deleted correctly
    // if you connect the following signals and slots:

    connect( startRL[i], SIGNAL(sigDroppedFrom(QString, KSVDragData*)),
	     scripts, SLOT(slotRemoveAfterDrop(QString, KSVDragData*)));
    connect( stopRL[i], SIGNAL(sigDroppedFrom(QString, KSVDragData*)),
	     scripts, SLOT(slotRemoveAfterDrop(QString, KSVDragData*)));
    connect( trash, SIGNAL(sigDroppedFrom(QString, KSVDragData*)),
	     startRL[i], SLOT(slotRemoveAfterDrop(QString, KSVDragData*)));
    connect( trash, SIGNAL(sigDroppedFrom(QString, KSVDragData*)),
	     stopRL[i], SLOT(slotRemoveAfterDrop(QString, KSVDragData*)));

    // for cut & copy
    connect( startRL[i], SIGNAL(selected(const KSVDragData*)),
	     this, SLOT(fwdSelected(const KSVDragData*)));
    connect( stopRL[i], SIGNAL(selected(const KSVDragData*)),
	     this, SLOT(fwdSelected(const KSVDragData*)));

    // otherwise some canvases won't be redrawn correctly
    startRL[i]->show();
    stopRL[i]->show();
  }


  // add text-diplay widget
  textDisplay = new QMultiLineEdit(panner, "TextDisplayWidget");
  textDisplay->setReadOnly(TRUE);
  textDisplay->setAutoUpdate(TRUE);
  textDisplay->setFocusPolicy(NoFocus);

  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      connect( startRL[i], SIGNAL(sigDroppedFrom(QString, KSVDragData*)),
	       startRL[j], SLOT(slotRemoveAfterDrop(QString, KSVDragData*)) );
      connect( startRL[i], SIGNAL(sigDroppedFrom(QString, KSVDragData*)),
	       stopRL[j], SLOT(slotRemoveAfterDrop(QString, KSVDragData*)) );
      connect( stopRL[i], SIGNAL(sigDroppedFrom(QString, KSVDragData*)),
	       startRL[j], SLOT(slotRemoveAfterDrop(QString, KSVDragData*)) );
      connect( stopRL[i], SIGNAL(sigDroppedFrom(QString, KSVDragData*)),
	       stopRL[j], SLOT(slotRemoveAfterDrop(QString, KSVDragData*)) );

      // capturing mouse moves
      connect( startRL[i], SIGNAL(mouseMove(int,int)), startRL[j], SLOT(autoScroll(int,int)) );
      connect( startRL[i], SIGNAL(mouseMove(int,int)), stopRL[j], SLOT(autoScroll(int,int)) );
      connect( stopRL[i], SIGNAL(mouseMove(int,int)), startRL[j], SLOT(autoScroll(int,int)) );
      connect( stopRL[i], SIGNAL(mouseMove(int,int)), stopRL[j], SLOT(autoScroll(int,int)) );

      // connect origin sigs & slots
      if (i != j)
	connect( startRL[i], SIGNAL(sigOrigin(bool)), startRL[j], SLOT(setOrigin(bool)) );
      connect( startRL[i], SIGNAL(sigOrigin(bool)), stopRL[j], SLOT(setOrigin(bool)) );
      connect( stopRL[i], SIGNAL(sigOrigin(bool)), startRL[j], SLOT(setOrigin(bool)) );
      if (i != j)
	connect( stopRL[i], SIGNAL(sigOrigin(bool)), stopRL[j], SLOT(setOrigin(bool)) );
    }

    // capturing mouse moves ... also for "Scripts"
    connect( scripts, SIGNAL(mouseMove(int,int)), startRL[i], SLOT(autoScroll(int,int)) );
    connect( scripts, SIGNAL(mouseMove(int,int)), stopRL[i], SLOT(autoScroll(int,int)) );

    // connecting origin things for "Scripts", too
    connect( scripts, SIGNAL(sigOrigin(bool)), startRL[i], SLOT(setOrigin(bool)));
    connect( scripts, SIGNAL(sigOrigin(bool)), stopRL[i], SLOT(setOrigin(bool)));
    connect( startRL[i], SIGNAL(sigOrigin(bool)), scripts, SLOT(setOrigin(bool)));
    connect( stopRL[i], SIGNAL(sigOrigin(bool)), scripts, SLOT(setOrigin(bool)));

    // doubleclick
    connect( startRL[i], SIGNAL(sigDoubleClick(KSVDragData*, QPoint)),
	     this, SLOT(slotDoubleClick(KSVDragData*, QPoint)));
    connect( stopRL[i], SIGNAL(sigDoubleClick(KSVDragData*, QPoint)),
	     this, SLOT(slotDoubleClick(KSVDragData*, QPoint)));

    // something changed
    connect( startRL[i], SIGNAL(sigChanged()), this, SLOT(slotChanged()));
    connect( stopRL[i], SIGNAL(sigChanged()), this, SLOT(slotChanged()));

    // cannot generate sorting number
    connect( startRL[i], SIGNAL(cannotGenerateNumber()),
	     this, SLOT(fwdCannotGenerateNumber()) );
    connect( stopRL[i], SIGNAL(cannotGenerateNumber()),
	     this, SLOT(fwdCannotGenerateNumber()) );

    // use this loop for setting tooltips
    QToolTip::add(startRL[i], QString().sprintf(i18n("Drag here to start services\nwhen entering Runlevel %i"), i).data());
    QToolTip::add(stopRL[i], QString().sprintf(i18n("Drag here to stop services\nwhen entering Runlevel %i"), i).data());
  }

  // scripts should delete drops on trash -- aehm whatever
  connect( trash, SIGNAL(sigDroppedFrom(QString, KSVDragData*)),
	   scripts, SLOT(slotRemoveAfterDrop(QString, KSVDragData*)));
}

void KSVGui::initScripts() {
  QDir scriptDir = QDir(conf->getScriptPath());
  if (!scriptDir.exists())
	return;

  scriptDir.setFilter( QDir::Files | QDir::Hidden |
		       QDir::NoSymLinks | QDir::Executable);

  scriptDir.setSorting( QDir::Name );

  const QFileInfoList *scriptList = scriptDir.entryInfoList();
  QFileInfoListIterator it( *scriptList ); 			                             // create list iterator
  QFileInfo* fi; 							                     // pointer for traversing
  while ( (fi=it.current()) ) {           			                             // for each file...
    scripts->initItem(fi->fileName(),
		      IOCore::relToAbs(conf->getScriptPath(),
				       fi->dirPath(FALSE)),
		      fi->fileName(), 0);
    ++it;        						                             // goto next list element

    // keep GUI alive
    qApp->processEvents();

  }
}

void KSVGui::initRunlevels() {
  for (int i = 0; i < 7; ++i) {
    QString _path = QString(conf->getRLPath().data());
    _path += QString().sprintf("/rc%i.d", i);
    if (!QDir(_path.data()).exists())
	continue;

    QDir d = QDir(_path.data());
    d.setFilter( QDir::Files );
    d.setSorting( QDir::Name );

    const QFileInfoList *rlList = d.entryInfoList();
    QFileInfoListIterator it( *rlList );	 			                     // create list iterator
    QFileInfo* fi; 							                     // pointer for traversing

    // progress
    //    emit progress( 2*i + 17 );
    emit advance(1);

    while ( (fi=it.current()) ) {           			                             // for each file...
      info2Widget( fi, i);
      ++it;        						                             // goto next list element
      // keep GUI alive
      qApp->processEvents();
    }

    // progress
    // emit progress(2*i + 18);
    emit advance(1);
  }
}

void KSVGui::info2Widget( QFileInfo* info, int index ) {
  if (!info->exists())
    return;

  QString l_name = info->readLink();
  QString f_name = info->fileName();

  QFileInfo link_info = QFileInfo(l_name);
  QString l_base = link_info.fileName();

  QString l_path = IOCore::relToAbs(conf->getScriptPath(), link_info.dirPath(FALSE));

  QString name;
  int number;
  IOCore::dissectFilename( f_name, name, number );

  // finally insert the items...
  if ( f_name.left(1) == "S" )
    startRL[index]->initItem( l_base, l_path, name, number );
  else
    stopRL[index]->initItem( l_base, l_path, name, number );
}

void KSVGui::slotWriteSysV() {
  appendLog(i18n("** WRITING CONFIGURATION **\n"));

  for (int i = 0; i < 7; ++i) {
    appendLog(QString().sprintf(i18n("** RUNLEVEL %i **\n"),
				i));

    clearRL(i); // rm changed/deleted entries

    // process "Start"
    int maxIndex = startRL[i]->count();

    for (int j = 0; j < maxIndex; ++j) {
      KSVDragData* cur_w = startRL[i]->at(j);

      if (cur_w->isChanged() || cur_w->isNew())
	writeToDisk(cur_w, i, j, TRUE);
    }

    // process "Stop"
    maxIndex = stopRL[i]->count();

    for (int j = 0; j < maxIndex; ++j) {
      KSVDragData* cur_w = stopRL[i]->at(j);

      if (cur_w->isChanged() || cur_w->isNew())
	writeToDisk(cur_w, i, j, FALSE);
    }

    // an extra blank line
    appendLog("\n");
  }

  // an extra blank line
  appendLog("\n");
}

void KSVGui::writeToDisk(KSVDragData* _w, int _rl, int _index, bool _start) {
  appendLog(IOCore::makeSymlink(_w, _rl, _start));
}

void KSVGui::clearRL(int _rl) {
  QString path = conf->getRLPath();
  path += QString().sprintf("/rc%i.d", _rl);

  QDir d (path);

  KSVList del_start = startRL[_rl]->getDeletedItems();
  KSVList del_stop = stopRL[_rl]->getDeletedItems();

  int max_start = del_start.count();

  for (int i = 0; i < max_start; ++i) {
    KSVDragData* data = del_start.at(i);

    // ugly hack -> don´t try to delete if entry is new (i.e. not save to disk)
    if (data->isNew())
      break;

    QFileInfo tmp (path + QString().sprintf("/S%.2i%s", data->number(), data->currentName().data()));

    appendLog(IOCore::removeFile(&tmp, d));

    int nr = data->number();
    QString name = data->currentName();
    bool remove = FALSE;

    if (data->isNumChanged()) {
      nr = data->oldNumber();
      remove = TRUE;
    }
    if (data->isNameChanged()) {
      name = data->oldName();
      remove = TRUE;
    }

    if (remove) {
      tmp = QFileInfo(path + QString().sprintf("/S%.2i%s", nr, name.data()));
      appendLog(IOCore::removeFile(&tmp, d));
    }
  }


  // keep GUI alive
  qApp->processEvents();

  int max_stop = del_stop.count();

  for (int i = 0; i < max_stop; ++i) {
    KSVDragData* data = del_stop.at(i);

    // ugly, too
    if (data->isNew())
      break;

    QFileInfo tmp (path + QString().sprintf("/K%.2i%s", data->number(), data->currentName().data()));

    appendLog(IOCore::removeFile(&tmp, d));

    if (data->isChanged()) {
      int nr = data->number();
      QString name = data->currentName();

      if (data->isNumChanged())
	nr = data->oldNumber();
      if (data->isNameChanged())
	name = data->oldName();

      tmp = QFileInfo(path + QString().sprintf("/K%.2i%s", nr, name.data()));
 //      tmp = QFileInfo(path + QString().sprintf("/K%.2i%s", data->oldNumber(), data->oldName().data()));
      appendLog(IOCore::removeFile(&tmp, d));
    }
  }

  // keep GUI alive
  qApp->processEvents();

  max_start = startRL[_rl]->count();

  for (int i = 0; i < max_start; ++i) {
    KSVDragData* data = startRL[_rl]->at(i);

    if (data->isNew())
      break;

    if (data->isChanged()) {
      int nr = data->number();
      QString name = data->currentName();

      if (data->isNumChanged())
	nr = data->oldNumber();
      if (data->isNameChanged())
	name = data->oldName();

      QFileInfo tmp = QFileInfo(path + QString().sprintf("/S%.2i%s", nr, name.data()));

      appendLog(IOCore::removeFile(&tmp, d));
    }
  }

  // keep GUI alive
  qApp->processEvents();

  max_stop = stopRL[_rl]->count();

  for (int i = 0; i < max_stop; ++i) {
    KSVDragData* data = stopRL[_rl]->at(i);

    if (data->isNew())
      break;

    if (data->isChanged()) {
      int nr = data->number();
      QString name = data->currentName();

      if (data->isNumChanged())
	nr = data->oldNumber();
      if (data->isNameChanged())
	name = data->oldName();

      QFileInfo tmp = QFileInfo(path + QString().sprintf("/K%.2i%s", nr, name.data()));
      appendLog(IOCore::removeFile(&tmp, d));
    }
  }

  // keep GUI alive
  qApp->processEvents();

}

void KSVGui::reInit() {
  // update GUI
  qApp->processEvents();

  // override cursor
  kapp->setOverrideCursor(waitCursor);

  const int total = 2 * 2 * 7 + 2;
  emit initProgress(total, i18n("Reading configuration: "));

  // update GUI
  qApp->processEvents();

  // sleep for 0.05 seconds to allow windowmanager
  // gain control of progress dialog (won't work neccessarily,
  // but should in most cases (the faster the system, the better :)
  usleep(DELAY);

  // update GUI
  qApp->processEvents();

  int i = 0;
  for (i = 0; i < 7; ++i) {
    startRL[i]->clear();
    //emit progress(2*i + 1);
    emit advance(1);

    // keep GUI alive
    qApp->processEvents();
    usleep(DELAY);

    stopRL[i]->clear();
    //    emit progress(2*i+2);
    emit advance(1);

    // set new colors
    startRL[i]->setNewColor(conf->getNewColor());
    startRL[i]->setChangedColor(conf->getChangedColor());
    stopRL[i]->setNewColor(conf->getNewColor());
    stopRL[i]->setChangedColor(conf->getChangedColor());

    // keep GUI alive
    qApp->processEvents();
    usleep(DELAY);
  }

  scripts->clear();
  //  emit progress(2*i+1);
  emit advance(1);

  // keep GUI alive
  qApp->processEvents();
  usleep(DELAY);

  initScripts();
  //  emit progress(2*i + 2);
  emit advance(1);

  // keep GUI alive
  qApp->processEvents();
  usleep(DELAY);

  initRunlevels();

  // restore cursor
  kapp->restoreOverrideCursor();


  // refresh GUI
  qApp->processEvents();

  // end progress
  emit endProgress();

  // set focus to none
  scripts->setFocus();
}

void KSVGui::slotChanged() {
  emit sigChanged();
}

void KSVGui::infoOnData(KSVDragData* data) {
  KSVPropDlg prop( data, qApp->mainWidget(), "props" );

  connect( &prop, SIGNAL(changed(const KSVDragData*)),
	   this, SLOT(slotChanged()) );
  connect( &prop, SIGNAL(changed(const KSVDragData*)),
	   parent(), SLOT(redrawEntry(const KSVDragData*)) );
  connect( &prop, SIGNAL(numberChanged()),
	   this, SLOT(reSortRL()) );
//   connect( &prop, SIGNAL(nameChanged(QString)), this, SLOT(slotChanged()) );
//   connect( &prop, SIGNAL(numberChanged(int)), this, SLOT(slotChanged()) );
//   connect( &prop, SIGNAL(targetChanged(QString)), this, SLOT(slotChanged()) );

  prop.exec();
}

void KSVGui::stopW() {
  KProcess *_proc = new KProcess();
  QString s_name = getOrigin()->selected()->filenameAndPath();
  _proc->clearArguments();

  *_proc << s_name << "stop";

  connect(_proc, SIGNAL(processExited(KProcess*)), this, SLOT(slotExitedProcess(KProcess*)));
  connect(_proc, SIGNAL(receivedStdout(KProcess*, char*, int)), this, SLOT(slotOutput(KProcess*, char*, int)));
  connect(_proc, SIGNAL(receivedStderr(KProcess*, char*, int)), this, SLOT(slotErr(KProcess*, char*, int)));

  // refresh textDisplay
  appendLog(i18n("** Stopping ") + s_name + " **\n");

  _proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);

  // notify parent
  emit sigRun(s_name + i18n(" stop"));
}		

void KSVGui::startW() {
  KProcess* _proc = new KProcess();
  QString s_name = getOrigin()->selected()->filenameAndPath();
  _proc->clearArguments();

  *_proc << s_name << "start";

  connect(_proc, SIGNAL(processExited(KProcess*)), this, SLOT(slotExitedProcess(KProcess*)));
  connect(_proc, SIGNAL(receivedStdout(KProcess*, char*, int)), this, SLOT(slotOutput(KProcess*, char*, int)));
  connect(_proc, SIGNAL(receivedStderr(KProcess*, char*, int)), this, SLOT(slotErr(KProcess*, char*, int)));

  // refresh textDisplay
  appendLog(i18n("** Starting ") + s_name + " **\n");

  _proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);

  // notify parent
  emit sigRun(s_name + i18n(" start"));
}

void KSVGui::restartW()
{
  // stopping
  KProcess *_proc = new KProcess();
  QString s_name = getOrigin()->selected()->filenameAndPath();
  _proc->clearArguments();

  *_proc << s_name << "stop";

  connect(_proc, SIGNAL(processExited(KProcess*)), this, SLOT(slotExitDuringRestart(KProcess*)));
  connect(_proc, SIGNAL(receivedStdout(KProcess*, char*, int)), this, SLOT(slotOutput(KProcess*, char*, int)));
  connect(_proc, SIGNAL(receivedStderr(KProcess*, char*, int)), this, SLOT(slotErr(KProcess*, char*, int)));

  // refresh textDisplay
  appendLog(i18n("** Re-starting ") + s_name + " **\n");

  _proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);

  // notify parent
  emit sigRun(s_name+ i18n(" restart"));
}	

void KSVGui::slotOutput( KProcess*, char* _buffer, int _buflen) {
  if (_buflen > 0) {
    QString buffer = QString( _buffer, _buflen + 1 );
    buffer[_buflen]='\0';
    appendLog(buffer);
  }
}

void KSVGui::slotErr( KProcess*, char* _buffer, int _buflen) {
  if (_buflen > 0) {
    QString buffer = QString( _buffer, _buflen + 1);
    buffer[_buflen]='\0';
    appendLog(buffer);
  }
}

void KSVGui::slotExitedProcess( KProcess* proc ) {
  appendLog("---------------------\n");

  emit sigStop();
  delete proc;
}

void KSVGui::slotToggleOut() {
  const bool showOut = conf->getShowLog();
  conf->setShowLog(!showOut);

  if (showOut) {
    conf->setPanningFactor(panner->separatorPos());
    panner->setSeparatorPos(100);
  } else
    panner->setSeparatorPos(conf->getPanningFactor());

  const QSize tmp = size();
  resize(tmp.width(), tmp.height() - 1);
  resize(tmp);
}

void KSVGui::slotScriptsNotRemovable() {
  emit sigNotRemovable();
}

void KSVGui::slotDoubleClick( KSVDragData* _data, QPoint) {
  infoOnData(_data);
}

void KSVGui::slotScriptProperties( KSVDragData* data, QPoint )
{
  KSVPropDlg prop( data, qApp->mainWidget(), "props" );
  prop.setEnabled(false);
  prop.exec();
}

void KSVGui::slotExitDuringRestart( KProcess* proc ) {
  delete proc;
  proc = new KProcess(); // necessary because otherwise we still have some
  			 // signals connected that screw up our output
  proc->clearArguments();

  QString s_name = getOrigin()->selected()->filenameAndPath();
  *proc << s_name << "start";

  connect(proc, SIGNAL(processExited(KProcess*)), this, SLOT(slotExitedProcess(KProcess*)));
  connect(proc, SIGNAL(receivedStdout(KProcess*, char*, int)), this, SLOT(slotOutput(KProcess*, char*, int)));
  connect(proc, SIGNAL(receivedStderr(KProcess*, char*, int)), this, SLOT(slotErr(KProcess*, char*, int)));

  proc->start(KProcess::NotifyOnExit, KProcess::AllOutput);
}

void KSVGui::initLayout() {
  QBoxLayout* realTopLayout = new QVBoxLayout(this);
  realTopLayout->addWidget(panner);
  panner->activate(content, textDisplay);
  realTopLayout->activate();

  QBoxLayout* topLayout = new QHBoxLayout(content);

  QBoxLayout* scriptLayout = new QVBoxLayout();
  topLayout->addLayout(scriptLayout, FULL);

  // scripts
  MIN_SIZE(avaiL);
  MIN_SIZE(servL);
  MIN_SIZE(scripts);
  MIN_SIZE(trash);
  trash->setFixedHeight(TRASH_HEIGHT);

  scriptLayout->addWidget(avaiL);
  scriptLayout->addWidget(servL);
  scriptLayout->addWidget(scripts, FULL);
  scriptLayout->addWidget(trash, FULL);

  // runlevels
  for (int i = 0; i < 7; ++i) {
    // a bit of extra spacing
    topLayout->addSpacing(1);

    QBoxLayout* rlLayout = new QVBoxLayout();
    topLayout->addLayout(rlLayout, FULL);

    MIN_SIZE(rlL[i]);
    MIN_SIZE(startL[i]);
    MIN_SIZE(startRL[i]);
    MIN_SIZE(stopL[i]);
    MIN_SIZE(stopRL[i]);

    rlLayout->addWidget(rlL[i]);
    rlLayout->addWidget(startL[i]);
    rlLayout->addWidget(startRL[i], FULL);
    rlLayout->addSpacing(HALF);
    rlLayout->addWidget(stopL[i]);
    rlLayout->addWidget(stopRL[i], FULL);
  }

  topLayout->activate();
}

void KSVGui::calcSizeOnce() {
  // set minimum size
  const int minWidth = SPACING + MAX(avaiL->width(),
				     servL->width()) + SPACING + ( MAX( MAX(rlL[0]->width(), startL[0]->width()),
									stopL[0]->width() ) + SPACING ) * 7;

  setMinimumSize( minWidth, 200 );
}

void KSVGui::initAccel() {
//   QAccel *a = new QAccel( this );        	// create accels for myWindow
//   a->connectItem( a->insertItem(CTRL+Key_P), 	// adds Ctrl+P accelerator
// 		  this,                  	// connected to myWindow's
// 		  SLOT( printDoc() ) );        	// printDoc() slot
}

void KSVGui::initPopupMenus() {
    normalPM = new QPopupMenu();
    normalPM->insertItem( i18n("C&ut"), parent(), SLOT(editCut()) );
    normalPM->setId(0, PopMenu::Cut);
    normalPM->insertItem( i18n("&Copy"), parent(), SLOT(editCopy()) );
    normalPM->setId(1, PopMenu::Copy);
    normalPM->insertItem( i18n("&Paste"), parent(), SLOT(editPaste()) );
    normalPM->setId(2, PopMenu::Paste);
    normalPM->insertSeparator();
    normalPM->insertItem( i18n("P&roperties..."), parent(), SLOT(properties()) );
    normalPM->setId(4, PopMenu::Properties);

    backPM = new QPopupMenu();
    backPM->insertItem( i18n("&Paste"), this, SLOT(pasteAppend()) );
    backPM->setId(0, PopMenu::Paste);

  for (int i = 0; i < 7; ++i) {
    startRL[i]->addPMenu(backPM);
    stopRL[i]->addPMenu(backPM);
    startRL[i]->addItemPMenu(normalPM);
    stopRL[i]->addItemPMenu(normalPM);
  }

  // menu for scripts
  scriptPM = new QPopupMenu();
  scriptPM->insertItem( i18n("&Copy"), parent(), SLOT(editCopy()) );
  scriptPM->setId(0, PopMenu::Copy);
  scriptPM->insertSeparator();
  scriptPM->insertItem( i18n("&Start"), this, SLOT(startW()) );
  scriptPM->insertItem( i18n("S&top"), this, SLOT(stopW()) );
  scriptPM->insertSeparator();
  scriptPM->insertItem( i18n("&Restart"), this, SLOT(restartW()) );
  scriptPM->insertSeparator();
  scriptPM->insertItem( i18n("&Properties..."), parent(), SLOT(scriptProperties()) );

  scripts->addItemPMenu(scriptPM);
}

KSVDragList* KSVGui::getOrigin() {
  KSVDragList* result = 0;

  for (int i = 0; i < 7; ++i) {
    result = startRL[i]->isOrigin() ? startRL[i] : result;
    result = stopRL[i]->isOrigin() ? stopRL[i] : result;
  }

  result = scripts->isOrigin() ? scripts : result;

  return result;
}

void KSVGui::setDisplayScriptOutput(bool val) {
  if (val)
    panner->setSeparatorPos(conf->getPanningFactor());
  else {
    conf->setPanningFactor(panner->separatorPos());
    panner->setSeparatorPos(100);
  }

  const QSize tmp = size();
  resize(tmp.width(), tmp.height() - 1);
  resize(tmp);
}

void KSVGui::setLog(QString _txt) {
  textDisplay->setAutoUpdate(FALSE);
  textDisplay->setText(_txt);
  textDisplay->setAutoUpdate(TRUE);
  textDisplay->setCursorPosition(textDisplay->numLines(), 0, FALSE);
  textDisplay->repaint();
}

void KSVGui::appendLog(QString txt) {
  setLog(getLog() + txt);
}

QString KSVGui::getLog() const {
  return textDisplay->text();
}

void KSVGui::fwdCannotGenerateNumber() {
  emit cannotGenerateNumber();
}

void KSVGui::updatePanningFactor() {
  if (conf->getShowLog() && conf->getPanningFactor() != panner->separatorPos())
    conf->setPanningFactor(panner->separatorPos());
}

void KSVGui::fwdSelected( const KSVDragData* d )
{
  emit selected(d);
}

void KSVGui::fwdSelectedScripts( const KSVDragData* d )
{
  emit selectedScripts(d);
}

void KSVGui::reSortRL()
{
  getOrigin()->reSort();
}

void KSVGui::pasteAppend()
{
  KSVDragList* tmp = getOrigin();
  
  if (tmp)
    {	
      KSVDragData* o = CLIP->getClipboard();
      const int pos = tmp->count();
      
      if (o && (tmp->find(o) > NONE ? tmp->find(o)!=pos : TRUE))
	tmp->insertItem(o, pos, TRUE);
    }	
  else
    fatal("Bug: could not get origin of \"Paste\" event.\n" \
	  "Please notify the maintainer of this program,\n" \
	  "Peter Putzer <putzer@kde.org>.");
}
