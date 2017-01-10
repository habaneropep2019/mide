/**************************************************************************

    kdisptext.cpp  - The widget that displays the karaoke/lyrics text 
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

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

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include "kdisptext.h"
#include <qpainter.h>
#include <qfont.h>
#include <qfontmet.h>
#include <qscrbar.h>
#include <kapp.h>
#include <kconfig.h>
#include "version.h"


#define NUMPREVLINES 2


KDisplayText::KDisplayText(QWidget *parent,const char *name) : QWidget(parent,name)
{
    first_line_[0]=first_line_[1]=NULL;
    linked_list_[0]=linked_list_[1]=NULL;
    cursor_line_[0]=cursor_line_[1]=NULL;
    cursor_[0]=cursor_[1]=NULL;
    nlines_[0]=nlines_[1]=0;
    linked_list=NULL;
    cursor_line=NULL;
    first_line=NULL;
    cursor=NULL;
    textscrollbar=NULL;
    nlines=0;
    setBackgroundColor(QColor (110,110,110));
    KConfig *kcfg=(KApplication::getKApplication())->getConfig();
    kcfg->setGroup("KMid");
    QFont *qtextfontdefault=new QFont("courier",22);
    qtextfont=new QFont(kcfg->readFontEntry("KaraokeFont",qtextfontdefault));
    delete qtextfontdefault;
    qfmetr=new QFontMetrics(*qtextfont);
    font_height=qfmetr->height();
    qpaint=NULL;
    nvisiblelines=height()/qfmetr->lineSpacing();
    textscrollbar=new QScrollBar(1,1,1,2,1,QScrollBar::Vertical,this,"TheKaraokeScrollBar");
    connect(textscrollbar,SIGNAL(valueChanged(int)),this,SLOT(ScrollText(int)));
};

KDisplayText::~KDisplayText()
{
    RemoveLinkedList();
    //if (textscrollbar!=NULL) delete textscrollbar;
};

void KDisplayText::PreDestroyer(void)
{
    delete qfmetr;
    delete qtextfont;
    if (qpaint!=NULL) delete qpaint;
};

void KDisplayText::RemoveLine(kdispt_line *tmpl)
{
    kdispt_ev *tmpe;
    while (tmpl->ev!=NULL)
    {
        tmpe=tmpl->ev;
        tmpl->ev=tmpe->next;
        //    delete tmpe->spev; Remember that the Special Events that this pointer
        //		 pointers to is the same that the Player object has instantiated
        delete tmpe;
    };
};

void KDisplayText::RemoveLinkedList(void)
{
    cursor=NULL;
    cursor_line=NULL;
    first_line=NULL;
    linked_list=NULL;
    nlines=0;
    nlines_[0]=nlines_[1]=0;
    first_line_[0]=first_line_[1]=NULL;
    cursor_line_[0]=cursor_line_[1]=NULL;
    cursor_[0]=cursor_[1]=NULL;
    
    kdispt_line *tmpl;
    for (int i=0;i<2;i++)
    {
        while (linked_list_[i]!=NULL)
        {
            RemoveLine(linked_list_[i]);
            tmpl=linked_list_[i];
            linked_list_[i]=linked_list_[i]->next;
            delete tmpl;
        };
    };
};

void KDisplayText::ClearEv(bool totally)
{
    RemoveLinkedList();
    if (totally)
    {
        textscrollbar->setRange(1,1);
        repaint(TRUE);
    };
};

int KDisplayText::IsLineFeed(char c,int type)
{
    switch (type)
    {
    case (1) : if (/*(c==0)||*/(c=='\\')||(c=='/')||(c=='@')) return 1;break;
    case (5) : if (/*(c==0)||*/(c==10)||(c==13)) return 1;break;
    default :  if ((c==0)||(c==10)||(c==13)||(c=='\\')||(c=='/')||(c=='@')) return 1;break;
    };
    return 0;
};

