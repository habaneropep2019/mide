
/*

 $Id$

 Copyright (C) Bernd Johannes Wuebben
               wuebben@math.cornell.edu
	       wuebben@kde.org

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 */

#include "urldlg.h"
#include <klocale.h>
#include "urldlg.moc"

#define klocale KApplication::getKApplication()->getLocale()



UrlDlg::UrlDlg( QWidget *parent,  const char* caption, const char* urltext)
	: QDialog( parent, "urldialog", TRUE )
{
	frame = new QGroupBox( caption, this );
	lineedit = new QLineEdit( this );
	lineedit->setText(urltext);

	this->setFocusPolicy( QWidget::StrongFocus );
	connect(lineedit, SIGNAL(returnPressed()), this, SLOT(accept()));

	ok = new QPushButton(klocale->translate("OK"), this );
	cancel = new QPushButton(klocale->translate("Cancel"), this ); 

	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
	resize(375, 120); 

}


void UrlDlg::resizeEvent(QResizeEvent *)
{
    frame->setGeometry(5, 5, width() - 10, 80);
    cancel->setGeometry(width() - 80, height() - 30, 70, 25);
    ok->setGeometry(10, height() - 30, 70, 25);
    lineedit->setGeometry(20, 35, width() - 40, 25);
}

void UrlDlg::focusInEvent( QFocusEvent *)
{
    lineedit->setFocus();

}

const char* UrlDlg::getText()
{
	return lineedit->text();
}
