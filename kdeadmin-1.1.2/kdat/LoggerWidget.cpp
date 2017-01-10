// KDat - a tar-based DAT archiver
// Copyright (C) 1998  Sean Vyain, svyain@mail.tds.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <qfiledlg.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmlined.h>

#include <kapp.h>
#include <kmsgbox.h>

#include "LoggerWidget.h"

#include "LoggerWidget.moc"

LoggerWidget::LoggerWidget( const char* title, QWidget* parent, const char* name )
        : QWidget( parent, name )
{
    QLabel* lbl1 = new QLabel( title, this );
    lbl1->setFixedHeight( lbl1->sizeHint().height() );

    _mle = new QMultiLineEdit( this );
    _mle->setReadOnly( TRUE );

    QVBoxLayout* l1 = new QVBoxLayout( this, 0, 4 );
    l1->addWidget( lbl1 );
    l1->addWidget( _mle, 1 );
}

LoggerWidget::~LoggerWidget()
{
}

void LoggerWidget::append( const char* text )
{
    _mle->append( text );
    _mle->setCursorPosition( _mle->numLines(), 0 );
}

void LoggerWidget::save()
{
    QString file = QFileDialog::getSaveFileName( 0, 0, this );
    if ( !file.isNull() ) {
        QFile f( file );
        if ( f.exists() ) {
            if ( KMsgBox::yesNo( this, i18n( "KDat: Save Log" ), i18n( "Log file exists, overwrite?" ), KMsgBox::EXCLAMATION ) != 1 ) {
                return;
            }
        }
        if ( f.open( IO_WriteOnly ) ) {
            const char* line;
            for ( int i = 0; i < _mle->numLines(); i++ ) {
                line = _mle->textLine( i );
                f.writeBlock( line, strlen( line ) );
                f.writeBlock( "\n", 1 );
            }
            f.close();
        }
    }
}
