
    /*

    kfract.C  version 0.1.2

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

#include <iostream.h>
#include <qkeycode.h>
#include <kmsgbox.h>

#include "kfract.moc"
#include "version.h"



KFract::KFract( int &argc, char **argv, const QString& rAppName) :
  KApplication( argc, argv, rAppName )
  {
  center = width = height = 0.0;
  w = new KTopLevelWidget( "kfract" );
  CHECK_PTR( w );
  setMainWidget( w );

  v = new DrawView( w );
  CHECK_PTR( v );

  m = new KMenuBar( w );
  CHECK_PTR( m );
  setupMenuBar();

  t = new KToolBar( w );
  CHECK_PTR( t );
  setupToolBar();
  w->addToolBar( t );

  w->setMenu( m );
  w->setView( v );
  w->show();
  w->resize( KFRACT_INITIAL_SIZE_X + KFRACT_SIZE_DIFF_X, 
             KFRACT_INITIAL_SIZE_Y + KFRACT_SIZE_DIFF_Y );
  };


KFract::~KFract()
  {
  if ( t != NULL ) delete t;
  if ( m != NULL ) delete m;
  }


void KFract::setupMenuBar()
  {
  QPopupMenu *file = new QPopupMenu();
  CHECK_PTR( file );
  file->insertItem( i18n("&Load params..."), this, SLOT( loadParam() ) );
  file->insertItem( i18n("&Save params..."), this, SLOT( saveParam() ) );
  file->insertItem( i18n("Save &pic..."), this, SLOT( savePic() ) );
  file->insertSeparator();
  file->insertItem( i18n("&Quit"), this, SLOT( my_quit() ), ALT+Key_Q );

  QPopupMenu *types = new QPopupMenu();
  CHECK_PTR( types );
  types->insertItem( i18n("Mandelbrot..."), this, SIGNAL( mandel() ) );
  types->insertItem( i18n("Julia..."), this, SIGNAL( julia() ) );

  QPopupMenu *options = new QPopupMenu();
  CHECK_PTR( options );
  options->insertItem( i18n("Max. Iterations..."), this, SIGNAL( iterations() ) );
  options->insertItem( i18n("Colour scheme..."), this, SIGNAL( colorScheme() ) );
  options->insertItem( i18n("Colour cycling..."), this, SIGNAL( colorCycling() ) );
  options->insertItem( i18n("Zoom factor..."), this, SIGNAL( zoomFactor() ) );

  QPopupMenu *help = kapp->getHelpMenu(true, QString(i18n("Fractals Generator"))
                                         + " " + KFRACT_VERSION
                                         + i18n("\n\nby Uwe Thiem")
                                         + " (uwe@uwix.alt.na)");  
  CHECK_PTR( help );


  m->insertItem( i18n("&File"), file );
  m->insertItem( i18n("&Types"), types);
  m->insertItem( i18n("&Options"), options );
  m->insertSeparator();
  m->insertItem( i18n("&Help"), help );

  connect( this, SIGNAL( mandel() ), this, SLOT( mandelFract() ) );
  connect( this, SIGNAL( julia() ), this, SLOT( juliaFract() ) );
  connect( this, SIGNAL( iterations() ), this, SLOT( iterFract() ) );
  connect( this, SIGNAL( colorScheme() ), this, SLOT( colorSchemeFract() ) );
  connect( this, SIGNAL( colorCycling() ), this, SLOT( notImplemented() ) );
  connect( this, SIGNAL( zoomFactor() ), this, SLOT( zoomFract() ) );
//  connect( this, SIGNAL( about() ), this, SLOT( aboutFract() ) );
  connect( this, SIGNAL( help() ), this, SLOT( helpFract() ) );
  }


void KFract::setupToolBar()
  {
  QPixmap pixmap;

  QString pixdir = kapp->kde_toolbardir().copy() + "/";
  pixmap.load( pixdir + "fileopen.xpm" );
  t->insertButton( pixmap, 0,
                   SIGNAL( clicked() ), this,
                   SLOT( loadParam() ), TRUE, i18n("Open parameters") );
  pixmap.load( pixdir + "filefloppy.xpm" );
  t->insertButton( pixmap, 0,
                   SIGNAL( clicked() ), this,
                   SLOT( saveParam() ), TRUE, i18n("Save parameters") );
  pixmap.load( pixdir + "filefloppy.xpm" );
  t->insertButton( pixmap, 0,
                   SIGNAL( clicked() ), this,
                   SLOT( savePic() ), TRUE, i18n("Save picture") );
  t->insertSeparator();
  pixmap.load( pixdir + "viewmag+.xpm" );
  t->insertButton( pixmap, 0,
                   SIGNAL( clicked() ), this,
                   SLOT( doZoomIn() ), TRUE, i18n("Zoom in") );
  pixmap.load( pixdir + "viewmag-.xpm" );
  t->insertButton( pixmap, 0,
                   SIGNAL( clicked() ), this,
                   SLOT( doZoomOut() ), TRUE, i18n("Zoom out") );
  t->insertSeparator();
  pixmap.load( pixdir + "home.xpm" );
  t->insertButton( pixmap, 0,
                   SIGNAL( clicked() ), this,
                   SLOT( goHome() ), TRUE, i18n("Defaults for this type") );
  t->insertSeparator();
  pixmap.load( pixdir + "help.xpm" );
  t->insertButton( pixmap, 0,
                   SIGNAL( clicked() ), this,
                   SLOT( helpFract() ), TRUE, i18n("Help") );

  t->setBarPos( KToolBar::Top );
  }


void KFract::mandelFract()
  {
  v->setNewType( DrawView::FMandel );
  MandelDlg dlg( v->getActualIter(),
                 v->getDefaultIter(),
                 v->getActualBailout(),
                 v->getDefaultBailout(),
                 v->getActualCenterX(),
                 v->getDefaultCenterX(),
                 v->getActualCenterY(),
                 v->getDefaultCenterY(),
                 v->getActualWidth(),
                 v->getDefaultWidth(),
                 w );
  connect( &dlg, SIGNAL( iterChanged( int ) ),
           this, SLOT( getIter( int ) ) );
  connect( &dlg, SIGNAL( bailoutChanged( double ) ),
           this, SLOT( getBailout( double ) ) );
  connect( &dlg, SIGNAL( centerXChanged( double ) ),
           this, SLOT( getCenterX( double ) ) );
  connect( &dlg, SIGNAL( centerYChanged( double ) ),
           this, SLOT( getCenterY( double ) ) );
  connect( &dlg, SIGNAL( widthChanged( double ) ),
           this, SLOT( getWidth( double ) ) );
  connect( &dlg, SIGNAL( changedToMandel() ),
           this, SLOT( changedToMandel() ) );
  dlg.exec();
  v->setTypeUndo();
  }


void KFract::juliaFract()
  {
  v->setNewType( DrawView::FJulia );
  JuliaDlg dlg( v->getActualIter(),
                v->getDefaultIter(),
                v->getActualBailout(),
                v->getDefaultBailout(),
                v->getActualCenterX(),
                v->getDefaultCenterX(),
                v->getActualCenterY(),
                v->getDefaultCenterY(),
                v->getActualWidth(),
                v->getDefaultWidth(),
                v->getActualExtraX(),
                v->getDefaultExtraX(),
                v->getActualExtraY(),
                v->getDefaultExtraY(),
                w );
  connect( &dlg, SIGNAL( iterChanged( int ) ),
           this, SLOT( getIter( int ) ) );
  connect( &dlg, SIGNAL( bailoutChanged( double ) ),
           this, SLOT( getBailout( double ) ) );
  connect( &dlg, SIGNAL( centerXChanged( double ) ),
           this, SLOT( getCenterX( double ) ) );
  connect( &dlg, SIGNAL( centerYChanged( double ) ),
           this, SLOT( getCenterY( double ) ) );
  connect( &dlg, SIGNAL( widthChanged( double ) ),
           this, SLOT( getWidth( double ) ) );
  connect( &dlg, SIGNAL( extraXChanged( double ) ),
           this, SLOT( getExtraX( double ) ) );
  connect( &dlg, SIGNAL( extraYChanged( double ) ),
           this, SLOT( getExtraY( double ) ) );
  connect( &dlg, SIGNAL( changedToJulia() ),
           this, SLOT( changedToJulia() ) );
  dlg.exec();
  v->setTypeUndo();
  }



void KFract::iterFract()
  {
  IterDlg dlg( v->getActualIter(),
               v->getDefaultIter(), w );
  connect( &dlg, SIGNAL( iterChanged( int ) ),
           this, SLOT( getIter( int ) ) );
  dlg.exec();
  }


void KFract::zoomFract()
  {
  ZoomDlg dlg( v->getActualZoomInFactor(),
               v->getDefaultZoomInFactor(),
               v->getActualZoomOutFactor(),
               v->getDefaultZoomOutFactor(), w );
  connect( &dlg, SIGNAL( zoomInChanged( double ) ),
           this, SLOT( getZoomIn( double ) ) );
  connect( &dlg, SIGNAL( zoomOutChanged( double ) ),
           this, SLOT( getZoomOut( double ) ) );
  dlg.exec();
  }


void KFract::getZoomIn( double zoom_in )
  {
  v->setNewZoomInFactor( zoom_in );
  }


void KFract::getZoomOut( double zoom_out )
  {
  v->setNewZoomOutFactor( zoom_out );
  }


void KFract::colorSchemeFract()
  {
  ColorDlg dlg( v->getActualColorScheme(), w );
  connect( &dlg, SIGNAL( colorChanged( DrawView::ColorScheme ) ),
           this, SLOT( getColorScheme( DrawView::ColorScheme ) ) );
  dlg.exec();
  }


void KFract::getColorScheme( DrawView::ColorScheme scheme )
  {
  v->setNewColorScheme( scheme );
  }


void KFract::notImplemented()
  {
  KMsgBox::message( w, i18n("Oops!"), i18n("Oops! Not implemented yet."),
                    KMsgBox::INFORMATION, i18n("OK") );
  }

/*
void KFract::aboutFract()
  {
  char tmp[1000];

  sprintf( tmp, "KFract %s\n"
                "Generator for fractals\n"
                "by Uwe Thiem\n"
                "uwe@uwix.alt.na", KFRACT_VERSION );
  KMsgBox::message( w, "About KFract", tmp,
                    KMsgBox::INFORMATION, "OK" );
  }
*/

