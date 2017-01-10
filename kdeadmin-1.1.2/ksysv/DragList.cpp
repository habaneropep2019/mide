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

#include <kmsgbox.h>
#include "DragList.h"
#include <kapp.h>

// include meta-object code
#include "DragList.moc"

#include "Constants.h"
#define SCROLLBAR_WIDTH 17
KSVDragList::KSVDragList( QWidget* parent, char* name ) : QFrame(parent, name) {
  // set Panel-Style
  if (kapp->style() == WindowsStyle)
    setFrameStyle( WinPanel | Sunken );
  else
    setFrameStyle( Panel | Sunken );
  setLineWidth(1);

  // initialize scrollbar
  bar = new QScrollBar(0, 0, 0, 0, 0, QScrollBar::Vertical, this, "scrollbar");
  connect(bar, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)));

  // initialize class members
  maxVisible = 10;
  curIndex = 0;
  offset = 0;
  //  defaultIcon = "unknown.xpm";
  viewPort = new KSVViewPort(this);
  viewPort->setBackgroundMode( PaletteBase );
  KDNDDropZone* zone = new KDNDDropZone(viewPort, DndText);
  connect(zone, SIGNAL(dropAction( KDNDDropZone* )), this, SLOT(slotRootDrop(KDNDDropZone*)));

  // dummy DropZone for preventing errors...
  KDNDDropZone* dummy_zone = new KDNDDropZone(this, DndText);
  dummy_zone->getAcceptType(); // to make the compiler happy...

  oldSliderValue = 0;

  // initialize "slider"
  movingPart = new KSVMovingPart(viewPort, "MOVINGPART");
  movingPart->setGeometry(0,0,80,800);

  // initialize canvas
  canvas = new KSVDragListCanvas(movingPart, "CANVAS");
  canvas->setRunlevel(QString(QWidget::name())); // use the widget name as "runlevel"
  connect(canvas, SIGNAL(sigResize(int, int)), movingPart, SLOT(slotResize(int, int)));

  connect(canvas, SIGNAL(sigChanged()), this, SLOT(updateBar()));
  // fwd sigChanged
  connect(canvas, SIGNAL(sigChanged()), this, SLOT(fwdChanged()));

  // fwd cannotGenerateNumber
  connect(canvas, SIGNAL(cannotGenerateNumber()), this, SLOT(fwdCannotGenerateNumber()));

  connect(canvas, SIGNAL(sigScrollUp(int)), this, SLOT(keyboardUp(int)));
  connect(canvas, SIGNAL(sigScrollDown(int)), this, SLOT(keyboardDown(int)));
  connect(canvas, SIGNAL(sigDroppedFrom(QString, KSVDragData*)), this, SLOT(slotDroppedFrom(QString, KSVDragData*)));
  connect(canvas, SIGNAL(sigDoubleClick(KSVDragListCanvas*, KSVDragData*, QPoint)), this, SLOT(doubleClickOnEntry(KSVDragListCanvas*, KSVDragData*, QPoint)));


  // capture mouse moves
  connect(canvas, SIGNAL(mouseMove(int,int)), this, SLOT(slotMouseMove(int,int)));

  // connect origin sigs & slots
  connect(canvas, SIGNAL(sigOrigin(bool)), this, SLOT(slotOrigin(bool)));
  connect(viewPort, SIGNAL(select(int)), canvas, SLOT(selectItem(int)));
  connect(canvas, SIGNAL(selected(const KSVDragData*)),
	  this, SLOT(fwdSelected(const KSVDragData*)));

  // set focusPolicy
  viewPort->setFocusProxy(canvas);
  viewPort->setFocusPolicy(ClickFocus);
  // I get a strange graphics error when I do not comment out the following statements
  //   bar->setFocusProxy(canvas);
  //   bar->setFocusPolicy(ClickFocus);
  this->setFocusProxy(canvas);
  this->setFocusPolicy(StrongFocus);

  // adjust size
  adjustSize();
}

