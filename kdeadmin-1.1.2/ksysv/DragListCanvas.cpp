/*

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <qpainter.h>
#include <qkeycode.h>
#include <qtooltip.h>
#include <qdrawutl.h>
#include <kmsgbox.h>
#include "DragListCanvas.h"
#include "ksv_core.h"
#include <stdlib.h>
#include <kapp.h>

// include meta-object code
#include "DragListCanvas.moc"

#include "Constants.h"

KSVDragListCanvas::KSVDragListCanvas(QWidget* parent, const char* name)
: KDNDWidget(parent, name),
  focus(NONE)
{
  kil = kapp->getIconLoader();
  dz = new KDNDDropZone(this, DndText);

  // connect the dropZone with my dropAction
  connect(dz, SIGNAL(dropAction(KDNDDropZone* )), this, SLOT(slotDropEvent( KDNDDropZone* )));
  // set background mode
  setBackgroundMode( NoBackground );

  // init (double-) buffer
  buffer = QPixmap(1,1);

  adjustSize();

  // initialize other class members
//   focus = 0;
  noFocus = FALSE;
  oldFocus = -1;
  pressed = FALSE;
  ro = FALSE;
  runlevel = "Foo";
  origin = FALSE;
  popup = 0;
  _newColor = blue;
  _changedColor = red;
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(autoScrollTimer()));

  // init members needed by draw()
  hiCol = kapp->selectColor;
  hiText = kapp->selectTextColor;
  midCol = palette().active().mid();

  blackCol = palette().active().text();
  whiteCol = palette().active().base();

  italic = kapp->fixedFont;
  //  italic.setItalic(TRUE);
}

KSVDragListCanvas::~KSVDragListCanvas() {
  // nothing yet
}

void KSVDragListCanvas::dndMouseMoveEvent ( QMouseEvent* _mouse ) {
  // 'pressed' is set in mousePressedEvent(...)
  if ( !pressed )
    return;

  int x = _mouse->pos().x();
  int y = _mouse->pos().y();

  if ( abs( x - press_x ) > Dnd_X_Precision || abs( y - press_y ) > Dnd_Y_Precision ) {

    KSVDragData* localO = getObject(y);
    if (!localO)
      return;

    const QString data = localO->toString();
    QPixmap pixmap = KSVCore::drawDragIcon( defaultIcon, monoIcon, 0, localO );

    const QPoint p = mapToGlobal( _mouse->pos() );

    const int dx = - ( monoIcon.width() + 2); // - pixmap.width() / 2;
    const int dy = - ( pixmap.height() / 2 + 2 );

    startDrag( new KDNDIcon( pixmap, p.x() + dx, p.y() + dy ), data.data(), data.length(), DndText, dx, dy );
    pressed = FALSE;
    timer->start(50);
  } else {
    // nothing to do
  }
}

void KSVDragListCanvas::dndMouseReleaseEvent( QMouseEvent* e ) {
  pressed = FALSE;
  redrawFocus();
}

void KSVDragListCanvas::mousePressEvent( QMouseEvent * _mouse ) {
  // Does the user want to select the icon ?
  if (withinLimits( _mouse->x(), _mouse->y() )) {

    if ( _mouse->button() == LeftButton ) {
      pressed = TRUE;
    } else if ( _mouse->button() == RightButton ) {
      pressed = FALSE;
      setFocus();
      if (popup)
	popup->popup( mapToGlobal(_mouse->pos()) );
    }
  }

  oldFocus = focus;
  focus = list.find(getObject(_mouse->y()));

  redrawFocus();
}

void KSVDragListCanvas::mouseReleaseEvent( QMouseEvent* e ) {
  KDNDWidget::mouseReleaseEvent( e );
}

void KSVDragListCanvas::mouseDoubleClickEvent( QMouseEvent* _mouse ) {
  if (_mouse->button() == LeftButton)
    emit sigDoubleClick( this, getObject(_mouse->y()), QPoint(_mouse->x(), _mouse->y()));
}

void KSVDragListCanvas::paintEvent( QPaintEvent* e ) {
  // just blit the buffer onto the widget...
  const QRect& rect = e->rect();
  bitBlt( this, rect.topLeft(), &buffer, rect );
}

void KSVDragListCanvas::slotDropEvent( KDNDDropZone* _zone) {
  if (!ro &&_zone->getAcceptType() == _zone->getDataType() ) {
    const QString droppedData = _zone->getData();

    QPoint akt_pos = mapFromGlobal(QCursor::pos());
    bool upper;
    int position = list.find(getObject(akt_pos.y(), &upper));

    KSVDragData* newO = new KSVDragData();
    newO->fromString(droppedData);

    if (!upper)
      position++;

    if (!insertOK(newO, position)) {
      delete newO;
      return;
    }

    // for removing elsewhere
    const QString _old_rl = newO->runlevel();

    // change sort number
    const int temp = generateNumber(position);
    if (temp == -1) {
      emit cannotGenerateNumber();
      delete newO;
      return;
    } else
      newO->setNumber(temp);

    insertItem(position, newO);

    emit sigDropOccured( _zone, this, upper );
    if (_old_rl != runlevel)
      emit sigDroppedFrom(_old_rl, newO);
  }
}

void KSVDragListCanvas::setDefaultIcon( const char* _icon ) {
  QString icon_name = QString().sprintf("ksysv_%s.xpm", _icon);
  QString mono_name = QString().sprintf("mono_%s.xpm", _icon);
  QString multi_name = QString().sprintf("multi_%s.xpm", _icon);

  defaultIcon = kil->loadIcon( icon_name );
  multiIcon = kil->loadIcon( multi_name );
  monoIcon = kil->loadIcon( mono_name );

  // prepare the icon in "selected" state
  selIcon = defaultIcon;
  QPainter p;
  QBrush b( kapp->selectColor, Dense4Pattern );
  p.begin(&selIcon);
  p.fillRect( 0, 0, selIcon.width(), selIcon.height(), b );
  p.end();
  selIcon.setMask(monoIcon);

  draw();
}

QPixmap KSVDragListCanvas::getDefaultIcon() {
  return defaultIcon;
}

void KSVDragListCanvas::resizeEvent( QResizeEvent* e ) {
  const int w = width();
  const int h = height();

  buffer.resize(w,h);
  emit sigResize(w,h);

  const int w_o = e->oldSize().width();
  const int h_o = e->oldSize().height();

  if (w_o < w || h_o < h)
    draw();
}

KSVDragData* KSVDragListCanvas::getObject( int _y, bool* upperHalf) {
  if (list.isEmpty())
    return 0;

  int res = _y / ENTRY_HEIGHT;

  *upperHalf = (_y % ENTRY_HEIGHT) < ENTRY_HEIGHT / 2 ? TRUE : FALSE;
  res = count() - 1 < res ? res - 1 : res;

  return list.at(res);
}

KSVDragData* KSVDragListCanvas::getObject(int _y) {
  bool foo;
  return getObject( _y, &foo );
}

void KSVDragListCanvas::insertItem(int _index, KSVDragData* _o, const bool genNr) {
  if (runlevel != _o->runlevel()) {
    KSVDragData* _tmp = _o;
    _o = new KSVDragData(*_tmp);

    _o->setNew();
    _o->setChanged(FALSE);
  }

  _o->setRunlevel(runlevel);

  // generate a sorting number if neccessary
  if (genNr) {
    const int result = generateNumber(_index);
    if (result > -1)
      _o->setNumber(result);
    else {
      emit cannotGenerateNumber();
      delete _o;
      return;
    }
  }

  int pos = list.find(_o);
  if (pos == -1) {
    list.insert( (uint)_index, _o);
  } else {
    KSVDragData* tmp_o = list.at(pos);
    list.remove(tmp_o);

    tmp_o->setNumber( _o->number() );

    if (pos < _index)
      _index--;

    list.insert( (uint)_index, tmp_o);

    delete _o;
  }

  adjustSize();
  int start, end;

  if (pos != -1) {
    start = pos < _index ? pos : _index;
    end = start != pos ? pos : _index;
  } else {
    start = _index;
    end = count() - 1;
  }

  draw(start,end);
  updateTooltips();

  if (pos != _index) {// if the item is not inserted into the same position as an existing one
    emit sigChanged();
  }
}

int KSVDragListCanvas::count() const {
  return (const int)list.count();
}

void KSVDragListCanvas::keyPressEvent( QKeyEvent* e ) {
  const int maxIndex = count() - 1;

  switch( e->key() ) {                    // Look at the key code

  case Key_Up:
    oldFocus = focus;
    if (focus > 0)
      focus--;

    redrawFocus();
    emit sigScrollUp(focus);
    break;

  case Key_Down:
    oldFocus = focus;
    if (focus < maxIndex)
      focus++;

    redrawFocus();
    emit sigScrollDown(focus);
    break;

  case Key_Enter:
    emit sigDoubleClick(this, at(focus), QPoint(1, focus * ENTRY_HEIGHT));
    break;

  case Key_Return:
    emit sigDoubleClick(this, at(focus), QPoint(1, focus * ENTRY_HEIGHT));
    break;

  default:                              // If not an interesting key,
    e->ignore();                        // we don't accept the event
    return;
  }
}

void KSVDragListCanvas::removeItem( KSVDragData* _item ) {
  const int i = list.find(_item);
  if (i != -1) {
    list.remove(list.at(i));
    adjustSize();
    draw(i, count());
    updateTooltips();
    if (list.find(_item) != i) { // if it wasn't a drop on itself
      emit sigChanged();
    }
  }

  if (focus)
    {
      if (focus >= count())
	focus--;
    }
  else
    {
      noFocus = true;
      emit selected(0);
    }
}

void KSVDragListCanvas::clear() {
  list.setAutoDelete(TRUE);
  int max = count();

  // I don´t do list.clear because of cpu load

  for(int i = 0; i < max; ++i) {
    // keep GUI alive
    qApp->processEvents();
    list.remove(list.at(0));
  }

  list.setAutoDelete(FALSE);
  adjustSize();
  draw();
  updateTooltips();

  // keep GUI alive
  qApp->processEvents();

  focus=NONE;
}

void KSVDragListCanvas::setRunlevel( QString _run ) {
  runlevel = _run;
}

void KSVDragListCanvas::updateTooltips() {
  const int max = count();
  int y = 0;

  // add tooltip
  for (int i = 0; i < max; i++) {
    QToolTip::add(this, QRect(0, y, 80, ENTRY_HEIGHT), list.at(i)->filenameAndPath());
    y += ENTRY_HEIGHT;
  }
}

void KSVDragListCanvas::adjustSize() {
  resize(width(), count() * ENTRY_HEIGHT);
}

const QString KSVDragListCanvas::getRunlevel() const {
  return runlevel;
}

void KSVDragListCanvas::setReadOnly(bool _value) {
  ro = _value;
}

KSVDragData* KSVDragListCanvas::at(int _index) {
  if (_index < count() && _index > -1)
    return list.at(_index);
  else
    return 0;
}

void KSVDragListCanvas::mouseMoveEvent( QMouseEvent* e ) {
  _lastPos = mapToGlobal(e->pos());

  KDNDWidget::mouseMoveEvent( e );
}

void KSVDragListCanvas::dragEndEvent() {
  timer->stop();

  KDNDWidget::dragEndEvent();
}

void KSVDragListCanvas::setOrigin( bool val ) {
  const bool old_nof = noFocus; // remember noFocus
  noFocus = FALSE;

  const bool oldOrigin = origin;
  origin = val;
  if ( origin && !oldOrigin) {
    redrawFocus();
    emit sigOrigin( FALSE );
  } else if ( oldOrigin && !origin ) {
    redrawFocus();
  } else if (old_nof)
    redrawFocus();

  if (origin)
    emit selected(list.at(focus));
}

bool KSVDragListCanvas::isOrigin() const {
  return origin;
}

void KSVDragListCanvas::focusInEvent( QFocusEvent* ) {
  if (count() && focus == NONE)
    {	
      focus = 0;
      redrawFocus();
    }

  setOrigin(TRUE);
}

void KSVDragListCanvas::focusOutEvent( QFocusEvent* ) {
  redrawFocus();
}

void KSVDragListCanvas::draw() {
  draw(0, count() - 1);
}

void KSVDragListCanvas::draw( const int index ) {
  draw(index,index);
}

void KSVDragListCanvas::draw( int start, int end ) {
  // available fonts && colors
  /*
    QColor hiCol;
    QColor blackCol;
    QColor whiteCol;
    QColor hiText;
    QColor midCol;
    QColor paint_back;
    QColor paint_text;
    QFont italic;
  */

  const int list_count = count();

  end = end < list_count ? end : list_count - 1;

  if (start < 0) warning("start < 0");

  // trying to optimize a bit more
  // is "register" ok?
  register int paint_y = ENTRY_HEIGHT * start;
  register int paint_width = width() + 1;
  register int paint_height = ENTRY_HEIGHT;

  QPainter p;

  // equals if height() == 1
  if(start > end) {
    // draw last line (since I can't resize to (width(), 0)
    p.begin(&buffer);
    p.setPen( whiteCol );
    p.drawLine( 0, 0, paint_width, 0);
    p.end();

    // the rest is not of interest to us
    return;
  }

  end++;

  // we will only draw in this area
  const QRect area (QPoint(0, paint_y), QPoint(paint_width, end * ENTRY_HEIGHT));

  p.begin( &buffer );
  p.fillRect(area, whiteCol);

  KSVDragData* localO;
  QPixmap* drawnIcon = 0;

  for (int i = start; i < end; ++i) {
    // we will do everything with this KSVDragData object
    localO = list.at(i);

    if ( origin && !noFocus && i == focus ) {
      paint_back = hiCol;
      paint_text = hiText;

      drawnIcon = &selIcon;

      //      p.setBackgroundMode( OpaqueMode );
      p.fillRect( 14 + defaultIcon.width() + 4, paint_y + 2,
		  p.fontMetrics().width(localO->currentName()) + 2,
		  ENTRY_HEIGHT - 4, paint_back );

      //      p.setBackgroundMode( TransparentMode );

    } else {
      paint_back = whiteCol;
      paint_text = blackCol;

      drawnIcon = &defaultIcon;
    }

    // paint the label & icon
    if (localO->isNew())
      p.setPen(_newColor);
    else if (localO->isChanged())
      p.setPen(_changedColor);
    else
      p.setPen( blackCol );

    p.setFont( italic );
    p.drawText( 1, paint_y + 14, QString().sprintf("%.2i", localO->number()));
    p.drawPixmap( QPoint( 17, paint_y + 2), *drawnIcon );

    p.setPen( paint_text );
    p.setFont( kapp->generalFont );
    p.drawText( 15 + (*drawnIcon).width() + 4, paint_y + 14, localO->currentName() );

    // move next picture down...
    paint_y += paint_height;
  }

  p.end();

  bitBlt( this, area.topLeft(), &buffer, area );
}