void KDisplayText::AddEv(SpecialEvent *ev)
{
    if ((ev->type==1) || (ev->type==5))
    {
        int idx=(ev->type==1)? 0 : 1;
        if (linked_list_[idx]==NULL)
        {
            linked_list_[idx]=new kdispt_line;
            linked_list_[idx]->next=NULL;
            linked_list_[idx]->num=1;
            linked_list_[idx]->ypos=30;
            linked_list_[idx]->ev=new kdispt_ev;
            cursor_line_[idx]=linked_list_[idx];
            cursor_[idx]=cursor_line_[idx]->ev;
            cursor_[idx]->spev=ev;
            cursor_[idx]->xpos=5;
            if (IsLineFeed(ev->text[0],ev->type))
                cursor_[idx]->width=qfmetr->width(&ev->text[1]);
            else
                cursor_[idx]->width=qfmetr->width(ev->text);
            cursor_[idx]->next=NULL;    
            first_line_[idx]=linked_list_[idx];
            first_line=first_line_[idx];
            nlines_[idx]=1;
        }
        else
        {
            if (IsLineFeed(ev->text[0],ev->type))
            {
                nlines_[idx]++;
                cursor_line_[idx]->next=new kdispt_line;
                cursor_line_[idx]=cursor_line_[idx]->next;
                cursor_line_[idx]->num=nlines_[idx];
                cursor_line_[idx]->ypos=cursor_line_[idx]->num*30;
                cursor_line_[idx]->ev=new kdispt_ev;
                cursor_line_[idx]->next=NULL;
                cursor_[idx]=cursor_line_[idx]->ev;
                cursor_[idx]->xpos=5;
                cursor_[idx]->width=qfmetr->width(&ev->text[1]);
            }
            else
            {
                cursor_[idx]->next=new kdispt_ev;
                cursor_[idx]->next->xpos=cursor_[idx]->xpos+cursor_[idx]->width;
                cursor_[idx]=cursor_[idx]->next;
                cursor_[idx]->width=qfmetr->width(ev->text);
            };
            cursor_[idx]->spev=ev;
            cursor_[idx]->next=NULL;
        };
    };
};

void KDisplayText::calculatePositions(void)
{
    int typeoftextevents=1;
    int fin=0;
    kdispt_line *tmpl;
    kdispt_ev *tmp;
    int tmpx;
    int tmpy;
    while (!fin)
    {
        tmpl=linked_list_[(typeoftextevents==1)?0:1];
        tmpy=30;
        while (tmpl!=NULL)
        {
            tmp=tmpl->ev;
            tmpx=5;
            while (tmp!=NULL)
            {
                if (tmp->spev->type==typeoftextevents)
                {
                    if (IsLineFeed(tmp->spev->text[0],typeoftextevents))
                    {
                        tmpy+=30;
                        tmpx=5;
                        tmp->xpos=tmpx;
                        if (tmp->spev->text[0]!=0) tmp->width=qfmetr->width(&tmp->spev->text[1]);
                    }
                    else
                    {
                        tmp->xpos=tmpx;
                        tmp->width=qfmetr->width(tmp->spev->text);
                    };
                    tmpx+=tmp->width;
                };
                tmp=tmp->next;
            };
            tmpl->ypos=tmpy;
            tmpl=tmpl->next;
        };
        
        if (typeoftextevents==1) typeoftextevents=5;
        else fin=1;
    };
    
};

void KDisplayText::paintEvent(QPaintEvent *)
{
    if (qpaint==NULL)
    {
        qpaint=new QPainter(this);
        qpaint->setFont(*qtextfont);
    };
    if (linked_list==NULL) {drawFrame();return;};
    if (nlines>nvisiblelines)
        textscrollbar->setRange(1,nlines-nvisiblelines+1);
    else
        textscrollbar->setRange(1,1);
    
    int i=0;
    qpaint->setPen(QColor(255,255,0));
    int colorplayed=1;
    if (cursor==NULL) colorplayed=0; // Thus, the program doesn't change the color
    kdispt_line *tmpl=first_line;
    kdispt_ev *tmp;
    KConfig *kcfg=KApplication::getKApplication()->getConfig();
    kcfg->setGroup("KMid");
    typeoftextevents=kcfg->readNumEntry("TypeOfTextEvents",1);
#ifdef KDISPTEXTDEBUG
    printf("events displayed %d\n",typeoftextevents);
#endif
    while ((i<nvisiblelines)&&(tmpl!=NULL))
    {
        tmpl->ypos=(tmpl->num-first_line->num+1)*qfmetr->lineSpacing();
        tmp=tmpl->ev;
        while ((tmp!=NULL)&&(tmp->spev->type!=typeoftextevents)) tmp=tmp->next;
        while (tmp!=NULL)
        {
            if ( colorplayed && 
                 //	(tmp->spev->absmilliseconds>=cursor->spev->absmilliseconds))
                 (tmp->spev->id>=cursor->spev->id))
            {
                qpaint->setPen(QColor(0,0,0));
                colorplayed=0;
            };	
            
            if (IsLineFeed(tmp->spev->text[0],tmp->spev->type))
                qpaint->drawText(tmp->xpos,tmpl->ypos,&tmp->spev->text[1]);
            else
                qpaint->drawText(tmp->xpos,tmpl->ypos,tmp->spev->text);
            tmp=tmp->next;
            while ((tmp!=NULL)&&(tmp->spev->type!=typeoftextevents)) tmp=tmp->next;
        };
        i++;
        tmpl=tmpl->next;
    }
    
    drawFrame();

    qpaint->setPen(QColor(255,255,0));
    
};