KSVDragList::~KSVDragList() {

  // nothing yet
}

void KSVDragList::adjustSize() {
  const int vp_width = width() - bar->width() - 2 * BORDERWIDTH;
  const int vp_height = height() - 2 * BORDERWIDTH;
  viewPort->setGeometry( BORDERWIDTH, BORDERWIDTH, vp_width, vp_height );

  canvas->resize( vp_width, canvas->height() );
  movingPart->slotResize(canvas->width(), canvas->height());

  bar->setGeometry( width() - BORDERWIDTH - SCROLLBAR_WIDTH,
		    BORDERWIDTH,
		    SCROLLBAR_WIDTH,
		    vp_height );

  // reset maxVisible to correct value
  maxVisible = viewPort->height() / ENTRY_HEIGHT;
  updateBar();
}

void KSVDragList::updateBar() {
  if ( canvas->count() > maxVisible) {
    bar->setRange(0, canvas->height() - viewPort->height() );
    bar->setSteps(ENTRY_HEIGHT, maxVisible*ENTRY_HEIGHT);
  } else {
    bar->setRange(0, 0);
    bar->setSteps(0, 0);
  }
}

void KSVDragList::setDefaultIcon( QString _name) {
  //  defaultIcon = _name;
  canvas->setDefaultIcon(_name);
}

void KSVDragList::clear() {
  // keep GUI alive
  qApp->processEvents();
  canvas->clear();
  clearDeletedItems();
}

void KSVDragList::slotValueChanged( int _newValue ) {
  movingPart->scroll(0, oldSliderValue - _newValue);
  oldSliderValue = _newValue;
}

void KSVDragList::initItem( const char* _file, const char* _path, const char* _name, int _nr ) {
  KSVDragData* data = new KSVDragData();
  data->setFilename(_file);
  data->setPath(_path);
  data->setRunlevel(canvas->getRunlevel());
  data->setName(_name);
  data->setNumber(_nr);
  data->setChanged(FALSE);

  canvas->insertItem(count(), data);
}

void KSVDragList::insertItem(KSVDragData* _w, int _pos, const bool genNr) {
  canvas->insertItem(_pos, _w, genNr);
}

void KSVDragList::removeItem(KSVDragData* _w) {
  if (0 > deletedItems.find(_w))
    deletedItems.append(new KSVDragData(*canvas->at(canvas->find(_w))));

  canvas->removeItem(_w);
}

void KSVDragList::slotRootDrop( KDNDDropZone* _zone ) {
  if (_zone->getAcceptType() == _zone->getDataType() ) {
    const QString droppedData = _zone->getData();

    KSVDragData* newO = new KSVDragData();
    newO->fromString(droppedData);

    const int position = canvas->count();

    if (!canvas->insertOK(newO, position)) {
      delete newO;
      return;
    }

    // for removing elsehwere
    const QString _old_rl = newO->runlevel();

    // change sort number
    const int temp = canvas->generateNumber(position);
    if (temp == -1) {
      delete newO;
      return;
    } else
      newO->setNumber(temp);

    canvas->insertItem(position, newO);

    if (_old_rl != canvas->getRunlevel())
      emit sigDroppedFrom(_old_rl, newO);

  }
}

void KSVDragList::keyboardUp(int _focus) {
  if ( (_focus + 1 - VISIBILITY_OFFSET <= offset) && (offset > 0) ) {
    bar->setValue(bar->value() - ENTRY_HEIGHT);
    offset--;
  }
}

void KSVDragList::keyboardDown(int _focus) {
  if ( (_focus + VISIBILITY_OFFSET >= offset + maxVisible) && (offset + maxVisible < canvas->count()) ) {
    bar->setValue(bar->value() + ENTRY_HEIGHT);
    offset++;
  }
}