void KFract::helpFract()
  {
  kapp->invokeHTMLHelp( "", 0 );
  }


void KFract::getIter( int new_iter )
  {
  v->setNewIter( new_iter );
  }


void KFract::getBailout( double new_bailout )
  {
  v->setNewBailout( new_bailout );
  }


void KFract::getCenterX( double new_center_x )
  {
  v->setNewCenterX( new_center_x );
  }


void KFract::getCenterY( double new_center_y )
  {
  v->setNewCenterY( new_center_y );
  }


void KFract::getWidth( double new_width )
  {
  v->setNewWidth( new_width );
  }


void KFract::getExtraX( double new_extra_x )
  {
  v->setNewExtraX( new_extra_x );
  }


void KFract::getExtraY( double new_extra_y )
  {
  v->setNewExtraY( new_extra_y );
  }


void KFract::my_quit()
  {
  v->stop();
  kapp->quit();
  }


void KFract::changedToMandel()
  {
  v->changeToMandel();
  }


void KFract::changedToJulia()
  {
  v->changeToJulia();
  }                 


void KFract::savePic()
  {
  v->savePic();
  }


void KFract::saveParam()
  {
  v->saveParam();
  }


void KFract::loadParam()
  {
  v->loadParam();
  }


void KFract::doZoomIn()
  {
  v->doZoomIn();
  }


void KFract::doZoomOut()
  {
  v->doZoomOut();
  }


void KFract::goHome()
  {
  v->goDefaults();
  }


