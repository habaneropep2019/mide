//-*-C++-*-
#ifndef PREFS_H
#define PREFS_H

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
// Preferences header file

#include <stdio.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qradiobt.h>
#include <ktabctl.h>
#include "kconfig.h"
#include "kslider.h"

class Preferences : public QDialog 
{
      Q_OBJECT
   private:
      QPushButton	*buttonOk, *buttonApply, *buttonCancel;
      KTabCtl		*tabctl;
      QWidget		*page1, *page2;
   public:
      Preferences( QWidget *parent );
      QLineEdit		*mixerLE;
      QString	 	mixerCommand;
      KConfig		*kcfg;
      int useDevice();
signals:
	void optionsApply();
	
   public slots:
      void slotShow();
      void slotOk();
      void slotApply();
      void slotCancel();

private:
      int  devNum;
      QRadioButton *rb2_4,*rb2_5;
};


#endif /* PREFS_H */