void KSVDragList::slotRemoveAfterDrop(QString _parent, KSVDragData* _o) {
  if ( _parent == name() ) {
    deletedItems.append(new KSVDragData(*canvas->at(canvas->find(_o))));

    removeItem(_o);
    delete _o;
  }
}

void KSVDragList::slotDroppedFrom(QString _par, KSVDragData* _o) {
  emit sigDroppedFrom(_par, _o);
}

void KSVDragList::doubleClickOnEntry(KSVDragListCanvas*, KSVDragData* _data, QPoint _pos) {
  emit sigDoubleClick(_data,_pos);
}

void KSVDragList::autoScroll( int _x, int _y ) {
  const QPoint globalPos = QPoint( _x, _y );
  const QPoint localPos = parentWidget()->mapFromGlobal(globalPos);
  const int lx = localPos.x();
  const int ly = localPos.y();

  if ( (x() <= lx) && (lx <= (x() + width())) ) {
    if ( (y() <= ly) && (ly <= (y() + height())) ) {

      const int downVal = y() + height() - ly;
      const int upVal = ly - y();
      const int scrollArea = (int)(1.5 * ENTRY_HEIGHT);
      const int amount = ENTRY_HEIGHT;

      if ( (0 <= downVal) && (downVal <= scrollArea) ) {
  	// scroll down
  	bar->setValue(bar->value() + amount);
	if (offset < (canvas->count() - maxVisible))
	  offset++;  	
      } else if ( (0 <= upVal) && (upVal <= scrollArea) ) {
  	// scroll up
  	bar->setValue(bar->value() - amount);
	if (offset > 0)
	  offset--;
      }
    }
  }
}

void KSVDragList::slotMouseMove( int _x, int _y ) {
  emit mouseMove( _x, _y);
}

void KSVDragList::slotOrigin( bool val ) {
  emit sigOrigin( val );
}

void KSVDragList::setOrigin( bool val ) {
  canvas->setOrigin(val);
  if (!val)
    canvas->update();
}

void KSVDragList::resizeEvent( QResizeEvent* ) {
  adjustSize();
}

void KSVDragList::addPMenu( QPopupMenu* menu ) {
  viewPort->addPMenu( menu );
}

void KSVDragList::addItemPMenu( QPopupMenu* menu ) {
  canvas->addPMenu( menu );
}

const int KSVDragList::count() const {
  return canvas->count();
}

void KSVDragList::fwdChanged() {
  // inform the parent that some change has occured
  // (moved from "updateBar")
  emit sigChanged();
}

const bool KSVDragList::isOrigin() const {
  return canvas->isOrigin();
}

void KSVDragList::clearDeletedItems() {
  deletedItems.setAutoDelete(TRUE);
  deletedItems.clear();
  deletedItems.setAutoDelete(FALSE);
}

KSVList KSVDragList::getDeletedItems() {
  return deletedItems;
}

void KSVDragList::fwdCannotGenerateNumber() {
  emit cannotGenerateNumber();
}

KSVDragData* KSVDragList::at( const int index ) {
  return canvas->at(index);
}

QSize KSVDragList::sizeHint() const {
  const int w = SCROLLBAR_WIDTH + 2 * BORDERWIDTH + 40;
  const int h = 40;

  return QSize(w,h);
}

KSVDragData* KSVDragList::selected() {
  return canvas->at(canvas->focusItem());
}

void KSVDragList::setNewColor( const QColor& color ) {
  canvas->setNewColor(color);
}

void KSVDragList::setChangedColor( const QColor& color ) {
  canvas->setChangedColor(color);
}

int KSVDragList::focusItem() const
{
  return canvas->focusItem();
}

int KSVDragList::find( const KSVDragData* data ) const
{
  return canvas->find(data);
}

void KSVDragList::redraw( int start, int end )
{
  canvas->draw( start, end );
}

void KSVDragList::fwdSelected( const KSVDragData* d )
{
  emit selected(d);
}

void KSVDragList::reSort() 
{
  canvas->reSort();
}
