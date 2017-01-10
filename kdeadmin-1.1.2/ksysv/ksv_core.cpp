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

// KSysV Core

#include <qbitmap.h>
#include <qpainter.h>
#include "ksv_core.h"
#include "DragListCanvas.h"

// include meta-object code
#include "ksv_core.moc"

// some macros
#define EXTRA_WIDTH 20

KSVCore::KSVCore() : QObject(0, "KSVCore") {
}

// Destructor
KSVCore::~KSVCore() {
} 

void KSVCore::msg(QString _text) {
  QMessageBox::warning(kapp->mainWidget(), i18n("Warning"), _text.data());     
}

const QString KSVCore::getAppName() {
  QString _name = kapp->getCaption();
  if (_name == "ksysv")
    _name = QString("SysV Init Editor");
  return _name;
}

QPixmap KSVCore::drawDragIcon( QPixmap icon, QBitmap maskIcon, QString overlay, KSVDragData* data ) {
  KIconLoader* kil = kapp->getIconLoader();

  QPainter p;
  QString title;
  if (data)
    title = data->currentName();
  else
    title = "";

  p.begin(&icon); // QFontMetric doesn´t like being used outside anymore
  int w = p.fontMetrics().width( title.data() ) + EXTRA_WIDTH;
  p.end();

  QBitmap mask( w, icon.height(), TRUE);
  QPixmap dragIcon( w, icon.height() );
  QPixmap overlayIcon;
  QBitmap overlayMono;

  if (!overlay.isEmpty()) {
    overlayIcon = kil->loadIcon( QString().sprintf("%s.xpm", overlay.data()) );
    overlayMono = kil->loadIcon( QString().sprintf("mono_%s.xpm", overlay.data()) );
  }

  dragIcon.fill( black );

  p.begin( &mask );
  p.setPen( color1 );
  p.drawPixmap( 0, 0, maskIcon );
  p.drawPixmap( 0, icon.height() - overlayMono.height(), overlayMono );
  p.drawText( icon.width() + 2, icon.height() - 4, title.data() );
  p.end();
  
  p.begin( &dragIcon );
  p.drawPixmap( 0, 0, icon );
  p.drawPixmap( 0, 0, overlayIcon);
  p.end();
  
  dragIcon.setMask( mask );
 
  return dragIcon;
}

QPixmap KSVCore::drawDragMulti( QPixmap icon, QString overlay ) {
  KIconLoader* kil = kapp->getIconLoader();

  QPainter p;

  QPixmap dragIcon( icon );
  QPixmap overlayIcon;

  if (!overlay.isEmpty()) {
    overlayIcon = kil->loadIcon( QString().sprintf("%s.xpm", overlay.data()) );
  }
  
  p.begin( &dragIcon );
  //  p.drawPixmap( 0, 0, icon );
  p.drawPixmap( 0, 0, overlayIcon);
  p.end();

  return dragIcon;
}

KSVClipBoard* KSVCore::clipboard() {
  static KSVClipBoard clip;

  return &clip;
}

KSVConfig* KSVCore::getConfig() {
  static KSVConfig conf;

  return &conf;
}

void KSVCore::setHelpTopic( const char* sect, const char* label ) {
  section = sect;
  topic = label;
}

QString KSVCore::helpTopic() {
  return topic;
}

void KSVCore::invokeHelp( const bool withTopic ) {
  if (withTopic)
    kapp->invokeHTMLHelp( section, topic );
  else
    kapp->invokeHTMLHelp( "ksysv/ksysv.html", QString() );
}

QString KSVCore::helpSection() {
  return section;
}

KSVCore* KSVCore::getCore() {
  static KSVCore c;
  
  return &c;
}

void KSVCore::goodSize( QPushButton* button ) {
  const int w = button->sizeHint().width();
  const int h = button->sizeHint().height();

  button->setMinimumSize( (w<75) ? 75 : w, (h<25) ? 25 : h);
}
