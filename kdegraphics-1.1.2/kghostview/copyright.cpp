/****************************************************************************
**
** A dialog which displays the terms of the GNU Public License.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "copyright.h"
#include "copyright.moc"

#include <qaccel.h>

#include <klocale.h>
#include <kapp.h>

CopyrightDialog::CopyrightDialog( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	setCaption(i18n("Copyright"));

	QFrame *frame = new QFrame ( this );
	frame->setGeometry( 0, 15, 505, 285 );
	frame ->setFrameStyle( QFrame::Panel | QFrame::Sunken);
   	frame ->setLineWidth(2);
	
	copyrightBox = new QMultiLineEdit( frame );
	copyrightBox->setGeometry( 2, 2, frame->width()-4,
	frame->height()-4 );
	copyrightBox->setFrameStyle( QFrame::NoFrame );
	copyrightBox->setFont( QFont("courier", 12) );
	copyrightBox->setReadOnly( TRUE );
	
	copyrightBox->setText(i18n("\n"
	"This program is free software; you can redistribute it and/or modify\n"\
	"it under the terms of the GNU General Public License as published by\n"\
	"the Free Software Foundation; either version 2 of the License, or\n"\
	"(at your option) any later version.\n"\
	"\n"\
	"This program is distributed in the hope that it will be useful,\n"\
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"\
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"\
	"GNU General Public License for more details.\n"\
	"\n"\
	"You should have received a copy of the GNU General Public License\n"\
	"along with this program; if not, write to the Free Software\n"\
	"Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA."));

	cancel = new QPushButton( this );
	cancel->setGeometry( 440, 315, 60, 30 );
	cancel->setText( i18n("Close") );
	cancel->setAutoDefault(TRUE);
	cancel->setFocus();
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );

	resize( 510, 350 );
	setMaximumSize( 510, 350 );
	setMinimumSize( 510, 350 );
	
}



