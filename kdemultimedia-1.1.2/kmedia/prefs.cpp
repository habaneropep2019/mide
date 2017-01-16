/*
   Copyright (c) 1997-1998 Christian Esken (esken@kde.org)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


// $Id$
// Preferences Widget of Media player

#include <kapp.h>
#include "prefs.h"
#include "prefs.moc"
//#include "kmediaui.h"

Preferences::Preferences( QWidget *parent ) :
   QDialog( parent )
{
   int tabwidth = 300, tabheight = 200;
   devNum = 0;  // Default

   tabctl = new KTabCtl( this );
   
   page1 = new QWidget( tabctl );
   page2 = new QWidget( tabctl );
   tabctl->addTab( page1, i18n("Players") );
   tabctl->addTab( page2, i18n("Options") );
   tabctl->setGeometry( 10, 10, tabwidth, tabheight );

   int x=10,y=10;
   // Define page 1
   QLabel    *mixerLB = new QLabel(i18n("Mixer"),page1);
   mixerLB->setGeometry( 10, y, mixerLB->width(), mixerLB->height() );
   x += mixerLB->width()+4;

   mixerLE = new QLineEdit(page1,"Mixer");
   QFontMetrics qfm = fontMetrics();
   mixerLE->setGeometry(x,y, tabctl->width()-x-10, mixerLE->height() );

   // Define page 2
   QButtonGroup *grpbox2 = new QButtonGroup( i18n("Use sound device"), page2 );
   grpbox2->setGeometry( 10, 10, tabctl->width()-20, 100 );
   QRadioButton *rb2_3 = new QRadioButton( i18n("Default"), grpbox2 );
   rb2_3->move( 10, 15 );   
   rb2_3->setChecked(true);
   rb2_4 = new QRadioButton( i18n("Device 1"), grpbox2 );
   rb2_4->move( 10, 40 );
   rb2_5 = new QRadioButton( i18n("Device 2"), grpbox2 );
   rb2_5->move( 10, 65 );
   QLabel *lbl2_1 = new QLabel( "",
				page2 );
   lbl2_1->setAlignment( AlignLeft );
   lbl2_1->adjustSize();
   lbl2_1->move( (tabctl->width()-lbl2_1->width())/2, 120 );
   

   buttonOk = new QPushButton( i18n("OK"), this );
   buttonOk->setGeometry( tabwidth-250, tabheight+20, 80, 25 );
   connect( buttonOk, SIGNAL(clicked()), this, SLOT(slotOk()));

   buttonApply = new QPushButton( i18n("Apply"), this );
   buttonApply->setGeometry( tabwidth-160, tabheight+20, 80, 25 );
   connect( buttonApply, SIGNAL(clicked()), this, SLOT(slotApply()));

   buttonCancel = new QPushButton( i18n("Cancel"), this );
   buttonCancel->setGeometry( tabwidth-70, tabheight+20, 80, 25 );
   connect( buttonCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
   
   setFixedSize( tabwidth + 20, tabheight + 55 );
   setCaption( i18n("KMedia Preferences") );
}

void Preferences::slotShow()
{
   show();
}

void Preferences::slotOk()
{
  slotApply();
  hide();
}

void Preferences::slotApply()
{
  devNum = 0;
  if ( rb2_4->isChecked() ) devNum = 1;
  if ( rb2_5->isChecked() ) devNum = 2;
  mixerCommand = mixerLE->text();
  kcfg->writeEntry( "MixerCommand", mixerCommand, true );
  kcfg->sync();
  emit optionsApply();
}

void Preferences::slotCancel()
{
  hide();
}

int Preferences::useDevice()
{
  return devNum;
}