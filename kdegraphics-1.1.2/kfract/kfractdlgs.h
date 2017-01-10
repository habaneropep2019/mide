
    /*

    kfractdlgs.h  version 0.1.2

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

#ifndef KFRACTDLGS_H
# define KFRACTDLGS_H

#include <qdialog.h>
#include <qlabel.h>
#include <qscrbar.h>
#include <qradiobt.h>
#include <krestrictedline.h>


#include "drawview.h"



class TypeDlg : public QDialog
  {
  Q_OBJECT
  public:
    TypeDlg( int number_of_headers,
             int number_of_footers,
             int iter, int iter_default,
             double bailout, double bailout_default,
             double center_x, double center_x_default,
             double center_y, double center_y_default,
             double width, double width_default,
             double extra_x, double extra_x_default,
             double extra_y, double extra_y_default,
             QWidget *parent = 0, const char *name = 0 );
  signals:
    void iterChanged( int value );
    void bailoutChanged( double value );
    void centerXChanged( double value );
    void centerYChanged( double value );
    void widthChanged( double value );
    void defaultsChosen();
    void okChosen();
    void changedType();
  protected:
    int my_number_of_headers;
    int my_y;
    double my_extra_x, my_extra_x_default;
    double my_extra_y, my_extra_y_default;
  protected slots:
    void myIter1Changed( int new_iter );
    void myIter1000Changed( int new_iter );
    void typeDefaults();
    void typeAccept();
    void setBailout();
    void setCenterX();
    void setCenterY();
    void setWidth();
  private:
    QScrollBar  *bar_iter_1, *bar_iter_1000;
    QLabel *num_iter;
    KRestrictedLine *center_x_input, *center_y_input;
    KRestrictedLine *bailout_input;
    KRestrictedLine *width_input;
    int my_number_of_footers;
    int my_iter, my_iter_default;
    double my_bailout, my_bailout_default;
    double my_center_x, my_center_x_default;
    double my_center_y, my_center_y_default;
    double my_width, my_width_default;
  };




class MandelDlg : public TypeDlg
  {
  Q_OBJECT
  public:
    MandelDlg( int iter, int iter_default,
               double bailout, double bailout_default,
               double center_x, double center_x_default,
               double center_y, double center_y_default,
               double width, double width_default,
               QWidget *parent = 0, const char *name = 0 );
  signals:
    void changedToMandel();
  protected:
  private slots:
    void typeHasChanged();
  private:
  };



class JuliaDlg : public TypeDlg
  {
  Q_OBJECT
  public:
    JuliaDlg( int iter, int iter_default,
              double bailout, double bailout_default,
              double center_x, double center_x_default,
              double center_y, double center_y_default,
              double width, double width_default,
              double extra_x, double extra_x_default,
              double extra_y, double extra_y_default,
              QWidget *parent = 0, const char *name = 0 );
  signals:
    void extraXChanged( double value );
    void extraYChanged( double value );
    void changedToJulia();
  protected:
  private slots:
    void setExtraX();
    void setExtraY();
    void typeHasChanged();
    void juliaDefaults();
    void juliaAccept();
  private:
    KRestrictedLine *extra_x_input, *extra_y_input;
  };




class IterDlg : public QDialog
  {
  Q_OBJECT
  public:
    IterDlg( int iter, int iter_default,
             QWidget *parent = 0, const char *name = 0 );
  signals:
    void iterChanged( int value );
  protected:
  private slots:
    void myIter1Changed( int new_iter );
    void myIter1000Changed( int new_iter );
    void iterDefaults();
    void iterAccept();
  private:
    QScrollBar *bar_iter_1, *bar_iter_1000;
    QLabel *num_iter;
    int my_iter, my_iter_default;
    int my_y;
  };



class ColorDlg : public QDialog
  {
  Q_OBJECT
  public:
    ColorDlg( DrawView::ColorScheme scheme, 
              QWidget *parent = 0, const char *name = 0 );
  signals:
    void colorChanged( DrawView::ColorScheme new_scheme );
  protected:
  private slots:
    void colorAccept();
  private:
    QRadioButton *hsv_button, *rgb_button;
  };



class ZoomDlg : public QDialog
  {
  Q_OBJECT
  public:
    ZoomDlg( double zoom_in, double zoom_in_default,
             double zoom_out, double zoom_out_default,
             QWidget *parent = 0, const char *name = 0 );
  signals:
    void zoomInChanged( double new_zoom_in );
    void zoomOutChanged( double new_zoom_out );
  protected:
  private slots:
    void zoomAccept();
    void zoomDefaults();
    void setZoomIn();
    void setZoomOut();
  private:
    KRestrictedLine *zoom_in_input, *zoom_out_input;
    double my_zoom_in, my_zoom_in_default;
    double my_zoom_out, my_zoom_out_default;
  };


#endif     // KFRACTDLGS_H