void KSVDragListCanvas::addPMenu( QPopupMenu* menu ) {
  popup = menu;
}

void KSVDragListCanvas::selectItem( int val ) {
  if (val == NONE) {
    noFocus = TRUE;
  } else {
    noFocus = FALSE;
    oldFocus = focus;
    focus = val < count() ? val : count();
  }

  redrawFocus();

  if (noFocus)
    emit selected(0);
}

int KSVDragListCanvas::find( const KSVDragData* o ) const
{
  return ((KSVList)list).find(o);
}

void KSVDragListCanvas::keyReleaseEvent( QKeyEvent* e ) {

  switch( e->key() ) {                    // Look at the key code

  default:                              // If not an interesting key,
    e->ignore();                        // we don't accept the event
    return;
  }
}

bool KSVDragListCanvas::withinLimits( int x, int y ) const {
  return ((0 <= x <= width()) && (0 <= y <= height()) ? TRUE : FALSE);
}

int KSVDragListCanvas::generateNumber( const int index ) {
  register int result;
  KSVDragData* high = list.at(index);
  KSVDragData* low = list.at(index - 1);

  // ugly
  if (!low && !high)
    return 50;

  if (!low)
    result = high->number() / 2;
  else if (!high)
    result = (100 - low->number()) / 2 + low->number();
  else
    result = (high->number() - low->number()) / 2 + low->number();

   if ((high && result == high->number()) || (low && result == low->number()))
     result = -1;

  return result;
}

bool KSVDragListCanvas::insertOK( KSVDragData* data, const int where ) {
  const int tmp = list.find(data);

  if (tmp == -1)
    return TRUE;
  else
    return ((where == tmp || where - 1 == tmp) ? FALSE : TRUE);
}

int KSVDragListCanvas::focusItem() const {
  return focus;
}

void KSVDragListCanvas::redrawFocus() {
  if (oldFocus > -1)
    draw(oldFocus);
  if (focus > -1 && focus != oldFocus)
    draw(focus);
}

void KSVDragListCanvas::autoScrollTimer() {
  emit mouseMove(_lastPos.x(), _lastPos.y());
}

void KSVDragListCanvas::setNewColor( const QColor& color ) {
  _newColor = color;
  draw();
}

void KSVDragListCanvas::setChangedColor( const QColor& color ) {
  _changedColor = color;
  draw();
}

void KSVDragListCanvas::reSort()
{
  list.reSort();
  draw();
}
