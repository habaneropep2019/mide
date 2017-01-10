/*  Yo Emacs, this -*- C++ -*-

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

#ifndef KSV_VIEW_H
#define KSV_VIEW_H

#include "trash.h"
#include <qfileinf.h>
#include <qframe.h>
#include <qpopmenu.h>
#include <qmlined.h>
#include <qgrpbox.h>
#include <kprocess.h>
#include <kiconloader.h>
#include <knewpanner.h>
#include "DragList.h"
#include "ksv_conf.h"
#include "Data.h"

class KSVGui : public QWidget
{
  Q_OBJECT

public:
  KSVGui(QWidget* parent = 0, const char* name = 0);
  ~KSVGui();

protected:
  friend class KSVTopLevel;
  KNewPanner* panner;
  QWidget* content;
  KSVDragList* startRL[7];
  KSVDragList* stopRL[7];
  KSVDragList* scripts;
  QLabel* avaiL;
  QLabel* servL;
  QLabel* rlL[7];
  QLabel* stopL[7];
  QLabel* startL[7];
  QPopupMenu* normalPM;
  QPopupMenu* backPM;
  QPopupMenu* scriptPM;
  KSVTrash* trash;
  QMultiLineEdit* textDisplay;
  KIconLoader* kil;
  KSVConfig* conf;

  void initLList();
  void initLayout();
  void initScripts();
  void initRunlevels();
  void info2Widget( QFileInfo* info, int index );
  void writeToDisk( KSVDragData* _w, int _rl,int _index, bool _start );
  void clearRL( int _rl );
  void initAccel();
  void initPopupMenus();

public:
  KSVDragList* getOrigin();
  QString getLog() const;

public slots:
  void slotDropAction( KDNDDropZone* _zone );
  void slotWriteSysV();
  void reInit();
  void infoOnData(KSVDragData* data);
  void setDisplayScriptOutput(bool val);
  void slotScriptProperties( KSVDragData* data, QPoint _pos );

protected slots:
  void slotChanged();
  void startW();
  void stopW();
  void restartW();
  void slotOutput( KProcess* _p, char* _buffer, int _buflen );
  void slotErr( KProcess* _p, char* _buffer, int _buflen );
  void slotExitedProcess(KProcess* proc);
  void slotToggleOut();
  void slotScriptsNotRemovable();
  void slotDoubleClick( KSVDragData* _data, QPoint _pos );
  void slotExitDuringRestart(KProcess* proc);
  void calcSizeOnce();
  void setLog(QString _txt);
  void appendLog(QString txt);
  void fwdCannotGenerateNumber();
  void updatePanningFactor();
  void fwdSelected( const KSVDragData* );
  void fwdSelectedScripts( const KSVDragData* );
  void reSortRL();
  void pasteAppend();
  
signals:
  void sigChanged();
  void sigUpdateParent();
  void sigRun(QString _name);
  void sigStop();
  void sigNotRemovable();
  void progress(int val);
  void advance(int steps);
  void initProgress(int max, QString label );
  void endProgress();
  void cannotGenerateNumber();
  void selected( const KSVDragData* );
  void selectedScripts( const KSVDragData* );
};

#endif
