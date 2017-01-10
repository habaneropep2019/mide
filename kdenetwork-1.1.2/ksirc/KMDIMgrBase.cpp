/***************************************************************************
             KMDIMgrBase.cpp  -  Base MDI classes for the KDE project                              
                             -------------------                                         

    version              :                                   
    begin                : Sat Oct 17 16:05:25 CDT 1998
                                           
    copyright            : (C) 1998 by Timothy Whitfield and Glover George                         
    email                : timothy@ametro.net and dime@ametro.net                                     
 ***************************************************************************/
/***************************************************************************
 Originally based on Eko Bono Suprijadi's keirc MDI classes.
 ***************************************************************************/
/***************************************************************************
 Keirc MDI classes are based from mdi.h , mdi.cpp by Aaron Granickko.
 ***************************************************************************/
/***************************************************************************
 Code used from the KDE projects kwm window manager.
 ***************************************************************************/

#include "KMDIMgrBase.h"

#include <qlayout.h>
#include <qdrawutil.h>
#include <kiconloader.h>

#include <iostream.h>
#include <qpainter.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <unistd.h>

// Don't like making the class kwm dependant.
//#include <kwm.h>
#include <kbutton.h>

#define RESIZE_TIMES    10
#define SLEEP_TIME     100
#define FANCY_RECT       0

#define pasivColor        gray
#define activColor        darkBlue

#define TITLE_ANIMATION_STEP 2

const int UPPER_LEFT  = 1;
const int LOWER_LEFT  = 2;
const int UPPER_RIGHT = 3;
const int LOWER_RIGHT = 4;

const int DEFAULT_WINDOW_WIDTH  = 450; 
const int DEFAULT_WINDOW_HEIGHT = 300;
const int DEFAULT_WINDOW_POSX   = 0;
const int DEFAULT_WINDOW_POSY   = 0;

const int RESIZE_BORDER         = 10;
const int MINIMUM_X             = 100;
const int MINIMUM_Y             = 100;
const int MIN_TITLE_HEIGHT      = 18;
const int TITLE_BUTTON_WIDTH    = 20;

const int MIN_EDGE              = 40;  // minimum amount of window showing.
const int STARTING_OFFSET       = 30;  // how much further away from the previous window

#define ICON_WIDTH       20
#define BORDER            4

QString KMDIMgrBase::minimizePic;
QString KMDIMgrBase::maximizePic;
QString KMDIMgrBase::closePic;

// I really hate to do things like this!
// but I stole this too, so it must be right =P
// this function grapped from kcontrol/titlebar.cpp
// it was partly grapped from kwm/client.C
QPixmap loadIcon(const char* name)
{
  QPixmap result;
  QString fn = "/share/apps/kwm/pics/";
  fn.append(name);
  QString s = KApplication::findFile(fn);
  if (!s.isEmpty())
    result.load(s.data());
  return result;
} 

// Caption
KMDITitleLabel::KMDITitleLabel(QWidget* p, const char* name) :
  QFrame(p, name)
{
//  setAlignment(AlignVCenter|AlignLeft);
//  setMargin   (BORDER);

  QPalette* pal=QApplication::palette();
  QColorGroup  active   = pal->active();
  QColorGroup  disabled = pal->disabled();
  QColorGroup  normal(white, blue,active.light(),
		      active.dark(), active.mid(), white,
		      active.base());
  QPalette npal(normal, disabled, active);
  setPalette(npal);

  options.titlebarPixmapActive = new("QPixmap") QPixmap;
  options.titlebarPixmapInactive = new("QPixmap") QPixmap;
  readConfiguration();

  titlestring_offset=0;
  titlestring_offset_delta=TITLE_ANIMATION_STEP;
}

KMDITitleLabel::~KMDITitleLabel()
{
  delete options.titlebarPixmapActive;
  delete options.titlebarPixmapInactive;
}

void KMDITitleLabel::setText (const char *txt)
{
    label=txt;
}

const char *KMDITitleLabel::getText ()
{
    return label;
}

void KMDITitleLabel::mousePressEvent ( QMouseEvent *e)
{
    if (e->button() == RightButton)
        emit popupMenu(e->pos());
    else
        emit moving();
}

void KMDITitleLabel::paintEvent( QPaintEvent *e )
{
    paintState();
}

// The following two functions were stolen from kwm. =-)
// And then I messed'em all up.

