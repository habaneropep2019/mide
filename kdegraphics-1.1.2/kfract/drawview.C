
    /*

    drawview.C  version 0.1.2

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

#include <stdio.h>
#include <stdlib.h>

#include <qapp.h>
#include <qimage.h>
#include <qfiledlg.h>
#include <qtstream.h>

#include "drawview.h"


#define FRACT_RATIO 0.01
#define FRACT_CENTER_X  -0.5
#define FRACT_CENTER_Y  -0.0
#define DEFAULT_ITER 100
#define JULIA_X 0.3
#define JULIA_Y 0.6
#define MAXCOLOR 16777215


DrawView::DrawView( QWidget *parent ) :
    QWindow( parent )
  {
  setBackgroundColor( black );
  w = parent;
  paint = new QPainter( this );
  CHECK_PTR( paint );
  fract = new Mandel();
  CHECK_PTR( fract );
  last_fract = FMandel;
  paint_buf = NULL;
  drawbuffer = NULL;
  my_map = NULL;
  reset();
  draw_runs = draw_stop = FALSE;
  }



void DrawView::reset()
  {
  my_button = NoButton;
  point_1.setX( 0 );
  point_1.setY( 0 );
  point_2.setX( 0 );
  point_2.setX( 0 );
  center_x = FRACT_CENTER_X;
  center_y = FRACT_CENTER_Y;
  my_width = KFRACT_INITIAL_SIZE_X / 100; 
  my_height = KFRACT_INITIAL_SIZE_Y / 100;
  bail_out = 4.0;
  max_iter = DEFAULT_ITER;
  fract_ratio = FRACT_RATIO;
  old_size_x = KFRACT_INITIAL_SIZE_X;
  old_size_y = KFRACT_INITIAL_SIZE_Y;
  param_1 = 0.0;
  param_2 = 0.0;
  zoom_in = 0.5;
  zoom_out = 2.0;
  draw_runs = draw_stop = FALSE;
  draw_force = TRUE;
  if ( my_map != NULL )
    {
    delete my_map;
    }
  my_map = new int[old_size_x * old_size_y * sizeof( int )];
  CHECK_PTR( my_map );
  if ( drawbuffer != NULL )
    {
    delete drawbuffer;
    }
  drawbuffer = new QPixmap( old_size_x, old_size_y + KFRACT_SIZE_DIFF_TOP );
  CHECK_PTR( drawbuffer );
  drawbuffer->fill( black );
  if ( paint_buf != NULL )
    {
    delete paint_buf;
    }
  paint_buf = new QPainter( drawbuffer );
  CHECK_PTR( paint_buf );
  points_drawn = 0;
  center_x = fract->defaultCenterX();
  center_y = fract->defaultCenterY();
  act_color_scheme = Rgb;    //Hsv;
  color_factor = MAXCOLOR / max_iter;
  test_fract = NULL;
  }



void DrawView::resetOnChange()
  {
  my_button = NoButton;
  point_1.setX( 0 );
  point_1.setY( 0 );
  point_2.setX( 0 );
  point_2.setX( 0 );
  draw_runs = draw_stop = FALSE;
  draw_force = TRUE;
  drawbuffer->fill( black );
  points_drawn = 0;
  center_x = fract->defaultCenterX();
  center_y = fract->defaultCenterY();
  param_1 = fract->defaultExtraX();
  param_2 = fract->defaultExtraY();
  my_width = fract->defaultWidth();
  my_height = ( my_width * ( height() - KFRACT_SIZE_DIFF_TOP ) ) / width();
  fract_ratio = my_width / width();
  bail_out = fract->defaultBailout();
  }


void DrawView::changeToMandel()
  {
  if ( last_fract != FMandel )
    {
    last_fract = FMandel;
    if ( fract != NULL )
      {
      delete fract;
      }
    fract = new Mandel();
    CHECK_PTR( fract );
    resetOnChange();
    param_1 = 0.0;
    param_2 = 0.0;
    }
  }




void DrawView::changeToJulia()
  {
  if ( last_fract != FJulia )
    {
    last_fract = FJulia;
    if ( fract != NULL )
      {
      delete fract;
      }
    fract = new Julia();
    CHECK_PTR( fract );
    resetOnChange();
    param_1 = JULIA_X;
    param_2 = JULIA_Y;
    }
  }



void DrawView::doZoomIn()
  {
  QPoint center;

  center.setX( old_size_x / 2 );
  center.setY( old_size_y / 2 );
  magnify( center, zoom_in );
  }



void DrawView::doZoomOut()
  {
  QPoint center;

  center.setX( old_size_x / 2 );
  center.setY( old_size_y / 2 );
  magnify( center, zoom_out );
  }



void DrawView::goDefaults()
  {
  setNewIter( DEFAULT_ITER );
  setNewBailout( fract->defaultBailout() );
  setNewCenterX( fract->defaultCenterX() );
  setNewCenterY( fract->defaultCenterY() );
  setNewWidth( fract->defaultWidth() );
  setNewExtraX( fract->defaultExtraX() );
  setNewExtraY( fract->defaultExtraY() );
  }



void DrawView::paintEvent( QPaintEvent * )
  {
  if ( draw_force )
    {
    if ( !draw_runs )  
      {
      draw();
      }
    }
  else
    {
    drawMap();
    } 
  }


void DrawView::drawCheck()
  {
  draw_force = TRUE;
  if ( draw_runs )  
    {
    if ( draw_stop == FALSE )
      {
      draw_stop = TRUE;
      }
    }
  else
    {
    update();
    }
  }


void DrawView::draw()
  {
  int x_max, y_max;
  int x, y;
  int *map_pointer;

  draw_runs = TRUE;
  draw_force = FALSE;
  points_drawn = 0;
  x_max = width();
  y_max = height();
  fract->init( center_x, center_y, fract_ratio, fract_ratio,
               x_max, y_max - KFRACT_SIZE_DIFF_TOP, max_iter, bail_out,
               param_1, param_2 );
  map_pointer = my_map;
  for ( y = KFRACT_SIZE_DIFF_TOP; y < y_max; y++ )
    {
    for ( x = 0; x < x_max; x++ )
      {
      if ( draw_stop )
        {
        draw_runs = draw_stop = FALSE;
        update();
        return;
        }
      *map_pointer++ = drawPoint( x, y );
      points_drawn++;
      }
    bitBlt( this, 0, y, drawbuffer, 0, y, 
                  x_max, 1, CopyROP, FALSE );
    qApp->processEvents();
    }
  draw_runs = draw_stop = FALSE;
  }


void DrawView::drawMap()
  {
  bitBlt( this, 0, KFRACT_SIZE_DIFF_TOP, 
          drawbuffer, 0, KFRACT_SIZE_DIFF_TOP, 
          width(), height() - KFRACT_SIZE_DIFF_TOP,
          CopyROP, FALSE);
       
  }


void DrawView::resizeEvent( QResizeEvent * )
  {

  my_width *= ( double ) width() / old_size_x;
  my_height = ( double ) ( my_width * ( height() - KFRACT_SIZE_DIFF_TOP ) ) / 
                                                                   width();
  old_size_x = width();
  old_size_y = height() - KFRACT_SIZE_DIFF_TOP;
  delete my_map;
  my_map = new int[ old_size_x * old_size_y * sizeof( int )];
  CHECK_PTR( my_map );
  delete paint_buf;
  delete drawbuffer;
  drawbuffer = new QPixmap( old_size_x, old_size_y + KFRACT_SIZE_DIFF_TOP );
  CHECK_PTR( drawbuffer );
  drawbuffer->fill( black );
  paint_buf = new QPainter( drawbuffer );
  CHECK_PTR( paint_buf );
  points_drawn = 0;
  drawCheck();
  }


void DrawView::mousePressEvent( QMouseEvent *e )
  {
  int this_button;

  this_button = e->button() & MouseButtonMask;
  if ( my_button == NoButton )
    {
    switch ( this_button )
      {
      case LeftButton:
           point_1 = point_2 = e->pos();
           my_button = my_button | LeftButton;
           break;
      case MidButton:
           magnify( e->pos(), zoom_in );
           break;
      default:
           magnify( e->pos(), zoom_out );
           break;
      }
    }
  }


void DrawView::mouseReleaseEvent( QMouseEvent *e )
  {
  int this_button;
  int my_left, my_right, my_top, my_bottom;

  this_button = e->button() & MouseButtonMask;

  switch ( this_button )
    {
    case LeftButton:
         if ( my_button == LeftButton )
           {
           point_2 = e->pos();
           my_left = QMIN( point_1.x(), point_2.x() );
           my_right = QMAX( point_1.x(), point_2.x() );
           my_top = QMIN( point_1.y(), point_2.y() );
           my_bottom = QMAX( point_1.y(), point_2.y() );
           if ( ( my_top != my_bottom ) && ( my_left != my_right ) )
             {
             center_x = center_x - my_width / 2.0 +
                        fract_ratio * my_left +
                        ( fract_ratio * ( my_right - my_left ) ) / 2.0;
             center_y = center_y - my_height / 2.0 +
                        fract_ratio * ( my_top - KFRACT_SIZE_DIFF_TOP ) +
                        ( fract_ratio * ( my_bottom - my_top ) ) / 2.0;
             my_width = ( my_width * ( my_right - my_left ) ) / width();
             my_height = ( my_width * ( height() - KFRACT_SIZE_DIFF_TOP ) ) / 
                                                                     width();
             fract_ratio = my_width / width();
             drawbuffer->fill( black );
             drawCheck();
             }
           else
             {
             bitBlt( this, my_left, my_top,
                     drawbuffer, my_left, my_top, my_right - my_left + 1,
                                                  my_bottom - my_top + 1,
                     CopyROP, FALSE );
             }
           }
         my_button = NoButton;
         break;
    case MidButton:
         my_button &= ~MidButton;
         break;
    case RightButton:
         my_button &= ~RightButton;
         break;
    default:
         break;
    }
  }


void DrawView::mouseMoveEvent( QMouseEvent *e )
  {
  int x1, x2, y1, y2;
  int current_x, current_y;

  current_x = e->x();
  current_y = e->y();
  if ( ( my_button & LeftButton ) == LeftButton )
    {
    x1 = QMIN( point_1.x(), point_2.x() );
    x2 = QMAX( point_1.x(), point_2.x() );
    y1 = QMIN( point_1.y(), point_2.y() );
    y2 = QMAX( point_1.y(), point_2.y() );
    bitBlt( this, x1, y1, drawbuffer, x1, y1, x2 - x1 + 1, y2 - y1 +1,
            CopyROP, FALSE );
    paint->setPen( white );
    paint->drawLine( point_1.x(), point_1.y(), current_x, point_1.y() );
    paint->drawLine( point_1.x(), current_y, current_x, current_y );
    paint->drawLine( point_1.x(), point_1.y(), point_1.x(), current_y );
    paint->drawLine( current_x, point_1.y(), current_x, current_y);
    point_2.setX( current_x );
    point_2.setY( current_y );
    }
  }


int DrawView::drawPoint( int x, int y )
  {
  int result;
  QColor c;

  result = fract->calcPoint( x, y - KFRACT_SIZE_DIFF_TOP );
  if ( result == max_iter )
    {
    paint_buf->setPen( black );
    }
  else
    {
    switch ( act_color_scheme )
      {
      case Rgb: 
        c.setRgb( result * color_factor );
        break;
      case Hsv:
        c.setHsv( ( result % 72 ) * 5, 255, 255 );
        break;
      default:
        c.setHsv( ( result % 72 ) * 5, 255, 255 );   // just in case ...
        break;
      }
    paint_buf->setPen( c );
    }
  paint_buf->drawPoint( x, y );
  return result;
  }


void DrawView::magnify( QPoint center, double mag_factor )
  {
  center_x += fract_ratio * center.x() - my_width / 2.0;
  center_y += fract_ratio * ( center.y() - KFRACT_SIZE_DIFF_TOP ) - 
                                                  my_height / 2.0;
  my_width *= mag_factor;
// We do it this way in order to get the X/Y ration straight regardless how
// often we zoom in or out.
  my_height = ( my_width * ( height() - KFRACT_SIZE_DIFF_TOP ) ) / width();
  fract_ratio = my_width / width();
  drawbuffer->fill( black );
  drawCheck();
  }



int DrawView::getActualIter()
  {
  int rc;

  if ( last_fract == new_type )
    {
    rc = max_iter;
    }
  else
    {
    if ( new_type == FUnknown )
      {
      rc = max_iter;
      }
    else
      {
      rc = DEFAULT_ITER;
      }
    }
  return rc;
  }



int DrawView::getDefaultIter()
  {
  return DEFAULT_ITER;
  }



double DrawView::getActualBailout()
  {
  double rc;

  if ( last_fract == new_type )
    {
    rc = bail_out;
    }
  else
    {
    rc = test_fract->defaultBailout();
    }
  return rc;
  }



double DrawView::getDefaultBailout()
  {
  return test_fract->defaultBailout();
  }



double DrawView::getActualCenterX()
  {
  double rc;

  if ( last_fract == new_type )
    {
    rc = center_x;
    }
  else
    {
    rc = test_fract->defaultCenterX();
    }
  return rc;
  }


double DrawView::getDefaultCenterX()
  {
  return test_fract->defaultCenterX();
  }


double DrawView::getActualCenterY()
  {
  double rc;

  if (last_fract == new_type )
    {
    rc = center_y;
    }
  else
    {
    rc = test_fract->defaultCenterY();
    }
  return rc;
  }


double DrawView::getDefaultCenterY()
  {
  return test_fract->defaultCenterY();
  }


double DrawView::getActualWidth()
  {
  double rc;

  if ( last_fract == new_type )
    {
    rc = my_width;
    }
  else
    {
    rc = test_fract->defaultWidth();
    }
  return rc;
  }


double DrawView::getDefaultWidth()
  {
  return test_fract->defaultWidth();
  }


double DrawView::getActualExtraX()
  {
  double rc;
  if ( last_fract == new_type )
    {
    rc = param_1;
    }
  else
    {
    rc = test_fract->defaultExtraX();
    }
  return rc;
  }


double DrawView::getDefaultExtraX()
  {
  return test_fract->defaultExtraX();
  }


double DrawView::getActualExtraY()
  {
  double rc;

  if ( last_fract == new_type )
    {
    rc = param_2;
    }
  else
    {
    rc = test_fract->defaultExtraY();
    }
  return rc;
  }


double DrawView::getDefaultExtraY()
  {
  return test_fract->defaultExtraY();
  }


double DrawView::getActualZoomInFactor()
  {
  return zoom_in;
  }


double DrawView::getDefaultZoomInFactor()
  {
  return 0.5;
  }


double DrawView::getActualZoomOutFactor()
  {
  return zoom_out;
  }


double DrawView::getDefaultZoomOutFactor()
  {
  return 2.0;
  }


DrawView::ColorScheme DrawView::getActualColorScheme()
  {
  return act_color_scheme;
  }


void DrawView::setNewType( FractRunning new_fract )
  {
  new_type = new_fract;
  if ( test_fract != NULL )
    {
    delete test_fract;
    }
  switch ( new_fract )
    {
    case FMandel:
      test_fract = new Mandel();
      break;
    case FJulia:
      test_fract = new Julia();
      break;
    default:
      test_fract = new Mandel();     // just in case ...
      break;
    }
  }


void DrawView::setTypeUndo()
  {
  new_type = FUnknown;
  if ( test_fract != NULL )
    {
    delete test_fract;
    test_fract = NULL;
    }
  }


void DrawView::setNewIter( int new_iter )
  {
  if ( max_iter != new_iter )
    {
    max_iter = new_iter;
    color_factor = MAXCOLOR / max_iter;
    drawbuffer->fill( black );
    drawCheck();
    }
  }



void DrawView::setNewBailout( double new_bailout )
  {
  if ( bail_out != new_bailout )
    {
    bail_out = new_bailout;
    drawbuffer->fill( black );
    drawCheck();
    }
  }



void DrawView::setNewCenterX( double new_center_x )
  {
  if ( center_x != new_center_x )
    {
    center_x = new_center_x;
    drawbuffer->fill( black );
    drawCheck();
    }
  }



void DrawView::setNewCenterY( double new_center_y )
  {
  if (center_y != new_center_y )
    {
    center_y = new_center_y;
    drawbuffer->fill( black );
    drawCheck();
    }
  }



void DrawView::setNewWidth( double new_width )
  {
  if ( my_width != new_width )
    {
    my_width = new_width;
// We do it this way in order to get the X/Y ration straight regardless how
// often we zoom in or out.
    my_height = ( my_width * ( height() - KFRACT_SIZE_DIFF_TOP ) ) / width();
    fract_ratio = my_width / width();
    drawbuffer->fill( black );
    drawCheck();
    }
  }



void DrawView::setNewExtraX( double new_extra_x )
  {
  if ( param_1 != new_extra_x )
    {
    param_1 = new_extra_x;
    drawbuffer->fill( black );
    drawCheck();
    }
  }



void DrawView::setNewExtraY( double new_extra_y )
  {
  if ( param_2 != new_extra_y )
    {
    param_2 = new_extra_y;
    drawbuffer->fill( black );
    drawCheck();
    }
  }


void DrawView::setNewZoomInFactor( double new_zoom_in_factor )
  {
  zoom_in = new_zoom_in_factor;
  }


void DrawView::setNewZoomOutFactor( double new_zoom_out_factor )
  {
  zoom_out = new_zoom_out_factor;
  }


void DrawView::setNewColorScheme( ColorScheme scheme )
  {
  if ( act_color_scheme != scheme )
    {
    act_color_scheme = scheme;
    drawbuffer->fill( black );
    drawCheck();
    }
  }


void DrawView::stop()
  {
  draw_stop = TRUE;
  }


void DrawView::saveParam()
  {
  QString filename;

  filename = QFileDialog::getSaveFileName( 0, "*.fct", this );
  if ( filename.isNull() )
    {
    return;
    }
  QFile file( filename );
  if ( file.open ( IO_WriteOnly | IO_Truncate ) )
    {
    QTextStream t( &file );
    t.precision( 50 );
    t << "KFract-0.1.1\n";
    switch( last_fract )
      {
      case FMandel:
        t << "mandelbrot\n";
        break;
      case FJulia:
        t << "julia\n";
        break;
      default:
        t << "bullshit\n";
        break;        // How dare you!
      }
    t << center_x  << "\n";
    t << center_y  << "\n";
    t << my_width  << "\n";
    t << my_height  << "\n";
    t << bail_out << "\n";
    t << max_iter  << "\n";
    t << param_1 << "\n";
    t << param_2 << "\n";
    file.close();
    }
  }


void DrawView::savePic()
  {
  QString filename;
  QPixmap *buffer;

  filename = QFileDialog::getSaveFileName( 0, "*.bmp", this );
  if ( !filename.isNull() )
    {
    buffer = new QPixmap( old_size_x, old_size_y );
    CHECK_PTR( buffer );
    bitBlt( buffer, 0, 0, 
            drawbuffer, 0, KFRACT_SIZE_DIFF_TOP,
            old_size_x, old_size_y,
            CopyROP, FALSE );
    buffer->save( filename, "BMP" );
    delete buffer;
    }
  }


void DrawView::loadParam( char *name )
  {
  QString filename;
  QString line;

  if ( name == NULL )
    {
    filename = QFileDialog::getOpenFileName ( 0, "*.fct", this );
    if ( filename.isNull() )
      {
      return;
      }
    }
  else
    {
    filename = name;
    }
  QFile file( filename );
  if ( file.open( IO_ReadOnly ) )
    {
    QTextStream t( &file );
    t >> line;
    if ( line == "KFract-0.1.1" )
      {
      delete fract;
      line.resize( 0 );
      t >> line;
      if ( line == "mandelbrot" )
        {
        last_fract = FMandel;
        fract = new Mandel();
        }
      else
        {
        if ( line == "julia" )
          {
          last_fract = FJulia;
          fract = new Julia();
          }
        else
          {
          exit ( 1 );     // Oops!
          }
        }
      t >> center_x;
      t >> center_y;
      t >> my_width;
      t >> my_height;
      t >> bail_out;
      t >> max_iter;
      t >> param_1;
      t >> param_2;
      file.close();
      my_height = ( my_width * height() ) / width();
      fract_ratio = my_width / width();
      drawbuffer->fill( black );
      points_drawn = 0;
      draw_force = TRUE;
      update();
      }
    }
  }



