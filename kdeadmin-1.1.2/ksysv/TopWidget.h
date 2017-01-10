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

#ifndef KSV_TOPWIDGET_H
#define KSV_TOPWIDGET_H

#include <qpopmenu.h>
#include <kprogress.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include <kstatusbar.h>
#include <drag.h>
#include <kconfig.h>
#include <ktmainwindow.h>
#include "prefs.h"
#include "ksv_core.h"
#include "OldView.h"

class KSVTopLevel : public KTMainWindow
{
  Q_OBJECT

public:
  KSVTopLevel(const char* name = 0);
  virtual ~KSVTopLevel();

protected:
  KIconLoader* _kil;
  KSVConfig* conf;
  KMenuBar* _menu;
  KToolBar* _tools;
  KStatusBar* _status;
  KProgress* prog;
  KSVGui* _view;
  KSVPrefDlg* _prefs;

public slots:

signals:

protected:
  virtual bool queryExit();
  virtual void closeEvent( QCloseEvent* e );
  void initMenu();
  void initToolBar();
  void initStatusBar();
  bool _changed;

private:
  QPopupMenu* _FileMenu;
  QPopupMenu* _HelpMenu;
  QPopupMenu* _OptionsMenu;
  QPopupMenu* _EditMenu;
  
  QPopupMenu* _normalPM;
  QPopupMenu* _backPM;
  QPopupMenu* _scriptPM;
  
  bool clip_filled;
  
protected slots:
  void slotCloseMain();
  void invokeHelpContents();
  void invokeHelpOnContext();
  void slotShowAbout();
  void slotClearChanges();
  void slotAcceptChanges();
  void slotShowConfig();
  void slotChanged();
  void slotReadConfig();
  void slotAboutQt();
  void toggleLog();
  void toggleTools();
  void toggleStatus();
  void saveOptions();
  void slotUpdateRunning( QString _text );
  void slotClearRunning();
  void editCut();
  void editCopy();
  void editPaste();
  void editUndo();
  void print();
  void printLog();
  void setChanged( bool val = TRUE );
  void setCopy( bool val = FALSE );
  void setCut( bool val = FALSE );
  void setPaste( bool val = FALSE );
  void setUndo( bool val = FALSE );
  void properties();
  void scriptProperties();
  void setLog( bool val );
  void setStatus( bool val );
  void setTools( bool val );
  void initProgress( int max, QString label );
  void endProgress();
  void catchCannotGenerateNumber();

  /**
   * Redraw entries if changed with properties dialog
   */
  void redrawEntry( const KSVDragData* d );

  /**
   * dispatch signals for enabling cut & copy
   */
  void dispatchCutCopy( const KSVDragData* d );

  /**
   * same as above, but for available services
   */
  void dispatchCopyScripts( const KSVDragData* d );
  
  /**
   * catches signal from clipboard
   */
  void dispatchPaste( const KSVDragData* d );
};

#endif
