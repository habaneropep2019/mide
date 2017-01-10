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
#ifndef Groupdlg_included
#define Groupdlg_included

#include <qwidget.h>
#include <qlistbox.h>
#include <qlist.h>
#include <qpopmenu.h>
#include <qtimer.h>

#include <ktopwidget.h>
#include "ktreeview.h"
#include "mytree.h"

#include "NNTP.h"

class Groupdlg : public KTopLevelWidget
{
    Q_OBJECT

public:

    Groupdlg
    (
	const char* name = NULL
    );

    virtual ~Groupdlg();
    void openGroup(KTreeViewItem *);

public slots:
    void openGroup(QString name);
    void openGroup(int);
    void openGroup(KTreeViewItem *, bool&);
    void online();
    void offline();
    bool needsConnect();
    void lostConnection();
    void updateCounter(const char *);
    
private slots:
    bool actions(int,NewsGroup *group=0);
    bool currentActions(int);
    bool subscrActions(int);
    bool taggedActions(int);
    bool listActions(int,QStrList);
    void fillTree();
    void tag(NewsGroup *group);
    void subscribe(NewsGroup *group);
    void checkUnread(NewsGroup *group);
    void getSubjects(NewsGroup *group);
    void getArticles(NewsGroup *group);
    void findGroup ();
    bool postQueued();
private:
    NNTP *server;
    MyTreeList *list;
    bool loadSubscribed();
    bool saveSubscribed();
    bool loadActive();
    bool activeloaded;
    QTimer *counterTimer;
};
#endif // Groupdlg_included
