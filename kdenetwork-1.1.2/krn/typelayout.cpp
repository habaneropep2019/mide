#include "typelayout.h"

#include <qgrpbox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qchkbox.h>
#include <qbttngrp.h>
#include <qradiobt.h>
#include <qpixmap.h>
#include <qlistbox.h>
#include <qcombo.h>
#include <keditcl.h>
#include <kcolorbtn.h>

#include <ktabctl.h>

QDict <TLObj> objDict;
QDict <QButtonGroup> bgroupDict;

KTypeLayout::KTypeLayout(TLForm *_parent)
{
    objDict.insert(_parent->ID,parent);
    outBorder=3;
    inBorder=4;
    parent=_parent;
    widget=parent->widget;
    path=parent->ID+".";
    windowStack.push(parent);
    lastobj=parent;
    rows.append(new QList <TLObj>);
}

KTypeLayout::~KTypeLayout()
{
}

void KTypeLayout::activate()
{
    while (!windowStack.isEmpty())
    {
        endGroup();
    }
}

void KTypeLayout::endGroup()
{
    TLForm *f=windowStack.pop();
    f->layout->end();
}

void KTypeLayout::end()
{
    QList <TLObj> *row;
    TLObj *o;
    
    uint y=0,x=0;
    uint maxH=0,acH=10;

    
    uint maxw=0;
    
    //First lets see if wee need aditional rows
    int slacky=0;
    for (row=rows.first(); row!=0; row=rows.next(),y++)
    {
        for ( o=row->first(); o!=0;o=row->next())
        {
            if ( (y+o->height) > (slacky+rows.count()) )
            {
                slacky=(y+o->height)-rows.count();
            }
        }
    }
    
    //Now lets look for aditional columns
    uint lens[rows.count()+slacky];

    for (uint il=0;il<rows.count()+slacky;il++)
    {
        lens[il]=0;
    }
    y=0;
    for (row=rows.first(); row!=0; row=rows.next(),y++)
    {
        for ( o=row->first(); o!=0;o=row->next())
        {
            for (int ih=0;ih<o->height;ih++)
                lens[y+ih]+=o->width;
        }
    } 
    for (uint il=0;il<rows.count()+slacky;il++)
    {
        if (lens[il]>maxw)
            maxw=lens[il];
    }

    for (QList <TLObj> *l=rows.first();l!=0;l=rows.next())
        if (maxw < l->count())
            maxw=l->count();
    for (uint il=0;il<rows.count()+slacky;il++)
    {
        if (lens[il]>maxw)
            maxw=lens[il];
    }
    
    grid=new QGridLayout (widget,rows.count()+slacky,
                          maxw,outBorder,inBorder);
    
    bool occupied[maxw][rows.count()+slacky];
    for (x=0;x<maxw;x++)
        for (y=0;y<rows.count()+slacky;y++)
            occupied[x][y]=false;
    
    y=0;x=0;
    for (row=rows.first(); row!=0; row=rows.next(),y++)
    {
        x=0;
        for ( o=row->first(); o!=0;o=row->next(),x++)
        {
            while (occupied[x][y])
                x++;
            if ((o->width==1)&&(o->height==1))
            {
                grid->addWidget(o->widget,y,x,o->align);
                occupied[x][y]=true;
            }
            else
            {
                grid->addMultiCellWidget(o->widget,y,y+o->height-1
                                         ,x,x+o->width-1,o->align);
                for (int ix=0;ix<o->width;ix++)
                    for (int iy=0;iy<o->height;iy++)
                    {
                        occupied[x+ix][y+iy]=true;
                    }
            }
            uint tmaxH=o->widget->minimumSize().height();
            if (tmaxH>maxH)
                maxH=tmaxH;
        }
        acH+=maxH+inBorder;
    }
    acH+=parent->vslack;
    widget->setMinimumHeight(acH);
    grid->activate();
    widget->resize(widget->minimumSize());
}


TLObj *KTypeLayout::addWidget(const char *ID,QWidget *w)
{
    TLObj *o;
    o=new TLObj(ID);
    o->widget=w;
    o->path=path;
    windowStack.top()->layout->rows.last()->append(o);
    objDict.insert(ID,o);
    o->align=AlignCenter;
    return o;
}

TLForm *KTypeLayout::addGroup(const char *ID,const char *title, bool frame)
{
    QWidget *g;
    
    if (frame)
        g=new QGroupBox(title,windowStack.top()->widget);
    else
        g=new QWidget(windowStack.top()->widget);

    g->show();
    TLForm *o=new TLForm(ID,"",g);
    if (frame)
    {
        int h=g->fontMetrics().height();
        o->vslack=h;
        o->layout->outBorder=h;
    }
    o->path=path;

    windowStack.top()->layout->rows.last()->append(o);
    path=path+ID+".";
    windowStack.push(o);
    objDict.insert(ID,o);
    return o;
}