void KDisplayText::drawFrame(void)
{
    int w=width();
    if (textscrollbar!=0L) w=textscrollbar->x();
    QColorGroup qcg=colorGroup();
    qpaint->setPen(qcg.light());
    qpaint->drawLine(0,height()-1,w-1,height()-1);
    qpaint->drawLine(w-1,0,w-1,height()-1);
    qpaint->setPen(qcg.background());
    qpaint->drawLine(1,height()-2,w-2,height()-2);
    qpaint->drawLine(w-2,1,w-2,height()-2);
    qpaint->setPen(qcg.mid());
    qpaint->drawLine(0,0,0,height()-1);
    qpaint->drawLine(0,0,w-1,0);
    qpaint->setPen(qcg.dark());
    qpaint->drawLine(1,1,1,height()-2);
    qpaint->drawLine(1,1,w-2,1);
//    qpaint->setPen(QColor(0,0,0));
//    qpaint->drawRect(2,2,w-4,height()-4);

};

void KDisplayText::resizeEvent(QResizeEvent *)
{
    nvisiblelines=height()/qfmetr->lineSpacing();
    
    textscrollbar->setGeometry(width()-16,0,16,height());
};

void KDisplayText::CursorToHome(void)
{
    KConfig *kcfg=KApplication::getKApplication()->getConfig();
    kcfg->setGroup("KMid");
    typeoftextevents=kcfg->readNumEntry("TypeOfTextEvents",1);
    linked_list=linked_list_[(typeoftextevents==1)? 0:1];
    nlines=nlines_[(typeoftextevents==1)? 0:1];
    cursor_line_[0]=linked_list_[0];
    first_line_[0]=cursor_line_[0];
    if (cursor_line_[0]!=NULL) cursor_[0]=cursor_line_[0]->ev;
    cursor_line_[1]=linked_list_[1];
    first_line_[1]=cursor_line_[1];
    if (cursor_line_[1]!=NULL) cursor_[1]=cursor_line_[1]->ev;
    
    if (linked_list==NULL)
    {
        cursor_line=NULL;
        cursor=NULL;
        first_line=NULL;
    }
    else
    {
        cursor_line=linked_list;
        cursor=cursor_line->ev;
        first_line=linked_list;
    };
    
    textscrollbar->setValue(1);
    
}; 

void KDisplayText::PaintIn(int type)
{
    if (type!=typeoftextevents)
    {
        int idx=(type==1)?0:1;
        if (cursor_[idx]==NULL) return;
        cursor_[idx]=cursor_[idx]->next;
        while ((cursor_[idx]==NULL)&&(cursor_line_[idx]!=NULL))
        {
            cursor_line_[idx]=cursor_line_[idx]->next;
            if (cursor_line_[idx]!=NULL)
            {
                cursor_[idx]=cursor_line_[idx]->ev;
                if ((cursor_line_[idx]->num>first_line_[idx]->num+NUMPREVLINES)
                    &&(cursor_line_[idx]->num<first_line_[idx]->num+nvisiblelines+1))
                    if ((first_line_[idx]!=NULL)&&(first_line_[idx]->num+nvisiblelines<=nlines_[idx])) first_line_[idx]=first_line_[idx]->next;
            };
        };
        return;
    };
    
    if ((cursor==NULL)||(cursor_line==NULL))
    {
        printf("KDispT : cursor == NULL !!!\n");
        return;
    };
    
    if ((cursor_line->num>=first_line->num)&&(cursor_line->num<first_line->num+nvisiblelines))
    { 
        if (cursor->spev->type==typeoftextevents)
            if (IsLineFeed(cursor->spev->text[0],cursor->spev->type))
                qpaint->drawText(cursor->xpos,cursor_line->ypos,&cursor->spev->text[1]);
            else
                qpaint->drawText(cursor->xpos,cursor_line->ypos,cursor->spev->text);
    };
    cursor=cursor->next;
    while ((cursor==NULL)&&(cursor_line!=NULL))
    {
        cursor_line=cursor_line->next;
        if (cursor_line!=NULL) 
        {
            cursor=cursor_line->ev;
            if ((cursor_line->num>first_line->num+NUMPREVLINES)
                &&(cursor_line->num<first_line->num+nvisiblelines+1))
                textscrollbar->setValue(textscrollbar->value()+1);
        };
    };
};

void KDisplayText::ScrollText(int i)
{
    kdispt_line *tmpl=linked_list;
    while ((tmpl!=NULL)&&(tmpl->num<i)) tmpl=tmpl->next;
    first_line=tmpl;
    repaint();
};

