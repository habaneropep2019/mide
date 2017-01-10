
    /*

    drawview.h  version 0.1.2

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


#ifndef DRAWVIEW_H
# define DRAWVIEW_H

#include <qwindow.h>
#include <qpainter.h>

#include "mandel.h"
#include "julia.h"




#define KFRACT_INITIAL_SIZE_X 400
#define KFRACT_INITIAL_SIZE_Y 300
#define KFRACT_SIZE_DIFF_TOP    0
#define KFRACT_SIZE_BORDER      2
#define KFRACT_SIZE_DIFF_X    KFRACT_SIZE_BORDER
#define KFRACT_SIZE_DIFF_Y    KFRACT_SIZE_DIFF_TOP+KFRACT_SIZE_BORDER



class DrawView : public QWindow
  {
  public:
    enum ColorScheme { Rgb, Hsv };
    enum FractRunning { FUnknown, FMandel, FJulia };
    DrawView( QWidget *parent );
    int getActualIter();
    int getDefaultIter();
    double getActualBailout();
    double getDefaultBailout();
    double getActualCenterX();
    double getDefaultCenterX();
    double getActualCenterY();
    double getDefaultCenterY();
    double getActualWidth();
    double getDefaultWidth();
    double getActualExtraX();
    double getDefaultExtraX();
    double getActualExtraY();
    double getDefaultExtraY();
    double getActualZoomInFactor();
    double getDefaultZoomInFactor();
    double getActualZoomOutFactor();
    double getDefaultZoomOutFactor();
    ColorScheme getActualColorScheme();
    void setNewType( FractRunning new_fract );
    void setTypeUndo();
    void setNewIter( int new_iter );
    void setNewBailout( double new_bailout );
    void setNewCenterX( double new_center_x );
    void setNewCenterY( double new_center_y );
    void setNewWidth( double new_width );
    void setNewExtraX( double new_extra_x );
    void setNewExtraY( double new_extra_y );
    void setNewZoomInFactor( double new_zoom_in_factor );
    void setNewZoomOutFactor( double new_zoom_out_factor );
    void setNewColorScheme( ColorScheme scheme );
    void changeToMandel();
    void changeToJulia();
    void stop();
    void doZoomIn();
    void doZoomOut();
    void goDefaults();
    void loadParam( char *name = NULL );
    void saveParam();
    void savePic();
  protected:
    void paintEvent( QPaintEvent * );
    void resizeEvent( QResizeEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );
  private:
    void drawCheck();
    void draw();
    void drawMap();
    void magnify( QPoint center, double mag_factor );
    int drawPoint( int x, int y );
    void reset();
    void resetOnChange();
    QWidget *w;
    QPainter *paint;
    QPainter *paint_buf;
    QPixmap *drawbuffer;
    Fract *fract;
    Fract *test_fract;
    int my_button;
    QPoint point_1, point_2;
    double my_width, my_height;
    double center_x, center_y;
    double bail_out;
    int max_iter;
    double fract_ratio;
    double param_1, param_2;
    int old_size_x, old_size_y;
    double zoom_in, zoom_out;
    bool draw_runs, draw_stop, draw_force;
    int points_drawn;
    int *my_map;
    FractRunning last_fract;
    FractRunning new_type;
    ColorScheme act_color_scheme;
    unsigned color_factor;
  };


#endif    // DRAWVIEW_H