void KMDITitleLabel::readConfiguration(){
  KConfig* config;
  QString key;
//  int i;

  killTimers();

  config = new("KConfig") KConfig(KApplication::kde_configdir() + "/kwmrc",
                       KApplication::localconfigdir() + "/kwmrc");

  // this belongs in kapp....
  config->setGroup("WM");
  activeTitleBlend = config->readColorEntry( 
    "activeBlend",&black);
  inactiveTitleBlend = config->readColorEntry(
    "inactiveBlend",&kapp->backgroundColor);

  config->setGroup( "General" );

  //BORDER = 4;

  key = config->readEntry("WindowMoveType");
  if( key == "Transparent")
    options.WindowMoveType = TRANSPARENT;
  else if( key == "Opaque")
    options.WindowMoveType = OPAQUE;
  else{
    config->writeEntry("WindowMoveType","Transparent");
    options.WindowMoveType = TRANSPARENT;
  }

  key = config->readEntry("WindowResizeType");
  if( key == "Transparent")
    options.WindowResizeType = TRANSPARENT;
  else if( key == "Opaque")
    options.WindowResizeType = OPAQUE;
  else{
    config->writeEntry("WindowResizeType","Transparent");
    options.WindowResizeType = TRANSPARENT;
  }

  key = config->readEntry("FocusPolicy");
  if( key == "ClickToFocus")
    options.FocusPolicy = CLICK_TO_FOCUS;
  else if( key == "FocusFollowsMouse" || key == "FocusFollowMouse")
    options.FocusPolicy = FOCUS_FOLLOWS_MOUSE;
  else if( key == "ClassicFocusFollowsMouse" || key == "ClassicFocusFollowMouse")
    options.FocusPolicy = CLASSIC_FOCUS_FOLLOWS_MOUSE;
  else if( key == "ClassicSloppyFocus")
    options.FocusPolicy = CLASSIC_SLOPPY_FOCUS;
  else{
    config->writeEntry("FocusPolicy","ClickToFocus");
    options.FocusPolicy = CLICK_TO_FOCUS;
  }

  key = config->readEntry("AltTabMode");
  if( key == "KDE")
    options.AltTabMode = KDE_STYLE;
  else if( key == "CDE")
    options.AltTabMode = CDE_STYLE;
  else{
    config->writeEntry("AltTabMode","KDE");
    options.AltTabMode = KDE_STYLE;
  }

  if (CLASSIC_FOCUS)
    options.AltTabMode = CDE_STYLE;

  key = config->readEntry("TitlebarLook");
  if( key == "shadedHorizontal")
    options.TitlebarLook = H_SHADED;
  else if( key == "shadedVertical")
    options.TitlebarLook = V_SHADED;
  else if( key == "plain")
    options.TitlebarLook = PLAIN;
  else if( key == "pixmap")
    options.TitlebarLook = PIXMAP;
  else{
    config->writeEntry("TitlebarLook", "shadedHorizontal");
    options.TitlebarLook = H_SHADED;
  }

  //CT 23Sep1998 - fixed the name of the titlebar pixmaps to become
  //   consistent with the buttons pixmaps definition technique

  if (options.TitlebarLook == PIXMAP) {
    *options.titlebarPixmapActive = loadIcon("activetitlebar.xpm");
    *options.titlebarPixmapInactive = loadIcon("inactivetitlebar.xpm");

    if (options.titlebarPixmapInactive->size() == QSize(0,0))
      *options.titlebarPixmapInactive = *options.titlebarPixmapActive;

    if (options.titlebarPixmapActive->size() == QSize(0,0))
      options.TitlebarLook = PLAIN;
  }

  //CT 12Jun1998 - variable animation speed from 0 (none!!) to 10 (max)
  if (config->hasKey("ResizeAnimation")) {
    options.ResizeAnimation = config->readNumEntry("ResizeAnimation");
    if( options.ResizeAnimation < 1 ) options.ResizeAnimation = 0;
    if( options.ResizeAnimation > 10 ) options.ResizeAnimation = 10;
  }
  else{
    options.ResizeAnimation = 1;
    config->writeEntry("ResizeAnimation", options.ResizeAnimation);
  }

  config->sync();

  delete config;
}

