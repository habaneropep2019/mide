/*
 *   khexdit - a little hex editor
 *   Copyright (C) 1996,97,98  Stephan Kulow
 *
 */

#ifndef _HEXWIDGET_H
#define _HEXWIDGET_H

#include <qmenubar.h>
#include <qlist.h>
#include <qpopmenu.h>

#include <kfm.h>
#include <kapp.h>
#include <kurl.h>
#include <klocale.h>

class HexFile;

class HexWidget : public KTopLevelWidget {
    Q_OBJECT
    
public:

    enum KIND_OF_OPEN {
	READONLY, READWRITE
    };
    enum action { GET, PUT };
    
    HexWidget();
    HexWidget(const char*);
    ~HexWidget();
    void open(const char*, KIND_OF_OPEN kind);
    void open(const char *fileName, const char *url, KIND_OF_OPEN);
    void openURL(const char *fileName, KIND_OF_OPEN kind);
    
protected slots:
    void menuCallback(int);
    void slotDropEvent( KDNDDropZone * _dropZone );
    virtual void saveProperties(KConfig*);
    virtual void readProperties(KConfig*);
    void unsaved(bool flag);
    /// Gets signals from KFM
    void slotKFMFinished();

private:
    QList<HexFile> files;
    HexFile *CurrentFile;
    int winID;
    QString netFile;
    KToolBar *toolbar; 
    KMenuBar *menu;
    KDNDDropZone * dropZone;
    /**
       Only one KFM connection should be opened at once. Otherwise kedit could get
       confused. If this is 0L, you may create a new connection to kfm.
    */
    KFM * kfm;
    
    /**
       If KEdit is waiting for some internet task to finish, this is the
       file that is involved. Mention that it is a complete URL like
       "file:/tmp/mist.txt".
    */
    QString tmpFile;
    /**
       If this is for example GET, then KFM loads a file from the net
       to the local file system.
    */
    action kfmAction; 
    
protected:
    static QList<HexWidget> windowList;
    int initMenu();
    //void paintEvent(QPaintEvent *pa);
    void closeEvent ( QCloseEvent *e);
    int scrollVWidth,scrollHHeight;
    void initGeometry();
};

#endif



