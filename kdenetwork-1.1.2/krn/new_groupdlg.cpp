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

#include "new_groupdlg.h"

#include <qlistview.h>
#include <qdict.h>
#include <qdir.h>
#include <qfileinfo.h>

#include <kapp.h>
#include <kiconloader.h>
#include <kfileio.h>

#include "new_globals.h"


#include "new_groupdlg.moc"

QDict <GroupFolder> folders;

Groupdlg::Groupdlg ( const char* name)
    :KTMainWindow(name)
{
    tree=new QListView (this);
    tree->addColumn("Nombre",-1);
    tree->setRootIsDecorated(TRUE);

    readFolder(KApplication::localkdedir()+"/share/apps/krn/folders");
/*
    addFolder("Subscribed Newsgroups");
    addFolder("Servers");
    addFolder("All Newsgroups");
*/
    setView(tree);
    conf->setGroup("Geometry");
    setGeometry(conf->readNumEntry("GroupX",100),
                conf->readNumEntry("GroupY",40),
                conf->readNumEntry("GroupW",400),
                conf->readNumEntry("GroupH",400));
}

Groupdlg::~Groupdlg()
{
    //Let's make it remember all the right stuff
    conf->setGroup("Geometry");
    conf->writeEntry("GroupX",x());
    conf->writeEntry("GroupY",y());
    conf->writeEntry("GroupW",width());
    conf->writeEntry("GroupH",height());
    conf->sync();
}

void Groupdlg::readFolder(const char *name)
{

    QDir d(name);
    d.setFilter(QDir::Files);
    QStrList *files=new QStrList (*d.entryList("*"));
    GroupFolder *item;
    
    for (char *fname=files->first();fname!=0;fname=files->next())
    {
        item=new GroupFolder(tree,fname);
        folders.insert(name,item);
    }
}

GroupFolder::GroupFolder( QListView * parent, const char *fname )
:QListViewItem(parent)
{
    setText(0,fname);
    setPixmap (0,Icon("krnfolder.xpm"));
    setOpen(true);
    filename=KApplication::localkdedir()+"/share/apps/krn/folders/";
    filename+=fname;
}
void GroupFolder::setOpen (bool o)
{
    QString base="";
    if ( o && !childCount())
    {
        // preload pixmaps
        QPixmap followup = Icon("followup.xpm");
        char tc;
        
        //Load the stuff and split it in lines
        QString data=kFileToString(filename);
        QStrList groups;
        char *tok=strtok (data.data(),"\n");
        while (tok)
        {
            groups.append(tok);
            tok=strtok(NULL,"\n");
        }
        
        //First make a list of all matches with
        //the exact same beginning, and either the same or one extra
        //dots. In two sorted lists.
        
        // we must insert non-NULL pointer into the dict,
        // so we take a dummy
        int nullItem = 0;
        
        QStrList items;
        items.setAutoDelete(true);
        QDict<int> dict(2423);
        char *iter;
        QStrListIterator iterat(groups);
        for (;iterat.current(); ++iterat)
        {
            iter=iterat.current();
            //this group's name matches the base
            int c1=QString(iter).contains('.');
            if (c1==0) //Is there a group with no dots in it somewhere?
                items.inSort(iter);
            else
            {
                char *nextdot=strchr(iter+1,'.')+1;
                tc=nextdot[0];
                nextdot[0]=0;
                if (!dict.find(iter))
                {
                    // It's new, so add it to the base list
                    // and add it to the branch list
                    dict.insert(iter, &nullItem);
                    items.inSort(iter);
                }
                nextdot[0]=tc;
            }
        }
        //Now we add all the items to the tree
        
        for (char *name=items.first();name!=0;name=items.next())
        {
            QListViewItem *itemp;
            if (name[strlen(name)-1]=='.')
            {
                itemp=new GroupHierarchy(this,name);
            }
            else
            {
                itemp=new QListViewItem(this,name);
                itemp->setPixmap(0,followup);
            }
        }
    }
    QListViewItem::setOpen(o);
}

void GroupFolder::setup()
{
    setExpandable (TRUE);
    QListViewItem::setup();
}

GroupHierarchy::GroupHierarchy( GroupFolder * parent, const char *fname )
:QListViewItem(parent)
{
    setText(0,fname);
    setPixmap (0,Icon("krnfolder.xpm"));
    setOpen(true);
    filename=parent->filename;
    name=fname;
    fullname=fname;
}

GroupHierarchy::GroupHierarchy( GroupHierarchy * parent, const char *fname )
:QListViewItem(parent)
{
    setText(0,fname);
    setPixmap (0,Icon("krnfolder.xpm"));
    setOpen(true);
    filename=parent->filename;
    name=fname;
    fullname=parent->fullname+name;
}

void GroupHierarchy::setup()
{
    setExpandable (TRUE);
    QListViewItem::setup();
}

void GroupHierarchy::setOpen(bool o)
{
    QString base="";
    if ( o && !childCount())
    {
        // preload pixmaps
        QPixmap followup = Icon("followup.xpm");
        char tc;
        int l=fullname.length();
        
        //Load the stuff and split it in lines
        QString data=kFileToString(filename);
        QStrList groups;
        char *tok=strtok (data.data(),"\n");
        while (tok)
        {
            if (!strncmp(tok,fullname.data(),fullname.length()))
                groups.append(tok);
            tok=strtok(NULL,"\n");
        }
        
        //First make a list of all matches with
        //the exact same beginning, and either the same or one extra
        //dots. In two sorted lists.
        
        // we must insert non-NULL pointer into the dict,
        // so we take a dummy
        int nullItem = 0;
        
        QStrList items;
        items.setAutoDelete(true);
        QDict<int> dict(2423);
        char *iter;
        QStrListIterator iterat(groups);
        for (;iterat.current(); ++iterat)
        {
            int c=QString(fullname).contains('.');
            iter=iterat.current()+l;
            //this group's name matches the base
            int c1=QString(iter).contains('.');
            if (c1==0)
                items.inSort(iter);
            else
            {
                char *nextdot=strchr(iter+1,'.')+1;
                tc=nextdot[0];
                nextdot[0]=0;
                if (!dict.find(iter))
                {
                    // It's new, so add it to the base list
                    // and add it to the branch list
                    dict.insert(iter, &nullItem);
                    items.inSort(iter);
                }
                nextdot[0]=tc;
            }
        }
        //Now we add all the items to the tree
        
        for (char *name=items.first();name!=0;name=items.next())
        {
            QListViewItem *itemp;
            if (name[strlen(name)-1]=='.')
            {
                itemp=new GroupHierarchy(this,name);
            }
            else
            {
                itemp=new QListViewItem(this,name);
                itemp->setPixmap(0,followup);
            }
        }
    }
    QListViewItem::setOpen(o);
}