// Repaint the state of a window. Active or inactive windows differ
// only in the look of the titlebar. If only_label is true then only
// the label string is repainted. This is used for the titlebar
// animation with animate = TRUE.
void KMDITitleLabel::paintState(bool only_label, bool colors_have_changed,
  bool animate){
  KMDIWindow *w;
  QRect title_rect=QRect(0,0,width(),height());
  QRect r = title_rect;
  bool double_buffering = false;
  QPixmap* buffer = 0;
  int x;

//  Nice, but this would make the class KWM dependant.
//  if (r.width() <= 0 || r.height() <= 0
//      || getDecoration()!=KWM::normalDecoration)
//    return;
//  int x;

  w=(KMDIWindow *)(parent()->parent()->parent());
  is_active=w->IsSelected();

  TITLEBAR_LOOK look = options.TitlebarLook;

  if (look == H_SHADED || look == V_SHADED){
    // the new("horizontal") horizontal (and vertical) shading code
    if (colors_have_changed){
      aShadepm.resize(0,0);
      iaShadepm.resize(0,0);
    }

    // the user selected shading. Do a plain titlebar anyway if the
    // shading would be senseless (higher performance and less memory
    // consumption)
    if (is_active){
      if ( kapp->activeTitleColor
          ==  activeTitleBlend)
	look = PLAIN;
    }
    else {
      if ( kapp->inactiveTitleColor
         ==  inactiveTitleBlend)
	look = PLAIN;
    }
  }

  // the following is the old/handmade vertical shading code

//   if (options.TitlebarLook == SHADED){
//     if (is_active && shaded_pm_active && shaded_pm_active_color != myapp->activeTitleColor){
//       delete shaded_pm_active;
//       shaded_pm_active = 0;
//     }
//     if (is_active && shaded_pm_inactive && shaded_pm_inactive_color != myapp->inactiveTitleColor){
//       delete shaded_pm_inactive;
//       shaded_pm_inactive = 0;
//     }
//   }

//   if (options.TitlebarLook == SHADED
//       &&
//       (is_active && (!shaded_pm_active)
//        || (!is_active && !shaded_pm_inactive))
//       ){
//     int i,y;
//     QImage image (50, TITLEBAR_HEIGHT, 8, TITLEBAR_HEIGHT);
//     QColor col = is_active ? myapp->activeTitleColor : myapp->inactiveTitleColor;
//     for ( i=0; i<TITLEBAR_HEIGHT; i++ ){
//       image.setColor( i, col.light(100+(i-TITLEBAR_HEIGHT/2)*3).rgb());
//     }
//     for (y=0; y<TITLEBAR_HEIGHT; y++ ) {
//       uchar *pix = image.scanLine(y);
//       for (x=0; x<50; x++)
//         *pix++ = is_active ? TITLEBAR_HEIGHT-1-y : y;
//     }
//     if (is_active){
//       shaded_pm_active = new("QPixmap") QPixmap;
//       *shaded_pm_active = image;
//       shaded_pm_active_color = myapp->activeTitleColor;
//     }
//     else{
//       shaded_pm_inactive = new("QPixmap") QPixmap;
//       *shaded_pm_inactive = image;
//       shaded_pm_inactive_color = myapp->inactiveTitleColor;
//     }
//   }

  QPainter p;

  if (only_label && animate){
    double_buffering = (look == H_SHADED || look == V_SHADED || look == PIXMAP);
    titlestring_offset += titlestring_offset_delta;
    if (!double_buffering){
      if (titlestring_offset_delta > 0)
	bitBlt(this,
	       r.x()+titlestring_offset_delta, r.y(),
	       this,
	       r.x(), r.y(),
	       r.width()-titlestring_offset_delta, r.height());
      else
	bitBlt(this,
	       r.x(), r.y(),
	       this,
	       r.x()-titlestring_offset_delta, r.y(),
	       r.width()+titlestring_offset_delta, r.height());
    }
  }

  if (!double_buffering)
    p.begin( this );
  else {
    // enable double buffering to avoid flickering with horizontal shading
    buffer = new("QPixmap") QPixmap(r.width(), r.height());
    p.begin(buffer);
    r.setRect(0,0,r.width(),r.height());
  }

  QPixmap *pm;
  p.setClipRect(r);
  p.setClipping(True);

  // old handmade vertical shading code

//   if (options.TitlebarLook == SHADED || options.TitlebarLook == PIXMAP){

//     if (options.TitlebarLook == SHADED)
//       pm = is_active ? shaded_pm_active : shaded_pm_inactive;
//     else

  if (look == PIXMAP){
      pm = is_active ? options.titlebarPixmapActive: options.titlebarPixmapInactive;
      for (x = r.x(); x < r.x() + r.width(); x+=pm->width())
	p.drawPixmap(x, r.y(), *pm);
  }
  else if (look == H_SHADED || look == V_SHADED ){
    // the new("horizontal") horizontal shading code
    QPixmap* pm = 0;
    if (is_active){
      if (aShadepm.size() != r.size()){
	aShadepm.resize(r.width(), r.height());
	kwm_gradientFill( aShadepm, 
          kapp->activeTitleColor,
          activeTitleBlend, look == V_SHADED );
	//aShadepm.gradientFill( activeTitleColor, activeTitleBlend, look == V_SHADED );
      }
      pm = &aShadepm;
    }
    else {
      if (iaShadepm.size() != r.size()){
	iaShadepm.resize(r.width(), r.height());
	kwm_gradientFill( iaShadepm,
          kapp->inactiveTitleColor,
          inactiveTitleBlend, look == V_SHADED ); 
//	iaShadepm.gradientFill(inactiveTitleColor, inactiveTitleBlend,
//        look == V_SHADED );
       }
       pm = &iaShadepm;
    }

    p.drawPixmap( r.x(), r.y(), *pm );
  }
  else { // TitlebarLook == TITLEBAR_PLAIN
    p.setBackgroundColor( is_active ? 
      kapp->activeTitleColor : 
      kapp->inactiveTitleColor);
    if (only_label && !double_buffering && animate){
       p.eraseRect(QRect(r.x(), r.y(), TITLE_ANIMATION_STEP+1, r.height()));
       p.eraseRect(QRect(r.x()+r.width()-TITLE_ANIMATION_STEP-1, r.y(),
 			TITLE_ANIMATION_STEP+1, r.height()));
    }
    else {
      p.eraseRect(r);
    }
  }
  p.setClipping(False);

  if (is_active)
    qDrawShadePanel( &p, r, colorGroup(), true );

  p.setPen(is_active ? 
    kapp->activeTextColor : 
    kapp->inactiveTextColor);

  QFont fnt = kapp->generalFont;
  fnt.setPointSize(12);
  fnt.setBold(true);
  p.setFont(fnt);
  bool titlestring_too_large = 
    (p.fontMetrics().width(QString(" ")+label+" ")>r.width());
  if (titlestring_offset_delta > 0){
    if (titlestring_offset > 0
	&& titlestring_offset > r.width() - p.fontMetrics().width(QString(" ")+label+" ")){
      titlestring_offset_delta *= -1;
    }
  }
  else {
    if (titlestring_offset < 0
	&& titlestring_offset +
	p.fontMetrics().width(QString(" ")+label+" ") < r.width()){
      titlestring_offset_delta *= -1;
    }
  }

  if (!titlestring_too_large)
    titlestring_offset = 0;
  p.setClipRect(r);
  p.setClipping(True);
  p.drawText(r.x()+(options.TitleAnimation?titlestring_offset:0),
	     r.y()+p.fontMetrics().ascent(),
	     QString(" ")+label+" ");
  p.setClipping(False);
  p.end();
  if (double_buffering){
    bitBlt(this,
	   title_rect.x(), title_rect.y(),
	   buffer,
	   0,0,
	   buffer->width(), buffer->height());
    delete buffer;
  }
}

void KMDITitleLabel::kwm_gradientFill(KPixmap &pm, QColor ca, QColor cb,
       bool upDown) {
   if(upDown == FALSE && QColor::numBitPlanes() >= 15) { 
    int w = pm.width();
    int h = pm.height();
    
    int c_red_a = ca.red() << 16;
    int c_green_a = ca.green() << 16;
    int c_blue_a = ca.blue() << 16;

    int c_red_b = cb.red() << 16;
    int c_green_b = cb.green() << 16;
    int c_blue_b = cb.blue() << 16;
    
    int d_red = (c_red_b - c_red_a) / w;
    int d_green = (c_green_b - c_green_a) / w;
    int d_blue = (c_blue_b - c_blue_a) / w;

    QImage img(w, h, 32);
    uchar *p = img.scanLine(0);

    int r = c_red_a, g = c_green_a, b = c_blue_a;
    for(int x = 0; x < w; x++) {
      *p++ = r >> 16;
      *p++ = g >> 16;
      *p++ = b >> 16;
      p++;
      
      r += d_red;
      g += d_green;
      b += d_blue;
    }

    uchar *src = img.scanLine(0);
    for(int y = 1; y < h; y++)
      memcpy(img.scanLine(y), src, 4*w);

    pm.convertFromImage(img);
  } else
    pm.gradientFill(ca, cb, upDown);
}

