// -*- C++ -*-

//
//  kjots
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.ivm.de or chris@kde.org
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <qframe.h>
#include <qlayout.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlined.h>

#include <kapp.h>
#include <ktabctl.h>
#include <kfontdialog.h>

#include "confdialog.h"

ConfDialog::ConfDialog ()
{
  initMetaObject();
  setCaption(klocale->translate("Configure KJots"));
  tabbar = new KTabCtl(this);
  tabbar->setGeometry(10,10, 300, 200);
  QFrame *f_url = new QFrame(tabbar);
  tabbar->addTab(f_url, klocale->translate("URL"));
  QFrame *f_fonts = new QFrame(tabbar);
  tabbar->addTab(f_fonts, klocale->translate("Fonts"));

  b_ok = new QPushButton(klocale->translate("OK"), this);
  b_ok->setFixedSize(80, 24);
  b_apply = new QPushButton(klocale->translate("Apply"), this);
  b_apply->setFixedSize(80, 24);
  b_cancel = new QPushButton(klocale->translate("Cancel"), this);
  b_cancel->setFixedSize(80, 24);

  QGridLayout *l_top_down = new QGridLayout(this, 2, 1, 4);
  QBoxLayout *l_buttons = new QBoxLayout(QBoxLayout::LeftToRight);
  l_top_down->addWidget(tabbar, 0, 0);
  l_top_down->setRowStretch(0, 1);
  l_top_down->addLayout(l_buttons, 1, 0);
  l_buttons->addStretch(1);
  l_buttons->addWidget(b_ok);
  l_buttons->addStretch(1);
  l_buttons->addWidget(b_apply);
  l_buttons->addStretch(1);
  l_buttons->addWidget(b_cancel);
  l_buttons->addStretch(1);
  l_top_down->activate();

  // generate URL dialog
  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( f_url, "Label_1" );
  tmpQLabel->setGeometry( 8, 16, 208, 24 );
  tmpQLabel->setText( klocale->translate("Specify which command to execute") );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  tmpQLabel = new QLabel( f_url, "Label_2" );
  tmpQLabel->setGeometry( 16, 48, 56, 24 );
  tmpQLabel->setText( klocale->translate("for HTTP:") );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );
  
  tmpQLabel = new QLabel( f_url, "Label_3" );
  tmpQLabel->setGeometry( 24, 88, 48, 24 );
  tmpQLabel->setText( klocale->translate("for FTP:") );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );
  
  i_http = new QLineEdit( f_url, "LineEdit_1" );
  i_http->setGeometry( 80, 48, 232, 24 );
  i_http->setText( "" );
  i_http->setMaxLength( 32767 );
  i_http->setEchoMode( QLineEdit::Normal );
  i_http->setFrame( TRUE );

  i_ftp = new QLineEdit( f_url, "LineEdit_2" );
  i_ftp->setGeometry( 80, 88, 232, 24 );
  i_ftp->setText( "" );
  i_ftp->setMaxLength( 32767 );
  i_ftp->setEchoMode( QLineEdit::Normal );
  i_ftp->setFrame( TRUE );

  tmpQLabel = new QLabel( f_url, "Label_4" );
  tmpQLabel->setGeometry( 16, 120, 240, 24 );
  tmpQLabel->setText( klocale->translate("( %u is replaced with the selected URL )") );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  // generate fonts dialog
  tmpQLabel = new QLabel( f_fonts, "Label_1" );
  tmpQLabel->setGeometry( 8, 16, 80, 24 );
  tmpQLabel->setText( klocale->translate("Font for editor:") );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );

  b_sel_ed_font = new QPushButton( f_fonts, "PushButton_1" );
  b_sel_ed_font->setGeometry( 168, 64, 100, 32 );
  b_sel_ed_font->setText( klocale->translate("Select") );
  b_sel_ed_font->setAutoRepeat( FALSE );
  b_sel_ed_font->setAutoResize( FALSE );
  connect(b_sel_ed_font, SIGNAL(clicked()), this, SLOT(selectEdFont()) );
  
  l_ed_font = new QLabel( f_fonts, "Label_6" );
  l_ed_font->setGeometry( 16, 48, 128, 64 );
  {
    QColorGroup normal( ( QColor( QRgb(0) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
    QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
    QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
    QPalette palette( normal, disabled, active );
    l_ed_font->setPalette( palette );
  }
  l_ed_font->setFrameStyle( 51 );
  l_ed_font->setLineWidth( 2 );
  l_ed_font->setText( klocale->translate("Dolor Ipse") );
  l_ed_font->setAlignment( 292 );
  l_ed_font->setMargin( -1 );

  readConfig();

  connect( b_ok, SIGNAL(clicked()), this, SLOT(ok()) );
  connect( b_apply, SIGNAL(clicked()), this, SLOT(apply()) );
  connect( b_cancel, SIGNAL(clicked()), this, SLOT(cancel()) );

  setMinimumSize(350, 250);
  resize(350, 250);
}

void ConfDialog::readConfig()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("kjots");
  i_http->setText(config->readEntry("execHttp"));
  i_ftp->setText(config->readEntry("execFtp"));
  QFont font( config->readEntry("EFontFamily"), config->readNumEntry("EFontSize"),
	      config->readNumEntry("EFontWeight"), config->readNumEntry("EFontItalic"),
	      (QFont::CharSet) config->readNumEntry("EFontCharset") );
  l_ed_font->setFont(font);
}

void ConfDialog::writeConfig()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("kjots");
  config->writeEntry( "execHttp", i_http->text() );
  config->writeEntry( "execFtp", i_ftp->text() );
  QFont font = l_ed_font->font();
  config->writeEntry("EFontFamily", font.family());
  config->writeEntry("EFontSize", font.pointSize());
  config->writeEntry("EFontWeight", font.weight());
  config->writeEntry("EFontItalic", font.italic());
  config->writeEntry("EFontCharset", font.charSet());
}

void ConfDialog::ok()
{
  writeConfig();
  emit commitChanges();
  emit confHide();
}

void ConfDialog::apply()
{
  writeConfig();
  emit commitChanges();
}

void ConfDialog::cancel()
{
  emit confHide();
}

void ConfDialog::selectEdFont()
{
  QFont font = l_ed_font->font();
  KFontDialog::getFont(font);
  l_ed_font->setFont(font);
}

void ConfDialog::closeEvent(QCloseEvent *)
{
  emit confHide();
}

