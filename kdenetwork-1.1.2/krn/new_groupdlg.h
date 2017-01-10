#ifndef GROUPDLG
#define GROUPDLG

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
// This file is copyright 1998 by                                           //
// Roberto Alsina <ralsina@unl.edu.ar>                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include <ktmainwindow.h>
#include <qlistview.h>

class Groupdlg : public KTMainWindow
{
    Q_OBJECT
public:
    Groupdlg (const char* name = NULL);
    virtual ~Groupdlg();
    void readFolder(const char *name);
private:
    QListView *tree;
};

class GroupFolder: public QListViewItem
{
public:
    GroupFolder( QListView * parent,const char *fname );
    void setOpen( bool o);
    void setup();
    QString filename;
};

class GroupHierarchy: public QListViewItem
{
public:
    GroupHierarchy( GroupFolder * parent,const char *fname );
    GroupHierarchy( GroupHierarchy * parent,const char *fname );
    void setOpen( bool o);
    void setup();
    QString filename;
    QString name;
    QString fullname;
};

#endif