// KMDITitle
KMDITitle::KMDITitle(const char* icon, QWidget* p, const char* name)
                  :  QWidget(p, name) 
{
    KConfig* config;

    config = new("KConfig") KConfig(KApplication::kde_configdir() + "/kwmrc",
                         KApplication::localconfigdir() + "/kwmrc");

    QHBoxLayout* hLayout = new("QHBoxLayout") QHBoxLayout(this, 0);

    QPixmap pixmap;
//    hLayout->addSpacing(BORDER);

    if(icon)
    {
      pixmap=kapp->getIconLoader()->loadIcon(icon);
    
      if (pixmap.size()==QSize(0,0)){
        pixmap=kapp->getIcon();
        if(pixmap.size()==QSize(0,0))
          pixmap=loadIcon("menu.xpm");
      }
    }
    else
    {
      // Maybe I am over paranoid or this stuff should be put in a function, but
      // whatever the case this was quick and it worked.
      pixmap=kapp->getIcon();
      if(pixmap.size()==QSize(0,0))
        pixmap=loadIcon("menu.xpm");
    }

    iconBtn = new("KButton") KButton(this);
    iconBtn->setPixmap(pixmap);
    iconBtn->setMouseTracking(true);
    hLayout->addWidget(iconBtn);
    iconBtn->setFixedSize(ICON_WIDTH, ICON_WIDTH);

    hLayout->addSpacing(BORDER);

    caption = new("KMDITitleLabel") KMDITitleLabel(this);
    caption->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    caption->setMouseTracking(true);
    hLayout->addWidget(caption, 10);

    hLayout->addSpacing(BORDER);

    if (!KMDIMgrBase::minimizePic.isEmpty()){
        pixmap=loadIcon( KMDIMgrBase::minimizePic );
	minBtn = new("KButton") KButton(this);
	minBtn->setPixmap(pixmap);
	minBtn->setMouseTracking(true);
	hLayout->addWidget(minBtn);
	minBtn->setFixedSize(ICON_WIDTH, ICON_WIDTH);
    }
    if (!KMDIMgrBase::maximizePic.isEmpty()){
        pixmap=loadIcon( KMDIMgrBase::maximizePic );
	maxBtn = new("KButton") KButton(this);
	maxBtn->setPixmap(pixmap);
	maxBtn->setMouseTracking(true);
	hLayout->addWidget(maxBtn);
	maxBtn->setFixedSize(ICON_WIDTH, ICON_WIDTH);
    }
    if (!KMDIMgrBase::closePic.isEmpty()){
        pixmap=loadIcon(KMDIMgrBase::closePic);
	closeBtn = new("KButton") KButton(this);
	closeBtn->setPixmap(pixmap);
	closeBtn->setMouseTracking(true);
	hLayout->addWidget(closeBtn);
	closeBtn->setFixedSize(ICON_WIDTH, ICON_WIDTH);
    }
    QPalette palette = topLevelWidget()->palette();
    setPalette(palette);
//    hLayout->addSpacing(BORDER);
    hLayout->activate();

    show();

    delete config;
}

void KMDITitle::setEnabled(bool f)
{
  iconBtn->setEnabled(f);
  caption->setEnabled(f);
  minBtn->setEnabled(f);
  maxBtn->setEnabled(f);
  closeBtn->setEnabled(f);
  if (f==false)
     hide();
  else
     show();
}


// KMDIWindow
KMDIWindow::KMDIWindow ( QWidget* p, const char* name, int flag,
                       const char* icon) : QWidget(p, name)
{
    view = 0L;

    this->flag = flag;
    moveMode   = false;
    resizeMode = 0;
    eraseResizeRect = false;

    frame = new("QFrame") QFrame(this);
    frame->setFrameStyle( QFrame::Panel | QFrame::Raised );
    frame->setLineWidth(2);
    frame->setMouseTracking(true);
    frame->installEventFilter(this);
    frame->show();
    
    titleBar = new("KMDITitle") KMDITitle(icon, frame);
    titleBar->setMouseTracking(true);
    titleBar->installEventFilter(this);
    titleBar->setGeometry(0,0,width(),MIN_TITLE_HEIGHT);
    titleBar->caption->installEventFilter(this);
    titleBar->iconBtn->installEventFilter(this);
    titleBar->closeBtn->installEventFilter(this);
    titleBar->show();

//    if (!KMDIMgrBase::detachPic.isEmpty() && (flag&MDI_DETACHED)==0){
//       connect(titleBar->detachBtn, SIGNAL(clicked()),
//	       this,                SLOT  (slotDetach()));
//    }
    if (!KMDIMgrBase::minimizePic.isEmpty()){
       connect(titleBar->minBtn, SIGNAL(clicked()),
	       this,             SLOT  (slotMinimize()));
    }
    if (!KMDIMgrBase::maximizePic.isEmpty()){
       connect(titleBar->maxBtn, SIGNAL(clicked()),
	       this,             SLOT  (slotMaximize()));
    }
    if (!KMDIMgrBase::closePic.isEmpty() && (flag&MDI_CLOSED)==0 ){
       connect(titleBar->closeBtn, SIGNAL(clicked()),
	       this,               SLOT  (slotClose()));
    }
    connect (titleBar->caption, SIGNAL(moving()), 
	     this,              SLOT  (slotMoving()));

    isMinimized = false;
    isMaximized = false;
    isAttached  = true;
}


void KMDIWindow::drawResizeAnimation(QRect &start, QRect &finish)
{
    int times = RESIZE_TIMES;
    QRect r;
    int cX = start.x();
    int cY = start.y();
    int cW = start.width();
    int cH = start.height();
    XGrabServer(qt_xdisplay());
    for (int i=0; i<times; i++){
        r = QRect(cX, cY, cW, cH);
        drawRect(r);
        XFlush(qt_xdisplay());
        XSync(qt_xdisplay(), False);
        usleep(SLEEP_TIME);
        drawRect(r);
        cX += (finish.x()-start.x())/(times);
        cY += (finish.y()-start.y())/(times);
        cW += (finish.width()-start.width())/times;
        cH += (finish.height()-start.height())/times;
    }
    XUngrabServer(qt_xdisplay());
}

// 
void KMDIWindow::setView(QWidget* widget)
{
    widget->recreate(frame, 0, QPoint(0,titleBar->height()), true);
    widget->move(0,titleBar->height());
    view = widget;
    // now recurse the child list, and install event filters
    linkChildren(widget);
}