TLObj *KTypeLayout::addLabel(const char *ID,const char *text)
{
    QWidget *g;
    QLabel *l=new QLabel(text,windowStack.top()->widget);
    g=l;

    g->show();
    g->setMinimumWidth(l->fontMetrics().width(text)+6);
    g->setMinimumHeight(l->fontMetrics().height());
    return addWidget(ID,g);
}

TLObj *KTypeLayout::addButton(const char *ID,const char *text)
{
    QWidget *g;
    QPushButton *l=new QPushButton(text,windowStack.top()->widget);
    g=l;

    g->show();

    int w=l->fontMetrics().width(text)+8;
    if (w<75) w=75;
    g->setFixedWidth(w);

    int h=l->fontMetrics().height()+8;
    if (h<25) h=25;
    g->setFixedHeight(h);
    
    TLObj *o=addWidget(ID,g);
    o->align=AlignCenter;
    return o;
}

TLObj *KTypeLayout::addColorButton(const char *ID,const QColor *c)
{

    KColorButton *l;
    QWidget *g;
    l=new KColorButton(windowStack.top()->widget);
    g=l;

    g->show();

    g->setMinimumHeight(30);
    
    TLObj *o=addWidget(ID,g);
    o->align=AlignCenter;
    return o;
}

TLObj *KTypeLayout::addButton(const char *ID,const QPixmap &p)
{
    QWidget *g;
    QPushButton *l=new QPushButton("",windowStack.top()->widget);
    l->setPixmap(p);
    g=l;

    g->show();

    int w=p.width()+6;
    g->setFixedWidth(w);
    int h=p.height()+6;
    g->setFixedHeight(h);
    
    TLObj *o=addWidget(ID,g);
    o->align=AlignCenter;
    return o;
}

TLObj *KTypeLayout::addCheckBox(const char *ID,const char *text,bool value)
{
    QWidget *g;
    QCheckBox *l=new QCheckBox(text,windowStack.top()->widget);
    l->setChecked(value);
    g=l;

    g->show();
    g->setMinimumWidth(l->fontMetrics().width(text)+20);
    g->setMinimumHeight(l->fontMetrics().height()>?12);
    TLObj *o=addWidget(ID,g);
    o->align=AlignLeft|AlignVCenter;
    return o;
}

TLObj *KTypeLayout::addRadioButton(const char *ID,const char *GroupID,
                                   const char *text,bool value,int id=-1)
{
    QWidget *g;
    QRadioButton *l=new QRadioButton(text,windowStack.top()->widget);

    QButtonGroup *bg=bgroupDict.find(GroupID);
    if (!bg)
    {
        bg=new QButtonGroup(windowStack.top()->widget,GroupID);
        bg->hide();
        bgroupDict.insert(GroupID,bg);
    }
    bg->insert(l,id);
    l->setChecked(value);
    g=l;

    g->show();
    g->setMinimumWidth(l->fontMetrics().width(text)+20);
    g->setMinimumHeight(l->fontMetrics().height() >? 12);
    TLObj *o=addWidget(ID,g);
    o->align=AlignLeft|AlignVCenter;
    return o;

}

TLBook *KTypeLayout::addBook(const char *ID)
{
    QWidget *g;
    
    g=new KTabCtl(windowStack.top()->widget,ID);

    g->show();
    TLBook *o=new TLBook(ID,g);
    o->vslack=g->fontMetrics().height()+8;
    o->path=path;

    windowStack.top()->layout->rows.last()->append(o);
    path=path+ID+".";
    windowStack.push(o);
    objDict.insert(ID,o);
    o->align=AlignCenter;
    return o;
}

TLForm *KTypeLayout::addPage(const char *ID,const char *title)
{
    QWidget *g;
    
    g=new QFrame(windowStack.top()->widget);
    ((KTabCtl *)windowStack.top()->widget)->addTab(g,title);

    g->show();

    TLForm *o=new TLForm(ID,"",g);
    o->path=path;
    windowStack.top()->layout->rows.last()->append(o);
    path=path+ID+".";
    windowStack.push(o);
    objDict.insert(ID,o);
    return o;
}

TLObj *KTypeLayout::addLineEdit(const char *ID,const char *text,int maxlen)
{
    QWidget *g;
    QLineEdit *l=new QLineEdit(windowStack.top()->widget);
    g=l;

    if (maxlen>=0)
        l->setMaxLength(maxlen);
    l->setText(text);

    g->show();
    g->setFixedHeight(l->fontMetrics().height()+10);
    g->setMinimumWidth(l->fontMetrics().width(text)+10);
    if (maxlen>0)
        g->setMaximumWidth(10+maxlen*l->fontMetrics().maxWidth());
    return addWidget(ID,g);
}

