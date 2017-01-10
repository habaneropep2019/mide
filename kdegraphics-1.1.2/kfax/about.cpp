/*

    $Id: about.cpp,v 1.7 1998/09/14 20:43:46 kulow Exp $
       
    Copyright (C) 1997 Bernd Johannes Wuebben   
                       wuebben@math.cornell.edu

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

*/

#include "about.h"
#include "version.h"
#include <qpushbt.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <kapp.h>
#include <klocale.h>

extern KApplication *mykapp;

About::About(QWidget *parent) : QDialog(parent, "About KFax", TRUE) {

  resize(350, 250);
  setFixedSize(size());

  setCaption(i18n("About KFax"));

  QString pixdir = mykapp->kde_datadir().copy();
  pixdir +=  "/kfax/pics/";
  QPixmap pm((pixdir + "kfaxlogo.xpm").data());

  QLabel *logo = new QLabel(this);
  logo->setPixmap(pm);
  logo->setGeometry(20, (height()-pm.height())/2 - 20, pm.width(), pm.height());

  QLabel *l;
  l = new QLabel("KFax", this);
  l->setFont(QFont("Utopia", 26, QFont::Normal));

  l->setGeometry((width() - l->sizeHint().width())/2, 60,
		 l->sizeHint().width(), 
		 l->sizeHint().height());

  QString s;
  s = i18n("Version ");
  s += KFAXVERSION \
  "\nCopyright (C) 1997,98\nBernd Johannes Wuebben\n"\
  "wuebben@kde.org\n\n";

  l = new QLabel(s.data(), this);
  l->setGeometry(145,100, 250, 200);
  l->setAlignment(WordBreak|ExpandTabs);

  QPushButton *b_ok = new QPushButton(i18n("OK"), this);
  b_ok->setGeometry(width()/2-40, height() - 48, 80, 32);
  /*  b_ok->setDefault(TRUE);*/
  b_ok->setAutoDefault(TRUE);
  connect(b_ok, SIGNAL(released()),
	  this, SLOT(accept()));
  b_ok->setFocus();
}

#include "about.moc"
