//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This file os part of KRN, a newsreader for the KDE project.              //
// KRN is distributed under the GNU General Public License.                 //
// Read the acompanying file COPYING for more info.                         //
//                                                                          //
// KRN wouldn't be possible without these libraries, whose authors have     //
// made free to use on non-commercial software:                             //
//                                                                          //
// MIME++ by Doug Sauder                                                    //
// Qt     by Troll Tech                                                     //
//                                                                          //
// This file is copyright 1997 by                                           //
// Roberto Alsina <ralsina@unl.edu.ar>                                      //
// Magnus Reftel  <d96reftl@dtek.chalmers.se>                               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#ifndef Artdlg_included
#define Artdlg_included

#include "NNTP.h"

#include <ktopwidget.h>
#include <qdir.h>
#include <qlistbox.h>
#include <qstrlist.h>
#include <qaccel.h>
#include <qevent.h>
#include <qlayout.h>

#include <kpanner.h>
#include "ktablistbox.h"
#include "kiconloader.h"

class KMReaderWin;
class KNumericSpinBox;

class Artdlg : public KTopLevelWidget
{
    Q_OBJECT
        
public:
    
    Artdlg (NewsGroup *_group, NNTP* server);
    virtual ~Artdlg();
    
    KMReaderWin *messwin;
    bool unread;
protected:
    virtual void closeEvent (QCloseEvent *e);

public slots:
    void updateCounter(const char *);
    void openURL(const char *);
    void init (NewsGroup *_group, NNTP* _server);
    void sortHeaders(int column);
private slots:
    bool defaultActions(int);
    bool actions(int,int index=-1);
    bool taggedActions(int);
    bool allActions(int);
    bool readActions(int);
    bool unreadActions(int);
    void fillTree();
    void getSubjects();
    void loadArt (int index,int column);
    bool loadArt (QString id);
    void markArt (int index,int column);
    void markReadArt (int index,int column);
    void decArt (int index,int column);
    void saveArt (QString id);
    void multiSaveArt (QString id);
    void copyText(bool b);
    void popupMenu(int index,int column);
    void FindThis (const char *,const char *,bool casesen,bool wildmode);
    void goTo (int row);
    void updateScores ();
    void setTarget(int);
        
signals:
    void needConnection ();
    void spawnGroup (QString name);
    
private:
    NNTP *server;
    KTabListBox *list;
    char *groupname;
    QAccel *acc;
    NewsGroup *group;
    KMenuBar *menu;
    QPopupMenu *article;
    QPopupMenu *taggedArticle;
    QPopupMenu *options;
    KPanner *panner;
    QVBoxLayout *gl;
    KStatusBar *status;
    KNumericSpinBox *minScore;
    bool showlocked;
    bool showcached;
    QStrList IDList;
    QList <int> depths;
    bool threaded;
    int key1;
    int key2;
    int key3;
    int key4;
};

#endif // Artdlg_included