// this is a PHAT recursive function, it will search through the widget's children, and it's
// children's children....and install the appropriate event filters.
void KMDIWindow::linkChildren(QWidget* widget)
{
    QList<QObject> *list = (QList<QObject> *)(widget->children());
    if (list){
        for (unsigned int i=0; i< list->count(); i++){
	    if (list->at(i)->isWidgetType()==false)
	       continue;
            QWidget *w = (QWidget *)list->at(i);
            linkChildren(w);
        }
    }
    widget->installEventFilter(this);
}

void KMDIWindow::slotMoving()
{
  if (isAttached){
     moveMode = true;
  }
}

void KMDIWindow::slotMinimize(bool emitted)
{
    isMinimized   = true;
    if (!isMaximized){
       restoreX      = x();
       restoreY      = y();
       restoreWidth  = width();
       restoreHeight = height();
    }
    // now do a cool animation
    if (isVisible()){
        hide();
        QRect b(x(), y(), width(), height());
        QRect e(x() + width()/2,y() + height()/2,0,0);
        drawResizeAnimation(b, e);
    }
    else
        hide();
    if (emitted)
       emit minimized(this);

}

void KMDIWindow::slotMaximize (bool emitted)
{
  if(isAttached)
  {
    if (isMaximized){
        slotRestore();
    }
    else{
        isMaximized = true;
        QWidget *p = (QWidget *)parent();
        restoreX = x();
        restoreY = y();
        restoreWidth  = width();
        restoreHeight = height();
        hide();
         // now do a cool animation
        QRect b(x(), y(), width(), height());
        QRect e(0,0, p->width(), p->height());
        drawResizeAnimation(b, e);
        show();
        setGeometry (0, 0, p->width(), p->height());
	if (emitted)
	   emit maximized (this);
    }
 
  }
}

void KMDIWindow::slotDetach()
{
  if (isAttached && parentWidget()!=0L){
     restoreX = x();
     restoreY = y();
     restoreWidth  = width();
     restoreHeight = height();
     oldParent = parentWidget();
     oldWFlags = getWFlags();
     QPoint pt = (parentWidget())->mapToGlobal(pos());
     recreate(0L, 0, pt, false);
     setIconText(QWidget::caption());
     titleBar->setEnabled(false);
     setGeometry(x(),y(),width(),height()-MIN_TITLE_HEIGHT);
     frame->setLineWidth(0);
     frame->setFrameStyle( QFrame::NoFrame );
     frame->hide();

     view->recreate(this, 0, QPoint(0,titleBar->height()), false);
//     titleBar->recreate(this, 0, QPoint(0,0), false);

//     if (!KMDIMgrBase::detachPic.isEmpty() && (flag&MDI_DETACHED)==0){        
//        connect(titleBar->detachBtn, SIGNAL(clicked()),
//  	       this,              SLOT  (slotAttach()));
//      }
//     KWM::setDecoration(winId(),KWM::tinyDecoration);
     isAttached = false;

     show();
  }
}

void KMDIWindow::slotAttach()
{
  if (!isAttached && parentWidget()==0L){
     recreate(oldParent, 0, QPoint(restoreX, restoreY), false);
     titleBar->setEnabled(true);
     setGeometry(restoreX, restoreY, restoreWidth, restoreHeight);

     frame->setFrameStyle( QFrame::Panel | QFrame::Raised );
     frame->setLineWidth(2);
     frame->show();
//     titleBar->recreate(frame, 0, QPoint(0,titleBar->height()), false);
     view->recreate(frame, 0, QPoint(0,titleBar->height()), false);
     view->move(0,titleBar->height());


//     if (!KMDIMgrBase::detachPic.isEmpty() && (flag&MDI_DETACHED)==0){
//        connect(titleBar->detachBtn, SIGNAL(clicked()),
// 	       this,                SLOT  (slotDetach()));
//     }

     isAttached = true;
     show();
  }
}

void KMDIWindow::setCaption (const char *txt )
{
  titleBar->caption->setText(txt);
  QWidget::setCaption(txt);
  emit captionChanged (this, txt);
}

void KMDIWindow::slotSelect(bool restoreIfMinimized)//, bool emitted)
{
   if (restoreIfMinimized){
     if (isMinimized)
        slotRestore();
  }
  setFocus();

//  if (emitted)
//  emit selected(this);
}

void KMDIWindow::changeColorSelect(bool f)
{
  //if (f)
  //   titleBar->caption->setBackgroundColor(activColor);
  //else
  //   titleBar->caption->setBackgroundColor(pasivColor);
  isSelected=f;
}


void KMDIWindow::slotRestore ()
{
  // Min->Normal
  if (isMinimized){
     isMinimized = false;
     if (isMaximized){
        QWidget *p = (QWidget *)parent();
         // now do a cool animation
        QRect b(p->width()/2, 
		p->height()/2, 
		0, 
		0);
        QRect e(0,0, p->width(), p->height());
        drawResizeAnimation(b, e);
        show();
        setGeometry (0, 0, p->width(), p->height());
        emit restored (this);
	return;
     }
     QRect b(restoreX + restoreWidth/2, restoreY + restoreHeight/2, 0, 0);
     QRect e(restoreX, restoreY, restoreWidth, restoreHeight);
     drawResizeAnimation(b, e);
     move (restoreX, restoreY);
     show();
     resize (restoreWidth, restoreHeight);
  }
  
  // Max->Normal
  if (isMaximized){
     isMaximized = false;
     // bust that animation
     QRect b(x(), y(), width(), height());
     QRect e(restoreX, restoreY, restoreWidth, restoreHeight);
     drawResizeAnimation(b, e);
     move (restoreX, restoreY);
     resize (restoreWidth, restoreHeight);
  }
    
  slotSelect();
  emit restored(this);
}

void KMDIWindow::slotClose()
{
  if (view){
     if (view->close()){
        emit closed(this);
	recreate(0L, 0, QPoint(0,0)); 
	close(true);
     }
  }
  else{
    if (close());{
       emit closed(this); 
       recreate(0L, 0, QPoint(0,0)); 
       close(true);
    }
  }
}