void KDisplayText::gotomsec(ulong i)
{
    int notidx=(typeoftextevents==1)?1:0;
    
    if (linked_list_[notidx]!=NULL) 
    {
        cursor_line_[notidx]=linked_list_[notidx];
        first_line_[notidx]=cursor_line_[notidx];
        cursor_[notidx]=cursor_line_[notidx]->ev;
        while ((cursor_line_[notidx]!=NULL)&&(cursor_[notidx]->spev->absmilliseconds<i))
        {
            cursor_[notidx]=cursor_[notidx]->next;
            while ((cursor_[notidx]==NULL)&&(cursor_line_[notidx]!=NULL))
            {
                cursor_line_[notidx]=cursor_line_[notidx]->next;
                if (cursor_line_[notidx]!=NULL) 
                {
                    cursor_[notidx]=cursor_line_[notidx]->ev;
                    if ((cursor_line_[notidx]->num>first_line_[notidx]->num+NUMPREVLINES)
                        &&(cursor_line_[notidx]->num<first_line_[notidx]->num+nvisiblelines+1))
                        if ((first_line_[notidx]!=NULL)&&(first_line_[notidx]->num+nvisiblelines<=nlines_[notidx])) first_line_[notidx]=first_line_[notidx]->next;
                };
            };
        };
    };
    
    if (linked_list!=NULL) 
    {
        cursor_line=linked_list;
        cursor=cursor_line->ev;
        first_line=linked_list;
        while ((cursor_line!=NULL)&&(cursor->spev->absmilliseconds<i))
        {
            cursor=cursor->next;
            while ((cursor==NULL)&&(cursor_line!=NULL))
            {
                cursor_line=cursor_line->next;
                if (cursor_line!=NULL) 
                {
                    cursor=cursor_line->ev;
                    if ((cursor_line->num>first_line->num+NUMPREVLINES)
                        &&(cursor_line->num<first_line->num+nvisiblelines+1))
                        if ((first_line!=NULL)&&(first_line->num+nvisiblelines<=nlines)) first_line=first_line->next;
                };
            };
            
            
        }
        
        textscrollbar->setValue(first_line->num);
        repaint();
    };
};

QFont *KDisplayText::getFont(void)
{
    return qtextfont;
};

void KDisplayText::fontChanged(void)
{
    KConfig *kcfg=(KApplication::getKApplication())->getConfig();
    kcfg->setGroup("KMid");
    QFont *qtextfontdefault=new QFont(*qtextfont);
    delete qtextfont;
    qtextfont=new QFont(kcfg->readFontEntry("KaraokeFont",qtextfontdefault));
    delete qtextfontdefault;
    qfmetr=new QFontMetrics(*qtextfont);
    font_height=qfmetr->height();
    if (qpaint!=NULL) qpaint->setFont(*qtextfont);
    calculatePositions();
    nvisiblelines=height()/qfmetr->lineSpacing();
    repaint(TRUE);
};

void KDisplayText::ChangeTypeOfTextEvents(int type)
{
    int idxold=(typeoftextevents==1)?0:1;
    int idxnew=(type==1)?0:1;
    cursor_line_[idxold]=cursor_line;
    first_line_[idxold]=first_line;
    cursor_[idxold]=cursor;
    linked_list=linked_list_[idxnew];
    cursor_line=cursor_line_[idxnew];
    first_line=first_line_[idxnew];
    cursor=cursor_[idxnew];
    nlines=nlines_[idxnew];
    typeoftextevents=type;
    if (first_line!=NULL) 
    {
        if (nlines>nvisiblelines)
            textscrollbar->setRange(1,nlines-nvisiblelines+1);
        else
            textscrollbar->setRange(1,1);
        textscrollbar->setValue(first_line->num);
    };
};

int KDisplayText::ChooseTypeOfTextEvents(void)
{
    return (nlines_[0]>nlines_[1])? 1 : 5;
};

void KDisplayText::ScrollDown()
{
    textscrollbar->setValue(textscrollbar->value()+1);
};

void KDisplayText::ScrollUp()
{
    textscrollbar->setValue(textscrollbar->value()-1);
};

void KDisplayText::ScrollPageDown()
{
    textscrollbar->setValue(textscrollbar->value()+nvisiblelines);
};

void KDisplayText::ScrollPageUp()
{
    textscrollbar->setValue(textscrollbar->value()-nvisiblelines);
};

void KDisplayText::saveLyrics(FILE *fh)
{
    kdispt_line *Lptr=linked_list_[(typeoftextevents==1)? 0:1];
    while (Lptr!=NULL)
    {
        kdispt_ev *Cptr=Lptr->ev;
        if (Cptr!=NULL)
        {
            if (strcmp(Cptr->spev->text,"")!=0) 
                if (IsLineFeed(Cptr->spev->text[0],Cptr->spev->type))
                    fputs(&Cptr->spev->text[1],fh);
                else
                    fputs(Cptr->spev->text,fh);
            Cptr=Cptr->next;
        };
        while (Cptr!=NULL)
        {
            fputs(Cptr->spev->text,fh);
            Cptr=Cptr->next;
        };
        fputs("\n",fh);
        Lptr=Lptr->next;
    };
};
