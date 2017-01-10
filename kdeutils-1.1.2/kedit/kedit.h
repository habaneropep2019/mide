/*

    $Id: kedit.h,v 1.17 1998/11/06 00:38:12 dsweet Exp $

    Copyright (C) 1997 Bernd Johannes Wuebben   
                       wuebben@math.cornell.edu
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#ifndef _KEDIT_H_
#define _KEDIT_H_

#include <qlist.h>
#include <qstring.h>

#include <qpopmenu.h>
#include <qmenubar.h>
#include <qapp.h>
#include <qkeycode.h>
#include <qaccel.h>
#include <qobject.h>
#include <qmlined.h>
#include <qradiobt.h>
#include <qfiledlg.h>
#include <qchkbox.h>
#include <qmsgbox.h>
#include <qgrpbox.h>
#include <qstrlist.h>
#include <qtooltip.h>
#include <qregexp.h> 
#include <qprinter.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include <kfm.h>
#include <kapp.h>
#include <kurl.h>
#include <kconfig.h>
#include <ktoolbar.h>
#include <kspell.h>
#include <ksconfig.h>


#include "kstatusbar.h"
#include "ktopwidget.h"
#include "kbutton.h"

#include "keditcl.h"
#include "print.h"
#include "filldlg.h"
#include "version.h"

// StatusBar field IDs
#define ID_LINE_COLUMN 1
#define ID_INS_OVR 2
#define ID_GENERAL 3


class TopLevel : public KTopLevelWidget
{
    Q_OBJECT

public:

    /// Tells us what kind of job kedit is waiting for.
    enum action { GET, PUT };
    
    TopLevel( QWidget *parent=0, const char *name=0 );
    ~TopLevel();
    KEdit *eframe;

    /// Works like the load method of the edit widget but is able to talk to KFM
    void openNetFile( const char *_url, int _mode );

    /// Saves the current text to the URL '_url'.
    void saveNetFile( const char *_url );

    /// List of all windows
    static QList<TopLevel> windowList;
    QPopupMenu *file, *edit, *help,*options,*right_mouse_button, *colors, *recentpopup;
    
    void closeEvent( QCloseEvent *e );

protected:

//    void resizeEvent(QResizeEvent *);
    void setSensitivity();

    void readSettings();
    void writeSettings();
    void setupMenuBar();
    void setupToolBar();
    void setupEditWidget();
    void setupStatusBar();        

private:    

    int statusID, toolID, indentID;
    KMenuBar *menubar;
    KToolBar *toolbar;
    QColor forecolor;
    QColor backcolor;
    QString url_location;
    KStatusBar *statusbar;
    QTimer *statusbar_timer;
    QStrList recent_files;

    int open_mode;
    int editor_width;
    int editor_height;
    int hide_toolbar;
    int hide_statusbar;
    // toolbar ID holder:
    int toolbar1;
    struct printinfo pi;
    bool fill_column_is_set;
    bool word_wrap_is_set;
    bool backup_copies_is_set;
    int fill_column_value;
    QString mailcmd;

    QFont generalFont;
    KConfig *config;

    /// KFM client
    /**
      Only one KFM connection should be opened at once. Otherwise kedit could get
      confused. If this is 0L, you may create a new connection to kfm.
      */
    KFM * kfm;
    
    /// Temporary file for internet purposes
    /**
      If KEdit is waiting for some internet task to finish, this is the
      file that is involved. Mention that it is a complete URL like
      "file:/tmp/mist.txt".
      */
    QString tmpFile;

    /// If we load a file from the net this is the corresponding URL
    QString netFile;
    
    /// Tells us what kfm is right now doing for us
    /**
      If this is for example GET, then KFM loads a file from the net
      to the local file system.
      */
    action kfmAction;
    
    /// The open mode for a net file
    /**
      If KEdit is waiting for an internet file, this is the mode in which
      it should open the file.
      */
    int openMode;

    //QString helpurl;

    // Session management 

    void saveProperties(KConfig*);
    void readProperties(KConfig*);
    void add_recent_file(const char*);

    // Spellchecking    
    KSpell *kspell;
    int spell_offset;

    
public slots:

    void openRecent(int);
    void set_colors();
    void gotoLine();
    void fill_column_slot();
    void mail();
    /*    void fancyprint();*/
    void set_foreground_color();
    void set_background_color();
    void saving_slot();
    void loading_slot();
    void setGeneralStatusField(QString string);
    void copy();
    void paste();
    void cut();
    void insertDate();
    void toggle_indent_mode();
    void print();
    void select_all();
    void timer_slot();
    void save_options();
    void file_open();
    void file_new();
    void insertFile();
    void file_open_url();
    void setFileCaption();
    void file_save_url();
    void quiteditor();
    void statusbar_slot();
    void newTopLevel();
    void file_close();
    void file_save();
    void file_save_as();
//    void about();
    void helpselected();
    void search();
    void replace();
    void font();
    void toggleStatusBar();
    void toggleToolBar();
    void search_again();
    void toggle_overwrite();
    /// Gets signals from KFM
    void slotKFMFinished();

    void spellcheck();
    void spell_configure();

    void spell_progress (unsigned int percent);
    void spell_done();

    /// Drag and Drop
    void slotDropEvent( KDNDDropZone * _dropZone );
};

#endif