bool KMDIWindow::eventFilter ( QObject *, QEvent *e )
{
    if (e->type() == Event_MouseMove){
        mouseMoveEvent ( (QMouseEvent *)e );
    }
    
    if (e->type() == Event_MouseButtonPress){
        mousePressEvent( (QMouseEvent *)e );
    }
   
    if (e->type() == Event_MouseButtonRelease){
        mouseReleaseEvent ( (QMouseEvent *)e );
    }
    return false;
}

void KMDIWindow::doResizing()
{
  drawRect(anchorRect, FANCY_RECT);
  QPoint np   = getCursorPos();
  QPoint np_g = mapToParent(np);
  QPoint op_g = mapToParent(anchorPoint);

  int rX = anchorRect.x();
  int rY = anchorRect.y();
  int rW = anchorRect.width();
  int rH = anchorRect.height();
  int dX = np_g.x()-op_g.x();
  int dY = np_g.y()-op_g.y();

  switch (resizeMode){
  case LOWER_RIGHT:
    rW += dX;
    rH += dY;
    break;
  case UPPER_RIGHT:
    rY += dY;
    rW += dX;
    rH -= dY;
    break;
  case LOWER_LEFT:
    rX += dX;
    rW -= dX;
    rH += dY;
    break;
  case UPPER_LEFT:
    rX += dX;
    rY += dY;
    rW -= dX;
    rH -= dY;
    break;
  }
  
  anchorRect = QRect(QMIN(rX, x() + width() -MINIMUM_X),
	       QMIN(rY, y() + height()-MINIMUM_Y),
	       QMAX(rW, MINIMUM_X), QMAX(rH, MINIMUM_Y));
  anchorPoint= getCursorPos();

  drawRect(anchorRect, FANCY_RECT);
}

void KMDIWindow::doMove(QMouseEvent*)
{
  if (cursor().shape()!=crossCursor.shape())
     setCursor(crossCursor);
  QPoint rp  =pos();
  QRect  rect=parentWidget()->frameGeometry();
  QPoint np  =getCursorPos();
  QPoint np_g  =mapToParent(np);
  QPoint op_g  =mapToParent(anchorPoint);

  if (np_g.x()<5)
     np_g.setX(5);
  else if (np_g.x()>rect.width()-5)
     np_g.setX(rect.width()-5);

  if (np_g.y()<5)
     np_g.setY(5);
  else if (np_g.y()>rect.height()-5)
     np_g.setY(rect.height()-5);
  move(rp+np_g-op_g);
}

QPoint KMDIWindow::getCursorPos()
{
  QPoint c = QCursor::pos();
  QPoint p = mapFromGlobal(c);
  return p;
}

    
void KMDIWindow::focusInEvent(QFocusEvent *)
{
  if (!isVisible())
     show();
  raise();
  emit selected(this);
}

void KMDIWindow::focusOutEvent(QFocusEvent *)
{
}

void KMDIWindow::mouseMoveEvent ( QMouseEvent *e )
{
     if (e->state() & LeftButton){
      if (resizeMode){
	  doResizing();
	  return;
	}
            
	if (moveMode){
	   doMove(e);
	   return;
	}
     }
     QPoint currentPoint = getCursorPos();
     int r = isInResizeArea(currentPoint.x(), currentPoint.y());
     if (r!=0){
        if (r == UPPER_LEFT || r == LOWER_RIGHT)
	   setCursor ( SizeFDiagCursor);
	else
	   setCursor ( SizeBDiagCursor);
     }
     else
       setCursor (ArrowCursor);
}

void KMDIWindow::mouseReleaseEvent (QMouseEvent *)
{
  if (resizeMode!=0){
     QRect rect = frameGeometry();
     drawRect(rect, FANCY_RECT);
     drawRect(anchorRect, FANCY_RECT);
     setGeometry(anchorRect);
  }
  resizeMode = 0;
  moveMode   = 0;
  setCursor(arrowCursor);

}


void KMDIWindow::mousePressEvent ( QMouseEvent *)
{
  isMinimized = false;
  slotSelect(true);
  anchorPoint = getCursorPos();
  anchorRect  = frameGeometry();
  corner = isInResizeArea (anchorPoint.x(), anchorPoint.y());
  if (corner){
    resizeMode = corner;
  }
}

void KMDIWindow::resizeEvent ( QResizeEvent *)
{
    frame->setGeometry(0,
		       0,
		       width(), 
		       height());
    QRect r = frame->contentsRect();
    int hTitle;
    if (isAttached)
       hTitle=18;
    else
       hTitle= 0;

    if (isAttached)
       titleBar->setGeometry(BORDER,
			     BORDER,
			     width()-2*BORDER, 
			     hTitle);
    if (view)
       view->setGeometry(r.x() + BORDER,     
			 hTitle+ 5, 
			 r.width()-2*BORDER, 
			 r.height() - hTitle - 3*BORDER);

    QString Geometry;
    Geometry.sprintf("%dx%d", geometry().width(), geometry().height());
    kapp->getConfig()->setGroup("KMDIMgrBase");
    kapp->getConfig()->writeEntry(
        "MDIWindowGeometry", Geometry);
}

int KMDIWindow::isInResizeArea ( int x, int y)
{
    if (x > (width() - RESIZE_BORDER)){
        if (y > (height() - RESIZE_BORDER)) // lower right corner
            return LOWER_RIGHT;
        if (y < RESIZE_BORDER) // upper right corner
            return UPPER_RIGHT;
    }

    else if (x < RESIZE_BORDER){
        if (y > (height() - RESIZE_BORDER)) // lower left corner
            return LOWER_LEFT;
        if (y < RESIZE_BORDER) // upper left corner
            return UPPER_LEFT;
    }     
    return 0;
}


