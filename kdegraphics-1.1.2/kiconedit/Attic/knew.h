/*
    KDE Draw - a small graphics drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#ifndef __KNEWICON_H__
#define __KNEWICON_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qfiledlg.h> 
#include <qbttngrp.h>
#include <qbutton.h>
#include <qradiobt.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <kiconloader.h>
#include <kapp.h>
#include <kintegerline.h>
#include <kwizard.h>
#include "kresize.h"

class KIconListBox;
class KIconTemplateContainer;

struct KIconTemplate
{
  QString path;
  QString title;
};

void createStandardTemplates(QList<KIconTemplate> *);

class KIconListBoxItem : public QListBoxItem    
{    
public:      
  KIconListBoxItem( KIconTemplate * );

  const QString path() { return icontemplate->path.copy(); }
  KIconTemplate *iconTemplate() { return icontemplate; }

protected:      
  virtual void paint( QPainter * );      
  virtual int height( const QListBox * ) const;      
  virtual int width( const QListBox * ) const;      
private:      
  QPixmap pm;    
  KIconTemplate *icontemplate;
};

class KIconListBox : public QListBox    
{    
  Q_OBJECT
public:      
  KIconListBox( QWidget *parent ) : QListBox(parent) {} ;
  const QString path(int idx) { return ((KIconListBoxItem*)item(idx))->path(); }
  KIconTemplate *iconTemplate(int idx) { return ((KIconListBoxItem*)item(idx))->iconTemplate(); }
  
};

class KIconTemplateContainer : public QObject
{
  Q_OBJECT
public:
  KIconTemplateContainer();
  ~KIconTemplateContainer();

  /**
  * The QList must not be deleted! Delete the object instead as it automatically
  * deletes the list when there are no more instances.
  */
  static QList<KIconTemplate> *getTemplates();

protected:
  static QList<KIconTemplate> *templatelist;
  static int instances;
};

class NewSelect : public QWidget
{
  Q_OBJECT
public:
  NewSelect(QWidget *parent);
  ~NewSelect();

signals:
  void iconopenstyle(int);

protected slots:
  void buttonClicked(int);

protected:
  KWizard *wiz;
  QButtonGroup *grp;
  QRadioButton *rbscratch, *rbtempl;
  QBoxLayout *ml, *l;
};

class NewFromTemplate : public QWidget
{
  Q_OBJECT
public:
  NewFromTemplate(QWidget *parent);
  ~NewFromTemplate();

  const QString path() { return QString(templates->path(templates->currentItem())); }

public slots:
  void checkSelection(int);
 
protected:
  KIconListBox *templates;
  KWizard *wiz;
  QBoxLayout *ml, *l;
  QGroupBox *grp;
};

class KNewIcon : public KWizard
{
  Q_OBJECT
public:
  KNewIcon(QWidget *parent);
  ~KNewIcon();

  enum { Blank, Template };
  int openStyle() { return openstyle; }
  const QString templatePath() { return QString(templ->path()); }
  const QSize templateSize() { return scratch->getSize(); }

protected slots:
  void okClicked();
  void cancelClicked();
  void iconOpenStyle(int);
  void checkPage(int);
  void checkPage(bool);

signals:
  void newicon(const QSize);
  void newicon(const QString);

protected:
  //static QList<KIconTemplate> templatelist;
  //QButtonGroup *bgrptmpl, *bgrpstl;
  NewSelect       *select;
  //NewFromScratch  *scratch;
  KResize  *scratch;
  NewFromTemplate *templ;
  KWizardPage *p1, *p2;
  int openstyle;
};

#endif // __KNEWICON_H__
