    /*

    kfractdlgs.C  version 0.1.2

    Copyright (C) 1997 Uwe Thiem   
                       uwe@uwix.alt.na

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


#include <qpushbt.h>
#include <qradiobt.h>
#include <qbttngrp.h>
#include <qlabel.h>
#include <kapp.h>
#include <stdio.h>
#include <stdlib.h>

#include "drawview.h"
#include "kfractdlgs.moc"


#define MAX_ITER 5000
#define PAGE_STEPS  10


#define D_LM 5
#define D_RM 195
#define D_TM 10
#define D_LABINCR 30
#define D_TEXTLABINCR 20
#define D_TEXTINCR 16
#define D_BARINCR 15
#define D_INPUTINCR 20
#define D_BETWEEN 10
#define D_TEXTH 15
#define D_BARH 15
#define D_INPUTH 20



TypeDlg::TypeDlg( int number_of_headers,
                  int number_of_footers,
                  int iter, int iter_default,
                  double bailout, double bailout_default,
                  double center_x, double center_x_default,
                  double center_y, double center_y_default,
                  double actual_width, double width_default,
                  double extra_x, double extra_x_default,
                  double extra_y, double extra_y_default,
                  QWidget *parent, const char *name ) :
         QDialog( parent, name, TRUE )
  {
  QLabel *text_iter, *text_iter_1, *text_iter_1000;
  QLabel *text_center_x, *text_center_y;
  QLabel *text_bailout, *text_width;
  QPushButton *ok, *cancel, *defaultb;
  int b_pos;

  my_iter = iter;
  my_iter_default = iter_default;
  my_bailout = bailout;
  my_bailout_default = bailout_default;
  my_center_x = center_x;
  my_center_x_default = center_x_default;
  my_center_y = center_y;
  my_center_y_default = center_y_default;
  my_width = actual_width;
  my_width_default = width_default;
  my_extra_x = extra_x;
  my_extra_x_default = extra_x_default;
  my_extra_y = extra_y;
  my_extra_y_default = extra_y_default;
  my_number_of_headers = number_of_headers;
  my_y = my_number_of_headers * D_LABINCR + D_TM + D_BETWEEN;
  my_number_of_footers = number_of_footers;;

  text_iter = new QLabel( this );
  CHECK_PTR( text_iter );
  text_iter->setGeometry( D_LM, my_y, 95, D_INPUTH );
  text_iter->setAlignment( AlignLeft );
  text_iter->setText( i18n("Iterations:") );
  num_iter = new QLabel( this );
  CHECK_PTR( num_iter );
  num_iter->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  num_iter->setGeometry( 130, my_y, 70, D_INPUTH );
  num_iter->setNum( my_iter );
  my_y += D_TEXTLABINCR;
  text_iter_1 = new QLabel( this );
  CHECK_PTR( text_iter_1 );
  text_iter_1->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_iter_1->setAlignment( AlignLeft );
  text_iter_1->setText( i18n("0 - 999") );
  my_y += D_TEXTINCR;
  if ( my_iter < 1000 )
    { 
    bar_iter_1 = new QScrollBar( 1, 999, 1, PAGE_STEPS, 
                                 my_iter % 1000, 
                                 QScrollBar::Horizontal, this );
    }
  else
    {
    bar_iter_1 = new QScrollBar( 0, 999, 1, PAGE_STEPS,
                                 my_iter % 1000,
                                 QScrollBar::Horizontal, this );
    }
  CHECK_PTR( bar_iter_1 );
  bar_iter_1->setGeometry( D_LM, my_y, D_RM, D_BARH );
  connect( bar_iter_1, SIGNAL( valueChanged( int ) ),
           this, SLOT( myIter1Changed( int ) ) );
  my_y += D_BARINCR;
  text_iter_1000 = new QLabel( this );
  text_iter_1000->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_iter_1000->setAlignment( AlignLeft );
  text_iter_1000->setText( i18n("1000 - 100000") );
  my_y += D_TEXTINCR;
  bar_iter_1000 = new QScrollBar( 0, 100, 1, 1,
                                  ( my_iter - ( my_iter % 1000 ) ) / 1000,
                                  QScrollBar::Horizontal, this );
  CHECK_PTR( bar_iter_1000 );
  bar_iter_1000->setGeometry( D_LM, my_y, D_RM, D_BARH );
  connect( bar_iter_1000, SIGNAL( valueChanged( int ) ),
           this, SLOT( myIter1000Changed( int ) ) ); 
  my_y += D_BARINCR + D_BETWEEN;

  text_bailout = new QLabel( this );
  CHECK_PTR( text_bailout );
  text_bailout->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_bailout->setAlignment( AlignLeft );
  text_bailout->setText( i18n("Bailout") );
  my_y += D_TEXTINCR;
  bailout_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( bailout_input );
  bailout_input->setGeometry( D_LM, my_y, D_RM, D_INPUTH );
  bailout_input->setMaxLength( 40 );
  char line[41];
  sprintf( line, "%.20f", my_bailout );
  bailout_input->setText( line );
  connect( bailout_input, SIGNAL( returnPressed() ),
           this, SLOT( setBailout() ) );
  my_y += D_INPUTINCR + D_BETWEEN;

  text_center_x = new QLabel( this );
  CHECK_PTR( text_center_x );
  text_center_x->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_center_x->setAlignment( AlignLeft );
  text_center_x->setText( i18n("Center of picture: Real part") );
  my_y += D_TEXTINCR;
  center_x_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( center_x_input );
  center_x_input->setGeometry( D_LM, my_y, D_RM, D_INPUTH );
  center_x_input->setMaxLength( 40 );
  sprintf( line, "%.20f", my_center_x );
  center_x_input->setText ( line );
  connect( center_x_input, SIGNAL( returnPressed() ),
           this, SLOT ( setCenterX() ) );
  my_y += D_INPUTINCR;
  text_center_y = new QLabel( this );
  CHECK_PTR( text_center_y );
  text_center_y->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_center_y->setAlignment( AlignLeft );
  text_center_y->setText( i18n("Center of picture: Imaginary part") );
  my_y += D_TEXTINCR;
  center_y_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( center_y_input );
  center_y_input->setGeometry( D_LM, my_y, D_RM, D_INPUTH );
  center_y_input->setMaxLength( 40 );
  sprintf( line, "%.20f", my_center_y );
  center_y_input->setText( line );
  connect( center_y_input, SIGNAL( returnPressed() ),
           this, SLOT( setCenterY() ) );
  my_y += D_INPUTINCR + D_BETWEEN;

  text_width = new QLabel( this );
  CHECK_PTR( text_width );
  text_width->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_width->setAlignment( AlignLeft );
  text_width->setText( i18n("Width of picture:") );
  my_y += D_TEXTINCR;
  width_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( width_input );
  width_input->setGeometry( D_LM, my_y, D_RM, D_INPUTH );
  width_input->setMaxLength( 40 );
  sprintf( line, "%.20f", my_width );
  width_input->setText( line );
  connect( width_input, SIGNAL( returnPressed() ),
           this, SLOT( setWidth() ) );
  if ( my_number_of_footers > 0 )
    {
    my_y += D_INPUTINCR + D_BETWEEN;
    }
  else
    {
    my_y += D_INPUTINCR;
    }
  
  b_pos = my_y + my_number_of_footers * ( D_TEXTINCR + D_INPUTINCR )
               + D_BETWEEN;
  ok = new QPushButton( this );
  CHECK_PTR( ok );
  ok->setText( i18n("OK") );
  ok->setGeometry( D_LM, b_pos, 60, 30 );
  connect( ok, SIGNAL( clicked() ), SLOT( typeAccept() ) );
  defaultb = new QPushButton( this );
  CHECK_PTR( defaultb );
  defaultb->setText( i18n("Defaults") );
  defaultb->setGeometry( 70, b_pos, 60, 30 );
  connect( defaultb, SIGNAL( clicked() ), SLOT( typeDefaults() ) );
  cancel = new QPushButton( this );
  CHECK_PTR( cancel );
  cancel->setText( i18n("Cancel") );
  cancel->setGeometry( 135, b_pos, 60, 30 );
  connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );

//  adjustSize();
//  setFixedSize( width(), height() );
  }


void TypeDlg::myIter1Changed( int new_iter )
  {
  my_iter = my_iter - ( my_iter % 1000 ) + new_iter;
  num_iter->setNum( my_iter );
  }


void TypeDlg::myIter1000Changed( int new_iter )
  {
  if ( new_iter < 1 )
    {
    bar_iter_1->setRange( 1, 999 );
    if ( my_iter == 0 )
      {
      my_iter++;
      }
    }
  else
    {
    bar_iter_1->setRange( 0, 999 );
    }
  my_iter = (my_iter % 1000 ) + new_iter * 1000;
  num_iter->setNum( my_iter );
  }


void TypeDlg::typeDefaults()
  {
  my_iter = my_iter_default;
  num_iter->setNum( my_iter );
  bar_iter_1->setValue( my_iter % 1000 );
  bar_iter_1000->setValue( ( my_iter - ( my_iter % 1000 ) ) / 1000 );
  if ( my_iter < 1000 )
    {
    bar_iter_1->setRange( 1, 999 );
    }
  else
    {
    bar_iter_1->setRange( 0, 999 );
    }
  my_bailout = my_bailout_default;
  char line[41];
  sprintf( line, "%.20f", my_bailout );
  bailout_input->setText( line );
  my_center_x = my_center_x_default;
  sprintf( line, "%.20f", my_center_x );
  center_x_input->setText( line );
  my_center_y = my_center_y_default;
  sprintf( line, "%.20f", my_center_y );
  center_y_input->setText( line );
  my_width = my_width_default;
  sprintf( line, "%.20f", my_width );
  width_input->setText( line );
  emit defaultsChosen();
  }


void TypeDlg::typeAccept()
  {
  emit changedType();
  emit iterChanged( my_iter );
  emit bailoutChanged( my_bailout );
  emit centerXChanged( my_center_x );
  emit centerYChanged( my_center_y );
  emit widthChanged( my_width );
  emit okChosen();
  accept();
  }


void TypeDlg::setBailout()
  {
  char *endptr;

  double value = strtod( bailout_input->text(), &endptr );
  if ( endptr == ( bailout_input->text() + 
                   strlen( bailout_input->text() ) ) )
    {
    my_bailout = value;
    }
  else
    {
    char line[41];
    sprintf( line, "%.20f", my_bailout );
    bailout_input->setText( line );
    }
  }


void TypeDlg::setCenterX()
  {
  char *endptr;

  double value = strtod( center_x_input->text(), &endptr );
  if ( endptr == ( center_x_input->text() + 
                   strlen( center_x_input->text() ) ) )
    {
    my_center_x = value;
    }
  else
    {
    char line[41];
    sprintf( line, "%.20f", my_center_x );
    center_x_input->setText( line );
    }
  }


void TypeDlg::setCenterY()
  {
  char *endptr;

  double value = strtod( center_y_input->text(), &endptr );
  if ( endptr == ( center_y_input->text() + 
                   strlen( center_y_input->text() ) ) )
    {
    my_center_y = value;
    }
  else
    {
    char line[41];
    sprintf( line, "%.20f", my_center_y );
    center_y_input->setText( line );
    }
  }


void TypeDlg::setWidth()
  {
  char *endptr;

  double value = strtod( width_input->text(), &endptr );
  if ( endptr == ( width_input->text() + 
                   strlen( width_input->text() ) ) )
    {
    my_width = value;
    }
  else
    {
    char line[41];
    sprintf( line, "%.20f", my_width );
    width_input->setText( line );
    }
  }






//###################################################################

MandelDlg::MandelDlg( int iter, int iter_default,
                      double bailout, double bailout_default,
                      double center_x, double center_x_default,
                      double center_y, double center_y_default,
                      double actual_width, double width_default,
                      QWidget *parent, const char *name ) :
            TypeDlg( 2, 0,
                     iter, iter_default,
                     bailout, bailout_default,
                     center_x, center_x_default,
                     center_y, center_y_default,
                     actual_width, width_default,
                     0.0, 0.0,
                     0.0, 0.0,
                     parent, name )
  {
  QLabel *header1, *header2;

  header1 = new QLabel( this );
  CHECK_PTR( header1 );
  header1->setGeometry( D_LM, D_TM, D_RM, D_INPUTH );
  header1->setAlignment( AlignCenter );
  header1->setText( i18n("z(0) = c = pixel") );
  header2 = new QLabel( this );
  CHECK_PTR( header2 );
  header2->setGeometry( D_LM, D_TM + D_TEXTLABINCR, D_RM, D_INPUTH );
  header2->setAlignment( AlignCenter );
  header2->setText( i18n("z(n+1) = z(n)^2 + c") );

  connect( this, SIGNAL( changedType() ),
           this, SLOT( typeHasChanged() ) );
  setCaption( i18n("Mandelbrot") );
  adjustSize();
  setFixedSize( width(), height() );
  }



void MandelDlg::typeHasChanged()
  {
  emit( changedToMandel() );
  }





//####################################################################

JuliaDlg::JuliaDlg( int iter, int iter_default,
                    double bailout, double bailout_default,
                    double center_x, double center_x_default,
                    double center_y, double center_y_default,
                    double actual_width, double width_default,
                    double extra_x, double extra_x_default,
                    double extra_y, double extra_y_default,
                    QWidget *parent, const char *name ) :
            TypeDlg( 2, 2,
                     iter, iter_default,
                     bailout, bailout_default,
                     center_x, center_x_default,
                     center_y, center_y_default,
                     actual_width, width_default,
                     extra_x, extra_x_default,
                     extra_y, extra_y_default,
                     parent, name )
  {
  QLabel *header1, *header2;
  QLabel *text_extra_x, *text_extra_y;

  my_extra_x = extra_x;
  my_extra_x_default = extra_x_default;
  my_extra_y = extra_y;
  my_extra_y_default = extra_y_default;

  header1 = new QLabel( this );
  CHECK_PTR( header1 );
  header1->setGeometry( D_LM, D_TM, D_RM, D_INPUTH );
  header1->setAlignment( AlignCenter );
  header1->setText( "z(0) = pixel" );
  header2 = new QLabel( this );
  CHECK_PTR( header2 );
  header2->setGeometry( D_LM, D_TM + D_TEXTLABINCR, D_RM, D_INPUTH );
  header2->setAlignment( AlignCenter );
  header2->setText( "z(n+1) = z(n)^2 + c" );

  text_extra_x = new QLabel( this );
  CHECK_PTR( text_extra_x );
  text_extra_x->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_extra_x->setAlignment( AlignLeft );
  text_extra_x->setText( i18n("Extra parameter: Real part") );
  my_y += D_TEXTINCR;
  extra_x_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( extra_x_input );
  extra_x_input->setGeometry( D_LM, my_y, D_RM, D_INPUTH );
  extra_x_input->setMaxLength( 40 );
  char line[41];
  sprintf( line, "%.20f", my_extra_x );
  extra_x_input->setText ( line );
  connect( extra_x_input, SIGNAL( returnPressed() ),
           this, SLOT ( setExtraX() ) );
  my_y += D_INPUTINCR;
  text_extra_y = new QLabel( this );
  CHECK_PTR( text_extra_y );
  text_extra_y->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_extra_y->setAlignment( AlignLeft );
  text_extra_y->setText( i18n("Extra parameter: Imaginary part") );
  my_y += D_TEXTINCR;
  extra_y_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( extra_y_input );
  extra_y_input->setGeometry( D_LM, my_y, D_RM, D_INPUTH );
  extra_y_input->setMaxLength( 40 );
  sprintf( line, "%.20f", my_extra_y );
  extra_y_input->setText( line );
  connect( extra_y_input, SIGNAL( returnPressed() ),
           this, SLOT( setExtraY() ) );

  connect( this, SIGNAL( changedType() ),
           this, SLOT( typeHasChanged() ) );
  setCaption( i18n("Julia") );
  adjustSize();
  setFixedSize( width(), height() );

  connect( this, SIGNAL( defaultsChosen() ),
           this, SLOT( juliaDefaults() ) );
  connect( this, SIGNAL( okChosen() ),
           this, SLOT( juliaAccept() ) );
  }



void JuliaDlg::typeHasChanged()
  {
  emit( changedToJulia() );
  }



void JuliaDlg::setExtraX()
  {
  char *endptr;

  double value = strtod( extra_x_input->text(), &endptr );
  if ( endptr == ( extra_x_input->text() + 
                   strlen( extra_x_input->text() ) ) )
    {
    my_extra_x = value;
    }
  else
    {
    char line[41];
    sprintf( line, "%.20f", my_extra_x );
    extra_x_input->setText( line );
    }
  }


void JuliaDlg::setExtraY()
  {
  char *endptr;

  double value = strtod( extra_y_input->text(), &endptr );
  if ( endptr == ( extra_y_input->text() + 
                   strlen( extra_y_input->text() ) ) )
    {
    my_extra_y = value;
    }
  else
    {
    char line[41];
    sprintf( line, "%.20f", my_extra_y );
    extra_y_input->setText( line );
    }
  }



void JuliaDlg::juliaDefaults()
  {
  char line[41];
  my_extra_x = my_extra_x_default;
  sprintf( line, "%.20f", my_extra_x );
  extra_x_input->setText( line );
  my_extra_y = my_extra_y_default;
  sprintf( line, "%.20f", my_extra_y );
  extra_y_input->setText( line );
  }



void JuliaDlg::juliaAccept()
  {
  emit( extraXChanged( my_extra_x ) );
  emit( extraYChanged( my_extra_y ) );
  }



//####################################################################



IterDlg::IterDlg( int iter, int iter_default,
                      QWidget *parent, const char *name ) :
            QDialog( parent, name, TRUE )
  {
  QLabel *text_iter, *text_iter_1, *text_iter_1000;
  QPushButton *ok, *cancel, *defaultb;

  my_iter = iter;
  my_iter_default = iter_default;
  setCaption( i18n("Max. Iterations") );
  my_y = D_TM;
  
  text_iter = new QLabel( this );
  CHECK_PTR( text_iter );
  text_iter->setGeometry( D_LM, my_y, 95, D_INPUTH );
  text_iter->setAlignment( AlignLeft );
  text_iter->setText( i18n("Iterations:") );
  num_iter = new QLabel( this );
  CHECK_PTR( num_iter );
  num_iter->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  num_iter->setGeometry( 130, my_y, 70, D_INPUTH );
  num_iter->setNum( my_iter );
  my_y += D_TEXTLABINCR;
  text_iter_1 = new QLabel( this );
  CHECK_PTR( text_iter_1 );
  text_iter_1->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_iter_1->setAlignment( AlignLeft );
  text_iter_1->setText( i18n("0 - 999") );
  my_y += D_TEXTINCR;
  if ( my_iter < 1000 )
    { 
    bar_iter_1 = new QScrollBar( 1, 999, 1, PAGE_STEPS, 
                                 my_iter % 1000, 
                                 QScrollBar::Horizontal, this );
    }
  else
    {
    bar_iter_1 = new QScrollBar( 0, 999, 1, PAGE_STEPS,
                                 my_iter % 1000,
                                 QScrollBar::Horizontal, this );
    }
  CHECK_PTR( bar_iter_1 );
  bar_iter_1->setGeometry( D_LM, my_y, D_RM, D_BARH );
  connect( bar_iter_1, SIGNAL( valueChanged( int ) ),
           this, SLOT( myIter1Changed( int ) ) );
  my_y += D_BARINCR;
  text_iter_1000 = new QLabel( this );
  text_iter_1000->setGeometry( D_LM, my_y, D_RM, D_TEXTH );
  text_iter_1000->setAlignment( AlignLeft );
  text_iter_1000->setText( i18n("1000 - 100000") );
  my_y += D_TEXTINCR;
  bar_iter_1000 = new QScrollBar( 0, 100, 1, 1,
                                  ( my_iter - ( my_iter % 1000 ) ) / 1000,
                                  QScrollBar::Horizontal, this );
  CHECK_PTR( bar_iter_1000 );
  bar_iter_1000->setGeometry( D_LM, my_y, D_RM, D_BARH );
  connect( bar_iter_1000, SIGNAL( valueChanged( int ) ),
           this, SLOT( myIter1000Changed( int ) ) ); 
  my_y += D_BARINCR + D_BETWEEN;

  ok = new QPushButton( this );
  CHECK_PTR( ok );
  ok->setText( i18n("OK") );
  ok->setGeometry( D_LM, my_y, 60, 30 );
  connect( ok, SIGNAL( clicked() ), SLOT( iterAccept() ) );
  defaultb = new QPushButton( this );
  CHECK_PTR( defaultb );
  defaultb->setText( i18n("Defaults") );
  defaultb->setGeometry( 70, my_y, 60, 30 );
  connect( defaultb, SIGNAL( clicked() ), SLOT( iterDefaults() ) );
  cancel = new QPushButton( this );
  CHECK_PTR( cancel );
  cancel->setText( i18n("Cancel") );
  cancel->setGeometry( 135, my_y, 60, 30 );
  connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );


  adjustSize();
  setFixedSize( width(), height() );
  }



void IterDlg::myIter1Changed( int new_iter )
  {
  my_iter = my_iter - ( my_iter % 1000 ) + new_iter;
  num_iter->setNum( my_iter );
  }


void IterDlg::myIter1000Changed( int new_iter )
  {
  if ( new_iter < 1 )
    {
    bar_iter_1->setRange( 1, 999 );
    if ( my_iter == 0 )
      {
      my_iter++;
      }
    }
  else
    {
    bar_iter_1->setRange( 0, 999 );
    }
  my_iter = (my_iter % 1000 ) + new_iter * 1000;
  num_iter->setNum( my_iter );
  }


void IterDlg::iterDefaults()
  {
  my_iter = my_iter_default;
  num_iter->setNum( my_iter );
  bar_iter_1->setValue( my_iter % 1000 );
  bar_iter_1000->setValue( ( my_iter - ( my_iter % 1000 ) ) / 1000 );
  if ( my_iter < 1000 )
    {
    bar_iter_1->setRange( 1, 999 );
    }
  else
    {
    bar_iter_1->setRange( 0, 999 );
    }
  }



void IterDlg::iterAccept()
  {
  emit iterChanged( my_iter );
  accept();
  }



//#############################################################


ColorDlg::ColorDlg( DrawView::ColorScheme scheme, 
                    QWidget *parent, const char *name ) :
           QDialog( parent, name, TRUE )
  {
  QButtonGroup *bg;
  QPushButton *ok, *cancel;

  hsv_button = new QRadioButton( i18n("Less colours"), this );
  CHECK_PTR( hsv_button );
  hsv_button->setGeometry( D_LM + 40, D_TM, 155, 20 );
  rgb_button = new QRadioButton( i18n("More colours"), this );
  CHECK_PTR( rgb_button );
  rgb_button->setGeometry( D_LM + 40, D_TM + 30, 155, 20 );
  if ( scheme == DrawView::Rgb )
    {
    rgb_button->setChecked( TRUE );
    }
  else
    {
    hsv_button->setChecked( TRUE );
    }

  bg = new QButtonGroup( this );
  CHECK_PTR( bg );
  bg->hide();
  bg->insert( hsv_button );
  bg->insert( rgb_button );

  ok = new QPushButton( this );
  CHECK_PTR( ok );
  ok->setText( i18n("OK") );
  ok->setGeometry( D_LM, D_TM + 60, 60, 30 );
  connect( ok, SIGNAL( clicked() ), SLOT( colorAccept() ) );
  cancel = new QPushButton( this );
  CHECK_PTR( cancel );
  cancel->setText( i18n("Cancel") );
  cancel->setGeometry( 135, D_TM + 60, 60, 30 );
  connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );
  setCaption( i18n("Colour schemes") );
  adjustSize();
  setFixedSize( width(), height() );
  }



void ColorDlg::colorAccept()
  {
  if ( hsv_button->isChecked() )
    {
    emit colorChanged( DrawView::Hsv );
    }
  else
    {
    emit colorChanged( DrawView::Rgb );
    }
  accept();
  }


//##############################################################

ZoomDlg::ZoomDlg( double zoom_in, double zoom_in_default,
                  double zoom_out, double zoom_out_default,
                  QWidget *parent, const char *name ) :
         QDialog( parent, name, TRUE )
  {
  QPushButton *ok, *defaultb, *cancel;
  QLabel *text_zoom_in, *text_zoom_out;
  int my_y;

  my_zoom_in = zoom_in;
  my_zoom_in_default = zoom_in_default;
  my_zoom_out = zoom_out;
  my_zoom_out_default = zoom_out_default;

  my_y = D_TM;
  text_zoom_in = new QLabel( this );
  CHECK_PTR( text_zoom_in );
  text_zoom_in->setGeometry( D_LM, my_y, 95, D_INPUTH );
  text_zoom_in->setAlignment( AlignLeft );
  text_zoom_in->setText( i18n("Zoom in factor:") );
  zoom_in_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( zoom_in_input );
  zoom_in_input->setGeometry( 100, my_y, 95, D_INPUTH );
  zoom_in_input->setMaxLength( 20 );
  char line[21];
  sprintf( line, "%.10f", my_zoom_in );
  zoom_in_input->setText ( line );
  connect( zoom_in_input, SIGNAL( returnPressed() ),
           this, SLOT ( setZoomIn() ) );
  my_y += D_TEXTLABINCR;
 
  text_zoom_out = new QLabel( this );
  CHECK_PTR( text_zoom_out );
  text_zoom_out->setGeometry( D_LM, my_y, 95, D_INPUTH );
  text_zoom_out->setAlignment( AlignLeft );
  text_zoom_out->setText( i18n("Zoom out factor:") );
  zoom_out_input = new KRestrictedLine( this, NULL, "0123456789.-+" );
  CHECK_PTR( zoom_out_input );
  zoom_out_input->setGeometry( 100, my_y, 95, D_INPUTH );
  zoom_out_input->setMaxLength( 20 );
  sprintf( line, "%.10f", my_zoom_out );
  zoom_out_input->setText ( line );
  connect( zoom_out_input, SIGNAL( returnPressed() ),
           this, SLOT ( setZoomOut() ) );
  my_y += D_TEXTLABINCR + D_BETWEEN;
 
  ok = new QPushButton( this );
  CHECK_PTR( ok );
  ok->setText( i18n("OK") );
  ok->setGeometry( D_LM, my_y, 60, 30 );
  connect( ok, SIGNAL( clicked() ), SLOT( zoomAccept() ) );
  defaultb = new QPushButton( this );
  CHECK_PTR( defaultb );
  defaultb->setText( i18n("Defaults") );
  defaultb->setGeometry( 70, my_y, 60, 30 );
  connect( defaultb, SIGNAL( clicked() ), SLOT( zoomDefaults() ) );
  cancel = new QPushButton( this );
  CHECK_PTR( cancel );
  cancel->setText( i18n("Cancel") );
  cancel->setGeometry( 135, my_y, 60, 30 );
  connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );

  setCaption( i18n("Zoom factors") );
  adjustSize();
  setFixedSize( width(), height() ); 
  }



void ZoomDlg::setZoomIn()
  {
  char *endptr;

  double value = strtod( zoom_in_input->text(), &endptr );
  if ( endptr == ( zoom_in_input->text() + 
                   strlen( zoom_in_input->text() ) ) )
    {
    my_zoom_in = value;
    }
  else
    {
    char line[21];
    sprintf( line, "%.10f", my_zoom_in );
    zoom_in_input->setText( line );
    }
  }


void ZoomDlg::setZoomOut()
  {
  char *endptr;

  double value = strtod( zoom_out_input->text(), &endptr );
  if ( endptr == ( zoom_out_input->text() + 
                   strlen( zoom_out_input->text() ) ) )
    {
    my_zoom_out = value;
    }
  else
    {
    char line[21];
    sprintf( line, "%.10f", my_zoom_out );
    zoom_out_input->setText( line );
    }
  }


void ZoomDlg::zoomDefaults()
  {
  char line[21];
  my_zoom_in = my_zoom_in_default;
  sprintf( line, "%.10f", my_zoom_in );
  zoom_in_input->setText( line );
  my_zoom_out = my_zoom_out_default;
  sprintf( line, "%.10f", my_zoom_out );
  zoom_out_input->setText( line );
  }



void ZoomDlg::zoomAccept()
  {
  emit( zoomInChanged( my_zoom_in ) );
  emit( zoomOutChanged( my_zoom_out ) );
  accept();
  }


