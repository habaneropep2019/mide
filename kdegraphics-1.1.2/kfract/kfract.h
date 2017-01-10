
    /*

    kfract.h  version 0.1.2

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


#ifndef KFRACT_H
# define KFRACT_H

//#define KFRACT_VERSION "1.2"


#include <qpainter.h>
#include <kapp.h>
#include <ktopwidget.h>

#include "drawview.h"
#include "kfractdlgs.h"
#include "version.h"

class KFract : public KApplication
  {
    Q_OBJECT
  public:
    KFract( int &argc, char **argv, const QString& rAppName );
    ~KFract();
  signals:
    void mandel();
    void julia();
    void iterations();
    void colorScheme();
    void colorCycling();
    void zoomFactor();
//    void about();
    void help();
  protected:
  private:
    double center, width, height;
    KTopLevelWidget *w;
    KMenuBar *m;
    KToolBar *t;
    DrawView *v;
    void setupMenuBar();
    void setupToolBar();
  private slots:
    void loadParam();
    void saveParam();
    void savePic();
//    void aboutFract();
    void helpFract();
    void mandelFract();
    void juliaFract();
    void iterFract();
    void zoomFract();
    void my_quit();
    void getIter( int new_iter );
    void getBailout ( double new_bailout );
    void getCenterX( double new_center_x );
    void getCenterY( double new_center_y );
    void getWidth( double new_width );
    void getExtraX( double extra_x );
    void getExtraY( double extra_y );
    void getZoomIn( double zoom_in );
    void getZoomOut( double zoom_out );
    void getColorScheme( DrawView::ColorScheme new_scheme );
    void changedToMandel();
    void changedToJulia();
    void doZoomIn();
    void doZoomOut();
    void goHome();
    void notImplemented();
    void colorSchemeFract();
  };




#endif   // KFRACT_H