void KMDIWindow::drawRect ( QRect &r, bool fancy )
{
  if (!r.isNull()){
     QWidget* pr=parentWidget();
     XGCValues gcvals;
     gcvals.foreground=black.pixel();
     gcvals.subwindow_mode=IncludeInferiors;
     gcvals.function=GXinvert;
     int valmask=GCForeground|GCSubwindowMode|GCFunction;
     GC gc=XCreateGC(x11Display(),pr->handle(),valmask,&gcvals);
     XDrawRectangle(x11Display(),pr->handle(),gc,r.x(),r.y(),r.width(),r.height());
     if (fancy){
        int dY = r.y() + r.height()/3;
	XDrawLine(x11Display(), pr->handle(), gc, r.x(), dY, r.x() + r.width(), dY);
	dY = r.y() + 2*(r.height())/3;
	XDrawLine(x11Display(), pr->handle(), gc, r.x(), dY, r.x() + r.width(), dY);
	int dX = r.x() + r.width()/3;
	XDrawLine(x11Display(), pr->handle(), gc, dX, r.y(), dX, r.y() + r.height());
	dX = r.x() + 2*(r.width())/3;
	XDrawLine(x11Display(), pr->handle(), gc, dX, r.y(), dX, r.y() + r.height());
     }
     XFreeGC(x11Display(),gc);
  }
}


//-----------------------------
// KMDIMgrBase
//-----------------------------

KMDIMgrBase::KMDIMgrBase ( QWidget* p, const char *name)
        : QFrame (p, name )
{
    minimizePic = "iconify.xpm";
    maximizePic = "maximize.xpm";
    closePic    = "close.xpm";

    selectedWnd = 0L;
    windowList  = new("QList<KMDIWindow>") QList<KMDIWindow>;
    windowList->setAutoDelete(false);
    numWindows = 0;

    kapp->getConfig()->setGroup("KMDIMgrBase");
    QString geometry=
        kapp->getConfig()->readEntry(
            "MDIWindowGeometry", "" );
    if(!geometry.isEmpty())
    {
        int width, height;
        sscanf( geometry,"%dx%d", &width, &height );
        setDefaultWindowSize(width, height);
    }
    else
       setDefaultWindowSize( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );

    setDefaultWindowPos ( DEFAULT_WINDOW_POSX,  DEFAULT_WINDOW_POSY );
    smartPlacement = false;

    setFrameStyle(QFrame::NoFrame);
    setFocusPolicy(QWidget::StrongFocus);
}

KMDIMgrBase::~KMDIMgrBase()
{
}

void KMDIMgrBase::setSmartPlacement(bool yn)
{
    smartPlacement = yn;
}

void KMDIMgrBase::setDefaultWindowSize ( int w, int h)
{
    defaultWindowWidth  = w;
    defaultWindowHeight = h;
}

void KMDIMgrBase::setDefaultWindowPos ( int x, int y )
{
    defaultWindowPosX = x;
    defaultWindowPosY = y;
}
   
void KMDIMgrBase::handleWindowSelect(KMDIWindow *win)
{
  for (KMDIWindow* w=windowList->first(); w!=0L; w=windowList->next()){
      if (w == win){
	 selectedWnd = w;
         w->changeColorSelect(true);
         w->getTitlebar()->caption->paintState();
      }
      else{   
	 if (w->isMinimized)
	    w->hide();
	 else{
	    if (!w->isVisible())
	       w->show();
	 }
         w->changeColorSelect(false);
         w->getTitlebar()->caption->paintState();
      }
  }
}

void KMDIMgrBase::nextWindow ()
{
    windowList->find(selectedWnd);
    KMDIWindow* w = windowList->next();
    if (w==0L)
       w = windowList->first();
    if (w)
       w->slotSelect();
}

void KMDIMgrBase::prevWindow ()
{
    windowList->find(selectedWnd);
    KMDIWindow* w = windowList->prev();
    if (w==0L)
       w = windowList->last();
    if (w)
       w->slotSelect();
}

KMDIWindow* KMDIMgrBase::addWindow(QWidget *widget, int flag, const char* icon)
{
    KMDIWindow *w = new("KMDIWindow") KMDIWindow(this, widget->name(), flag, icon);

    w->setView(widget);
    addWindow(w, flag);
    return w;
}

void KMDIMgrBase::addWindow(KMDIWindow *w, int flag)
{
    CHECK_PTR(w);
    if (w->parentWidget() != this)
        w->recreate(this, 0, QPoint(0,0));
    windowList->append(w);

    // now we place and resize the new("window") window
    int x, y;
    if (smartPlacement){
        x = (width() / 2)  - (defaultWindowWidth  / 2);
        y = (height() / 2) - (defaultWindowHeight / 2);
    }
    else{
        x = defaultWindowPosX;
        y = defaultWindowPosY;
    }
    
    while (isPointTaken(x, y)){
       x += STARTING_OFFSET;
       y += STARTING_OFFSET;
    }
    w->move(x, y);

    kapp->getConfig()->setGroup("KMDIMgrBase");
    QString geometry =
        kapp->getConfig()->readEntry(
            "MDIWindowGeometry", "" ); 
  

    if (!geometry.isEmpty())
    {
        int width, height;
        sscanf( geometry,"%dx%d", &width, &height );
        setDefaultWindowSize(width, height);
    }
    w->resize(defaultWindowWidth, defaultWindowHeight);

    connect(w, SIGNAL(selected(KMDIWindow *)), 
	       SLOT  (handleWindowSelect( KMDIWindow *)) );
    connect(w, SIGNAL(closed(KMDIWindow *)),   
	       SLOT  (removeWindow(KMDIWindow *)) );

    w->installEventFilter(this);

    // make sure the window gets deleted when we are done
    numWindows++;

    if ((flag&MDI_ICONIFY)==MDI_ICONIFY)
       w->hide();
    else{
       w->show();
       w->slotSelect();
    }
    emit windowAdded(w);
    handleWindowSelect(w);
}