TLObj *KTypeLayout::addIntLineEdit(const char *ID,const char *text,int maxlen)
{
    QWidget *g;
    KIntLineEdit *l=new KIntLineEdit(windowStack.top()->widget);
    g=l;

    if (maxlen>=0)
        l->setMaxLength(maxlen);
    l->setText(text);

    g->show();
    g->setFixedHeight(l->fontMetrics().height()+10);
    g->setMinimumWidth(l->fontMetrics().width(text)+10);
    if (maxlen>0)
        g->setMaximumWidth(10+maxlen*l->fontMetrics().width("8"));
    return addWidget(ID,g);
}

TLObj *KTypeLayout::addListBox (const char *ID,QStrList *contents,int minRows)
{
    QWidget *g;
    QListBox *l=new QListBox(windowStack.top()->widget);
    g=l;

    if (contents)
        l->insertStrList(contents);
    g->show();
    g->setMinimumHeight((l->fontMetrics().height()+1)*minRows+6);
    return addWidget(ID,g);
}

TLObj *KTypeLayout::addComboBox (const char *ID,QStrList *contents,bool rw)
{
    QWidget *g;
    QComboBox *l=new QComboBox(rw,windowStack.top()->widget);
    g=l;

    int w=0;
    if (contents)
    {
        l->insertStrList(contents);
        for (char *item=contents->first();item!=0;item=contents->next())
        {
            int w2=l->fontMetrics().width(item);
            if (w<w2)
                w=w2;
        }
    }
    w=w+32;
    if (w<75)
        w=75;
    g->show();
    g->setFixedHeight(l->fontMetrics().height()+10);
    g->setFixedWidth(w);
    return addWidget(ID,g);
}

void KTypeLayout::newLine()
{
    KTypeLayout *l=windowStack.top()->layout;
    l->rows.append(new QList <TLObj>);
}

void KTypeLayout::skip()
{
    addLabel("__++__skipspace__++__","");
    //Skips shouldn't affect stretching, so ...
    setAlign("__++__skipspace__++__",0);
}

void KTypeLayout::setAlign (char *ID,int align)
{
    TLObj *o=findObject(ID);
    if (!o)
    {
        warning ("Aligning unexisting object %s",ID);
        return;
    }

    QWidget *w=o->widget;
    
    if ((align & AlignTop) && (align & AlignBottom))
    {
        align=align-AlignTop-AlignBottom;
        align=align|AlignVCenter;
        w->setMaximumHeight(10000);
    }
    if ((align & AlignLeft) && (align & AlignRight))
    {
        align=align-AlignLeft-AlignRight;
        align=align|AlignHCenter;
        w->setMaximumWidth(10000);
    }

    o->align=align;
}


void KTypeLayout::setSpawn (char *ID,int vSpawn,int hSpawn)
{
    TLObj *o=findObject(ID);
    if (!o)
    {
        warning ("Setting spawn of unexisting object %s",ID);
        return;
    }
    o->width=vSpawn;
    o->height=hSpawn;
}


TLObj *KTypeLayout::findObject(const char *_ID)
{
    return objDict.find(_ID);
}

QWidget *KTypeLayout::findWidget(const char *_ID)
{
    TLObj *o=objDict.find(_ID);
    if (o)
        return o->widget;
    return 0;
}

KBookLayout::KBookLayout(TLForm *_parent)
{
    objDict.insert(_parent->ID,parent);
    outBorder=3;
    inBorder=4;
    parent=_parent;
    widget=parent->widget;
    path=parent->ID+".";
    windowStack.push(parent);
    lastobj=parent;
    rows.append(new QList <TLObj>);
}
KBookLayout::~KBookLayout()
{
}

void KBookLayout::end()
{
    int minx=0;
    int miny=0;

    QList <TLObj> *row;
    TLObj *o;

    //find the biggest page
    for (row=rows.first(); row!=0; row=rows.next())
    {
        for ( o=row->first(); o!=0;o=row->next())
        {
            if (minx < o->widget->minimumSize().width())
                minx=o->widget->minimumSize().width();
            if (miny < o->widget->minimumSize().height())
                miny=o->widget->minimumSize().height();
        }
    }
    //now make all pages the same size
    for (row=rows.first(); row!=0; row=rows.next())
    {
        for ( o=row->first(); o!=0;o=row->next())
        {
            o->widget->resize(minx,miny);
        }
    }
    
    miny+=widget->fontMetrics().height()+16;
    minx+=8;
    widget->setMinimumWidth(minx);
    widget->setMinimumHeight(miny);
    widget->resize(minx,miny);
}
