#ifndef MY_TREE_H
#define MY_TREE_H

#include "ktreeview.h"

class MyTreeList : public KTreeView
{
public:
    MyTreeList(QWidget *parent = 0,
               const char *name = 0,
               WFlags f = 0) :
    KTreeView(parent , name , f) {};
    void setTopCell(int c) { KTreeView::setTopCell(c); }
    void appendChildItem(KTreeViewItem* parent,
                         KTreeViewItem* child){KTreeView::appendChildItem(parent,child);};

};  

#endif