void KMDIMgrBase::removeWindow ( KMDIWindow *win )
{
    if (win == selectedWnd)
        selectedWnd = 0L;
    
    int i = windowList->find(win);
    if ( i > -1){
        windowList->remove();
        numWindows --;
        emit windowRemoved(win);
    }
    else
        warning("KMDIMgrBase::RemoveWindow: window not found.");

    KMDIWindow *w=windowList->first();
    if(w){
        w->slotSelect(TRUE);
        w->raise();
        handleWindowSelect(w);
    }
    
}

void KMDIMgrBase::removeWindow(const char* name)
{
  KMDIWindow* wnd=getWindowByName(name, false);
  if (wnd!=0L)
     removeWindow(wnd);
}
        
void KMDIMgrBase::cascade()
{
    int posX = 0;
    int posY = 0;

    kapp->getConfig()->setGroup("KMDIMgrBase");
    QString geometry =
        kapp->getConfig()->readEntry(
            "MDIWindowGeometry", "" );
     if(!geometry.isEmpty())
     {
        int width, height;
       sscanf( geometry, "%dx%d", &width, &height );
       setDefaultWindowSize(width, height);
    }

    KMDIWindow* w;
    for (w=windowList->first();w!=0L;w=windowList->next()){
        if (w->isVisible()){ // we act on visible windows, not necessarily restored ones
            w->setGeometry(posX, posY, defaultWindowWidth, defaultWindowHeight);
            posX += STARTING_OFFSET;
            posY += STARTING_OFFSET;
        }
    }
    for (w=windowList->last();w!=0L;w=windowList->prev()){
        if (!w->isVisible())
	   continue;
	w->slotSelect(true);
	break;
    }
}
       
void KMDIMgrBase::tile()
{
      kapp->getConfig()->setGroup("KMDIMgrBase");
      QString geometry =
        kapp->getConfig()->readEntry(
            "MDIWindowGeometry", "" );

    if (!geometry.isEmpty())
    {
       int width, height;
       sscanf( geometry, "%dx%d", &width, &height );
       setDefaultWindowSize(width, height);
    }

    bool isOdd; // is there an odd number of windows?
    // tiles the windows
    int startX = 0;
    int startY = 0;// menu->height()+tool->height();
    
    int totalWidth = width()-startX;
    int totalHeight = height()-startY;

    int eachHeight=0;
    int eachWidth=0;

    int columns = 0;
    int rows = 0;

    int numVisible = 0;
    
    KMDIWindow *w = windowList->first();
    while (w)
    {
        if (w->isVisible())
            numVisible++;
        
        w = windowList->next();
    }
    
    if (numVisible == 0)
        return;

    if (numVisible%2)
        isOdd = true;
    else
        isOdd = false;

    if (numVisible >= 5)
        columns = 3;
    if (numVisible < 5) 
        columns = 2;
    if (numVisible < 3) 
        columns = 1;

    if (numVisible == 1)
        rows = 1;
    else
    {
        if (isOdd)
            rows = (numVisible +1) / columns;  // treat 3 like 4, 5 like 6, etc
        else
            rows = numVisible / columns;
    }
   
    eachHeight = int(totalHeight / rows);
    eachWidth  = int(totalWidth  / columns);
    
      
    // now call setGeometry() on each window accordingly
    int thisX = startX;
    int thisY = startY;

    w=windowList->first();
    for (int c=0;c<columns;c++){                
        for (int r=0;r<rows;r++){
            if (w){
                while (w &&  !(w->isVisible())){
		   w=windowList->next();
                }
		if (w!=0L){
		   // if the last window and odd, we must double it
		  if (r+1 == rows && c+2 == columns && isOdd && columns >1) 
		     w->setGeometry(thisX, thisY, 2*eachWidth, eachHeight);
		  else
		     w->setGeometry(thisX, thisY, eachWidth, eachHeight);
		}
            }
            thisY += eachHeight;
	    w=windowList->next();
        } // end for rows
            
        thisX += eachWidth;
        thisY = startY;
        
    } // end for columns

    for (w=windowList->last();w!=0L;w=windowList->prev()){
        if (!w->isVisible())
	   continue;
	w->slotSelect(true);
	break;
    }

    QString Geometry;
    Geometry.sprintf("%dx%d", defaultWindowWidth, defaultWindowHeight);
    kapp->getConfig()->writeEntry(
        "MDIWindowGeometry",Geometry);
}


KMDIWindow *KMDIMgrBase::getWindowByName ( const char *name, bool caseSensitive )
{
    KMDIWindow *w = windowList->first();
    QString name1 (name);
    if (!caseSensitive)
        name1 = name1.lower();
    QString name2;
    while ( w ){
        name2 = w->name();
        if (!caseSensitive)
           name2 = name2.lower();
        if (name1 == name2){
            return ( w );
        }
        
        w = windowList->next();
    }

    return 0L; // didn't find it
}
 
KMDIWindow *KMDIMgrBase::getWindowByIndex ( int index )
{
    KMDIWindow *w = windowList->at(index);
    if (w)
        return(w);
    else
        return 0L;
}
    

bool KMDIMgrBase::isPointTaken ( int x, int y)
{
    KMDIWindow *w;
    for (int i=0; i< numWindows; i++){
        w = getWindowByIndex(i);
        if (w){
            if ((w->x() == x) && (w->y() == y))
	       return true;
        }
    }
    return false;
}

void KMDIMgrBase::slotSetColor(const QColor &col, int Type)
{
  if (Type&TYPE_BGMDI){
     setBackgroundColor (col);  
     kapp->getConfig()->writeEntry(
         "BackgroundImage", "");
  }
}

void KMDIMgrBase::resizeEvent ( QResizeEvent *)
{
  if (selectedWnd!=0L && selectedWnd->IsMaximized()){
      selectedWnd->setGeometry(0,
			       0,
			       width(),
			       height());
  }
}

void KMDIMgrBase::slotSetBgImage(const char* url)
{
#ifdef EDEBUG
  cout << "KMDIMgrBase::slotSetBgImage:"<<url<<endl;
#endif
  if (url==0L || strlen(url)<1)
     return;

  bgPixmap.load(url);
  setBackgroundPixmap (bgPixmap);
  update();
}

#include "KMDIMgrBase.moc"
