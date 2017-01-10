#include "channelview.h"
#include "channelcfgdlg.h"
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qradiobt.h>
#include <kapp.h>
#include <kmsgbox.h>
#include "version.h"

#ifndef MAX
#define MAX(a,b) ((a<b)?(b):(a))
#endif

ChannelViewConfigDialog::ChannelViewConfigDialog(QWidget *parent,const char *name) : QDialog(parent,name,TRUE)
{
    setCaption(i18n("Configure Channel View"));
    setMinimumSize(300,240);
    setMaximumSize(300,240);
    ok=new QPushButton(i18n("OK"),this);
    ok->setGeometry(80,200,100,30);
    connect(ok,SIGNAL(clicked()),SLOT(accept()) );
    cancel=new QPushButton(i18n("Cancel"),this);
    cancel->setGeometry(190,200,100,30);
    connect(cancel,SIGNAL(clicked()),SLOT(reject()) );
    
    qbg = new QButtonGroup(i18n("Choose look mode"),this,"qbg");
    rb0=new QRadioButton(i18n("3D look"),this,"3d");
    rb1=new QRadioButton(i18n("3D - filled"),this,"4d");

    rb0->adjustSize();
    rb1->adjustSize();

    rb0->move(20,30);
    rb1->move(20,70);
    
    qbg->setExclusive(TRUE);

    qbg->setGeometry(5,5,(MAX(rb0->width(),rb1->width()))+20,100);
    qbg->insert(rb0);
    qbg->insert(rb1);

    ((ChannelView::lookMode()==0)?rb0:rb1)->setChecked(TRUE);
    
    connect (qbg, SIGNAL(pressed(int)),this,SLOT(modeselected(int)));

};

void ChannelViewConfigDialog::modeselected(int idx)
{
selectedmode=idx;
};

int ChannelViewConfigDialog::selectedmode;
