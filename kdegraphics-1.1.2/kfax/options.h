/*
    $Id: options.h,v 1.3 1997/12/12 13:53:46 denis Exp $

       
    Copyright (C) 1996 Bernd Johannes Wuebben   
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


#ifndef _OPTIONS_DIALOG_H_
#define _OPTIONS_DIALOG_H_

#include <qmsgbox.h>
#include <qapp.h>
#include <qfiledlg.h>
#include <qframe.h> 
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qscrbar.h>
#include <qtooltip.h>

#include <qstring.h>


struct optionsinfo {
  int geomauto;
  int resauto;
  int width ;
  int height;
  int fine;
  int landscape;
  int flip;
  int invert;
  int lsbfirst;
  int raw;
};

class KIntLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  KIntLineEdit( QWidget *parent = 0, const char *name = 0 ) 
    : QLineEdit( parent, name ) {};
  
  int getValue() { return atoi( text() ); };

protected:
	
  void keyPressEvent( QKeyEvent *e ) {
    char key = e->ascii();
    
    if( isdigit( key ) 
	|| ( e->key() == Key_Return) || ( e->key() == Key_Enter    )
	|| ( e->key() == Key_Delete) || ( e->key() == Key_Backspace)
	|| ( e->key() == Key_Left  ) || ( e->key() == Key_Right    )){

      QLineEdit::keyPressEvent( e );
      return;
    } else {
      e->ignore();
      return;
    }
  };
};

class OptionsDialog : public QDialog {

    Q_OBJECT

public:
    OptionsDialog( QWidget *parent = NULL, const char *name = NULL);

    struct optionsinfo* getInfo();
    void setWidgets(struct optionsinfo *oi);

protected:
    void focusInEvent ( QFocusEvent *);

signals:

public slots:

      void ready();
      void help();
      void geomtoggled();
      void g32toggled();
      void g4toggled();
      void g3toggled();
      
private:

    QGroupBox	 *bg;
    QLabel 	*reslabel;
    QButtonGroup *resgroup;
    QRadioButton *fine;
    QRadioButton *resauto;
    QRadioButton *normal;
    QLabel	*displaylabel;
    QButtonGroup *displaygroup;
    QCheckBox *landscape;
    QCheckBox *geomauto;
    QCheckBox *flip;
    QCheckBox *invert;

    QButtonGroup  *lsbgroup;
    QLabel        *lsblabel;
    QCheckBox *lsb;
    QButtonGroup *rawgroup;	
    QRadioButton *g3;
    QRadioButton *g32d;
    QRadioButton *g4;

    QLabel 	*rawlabel;
    
    QLabel 	*widthlabel;
    QLabel 	*heightlabel;
    KIntLineEdit *widthedit;
    KIntLineEdit *heightedit;
    
    QPushButton	 *ok_button;
    QPushButton	 *cancel_button;
    QPushButton	 *helpbutton;
    
    struct optionsinfo oi;

};


#endif
