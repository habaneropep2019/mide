/*

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997-99 Peter Putzer
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

#include <qlabel.h>
#include <qstring.h>
#include <qpushbt.h>
#include <qlayout.h>
#include <kfm.h>
#include "About.h"
#include "ksv_core.h"
#include <kurllabel.h>
#include <kapp.h>

// include meta-object code
#include "About.moc"

// some constants
#include "version.h"
#define FULL 10
#define HALF 5
#define MINIMAL 2
#define MIN_SIZE(A) A->setMinimumSize(A->sizeHint())

AboutDlg::AboutDlg( QWidget* parent) : QDialog(parent, "AboutDialog", TRUE) {
  setCaption(QString().sprintf(i18n("About %s"),
			       KSVCore::getAppName().data()));

  KIconLoader* kil = kapp->getIconLoader();

  QBoxLayout* topLayout = new QVBoxLayout(this, FULL);

  QFont large_bold_font = kapp->generalFont;
  large_bold_font.setBold(TRUE);
  large_bold_font.setPointSize( 24 );

  QFont small_bold_font = kapp->generalFont;
  small_bold_font.setBold(TRUE);
  small_bold_font.setPointSize( 10 );

  QFont normal_bold_font = kapp->generalFont;
  normal_bold_font.setBold(TRUE);

  QLabel* title = new QLabel( i18n("KDE SysV Init Editor"), this);
  title->setFont(large_bold_font);
  title->setAlignment(AlignCenter);
  MIN_SIZE(title);
  topLayout->addWidget(title);

  QBoxLayout* picLayout = new QHBoxLayout(0);
  topLayout->addLayout(picLayout);
  QLabel* pic = new QLabel(this);
  pic->setPixmap( kil->loadIcon("ksysv_logo.xpm") );
  MIN_SIZE(pic);
  picLayout->addWidget(pic, MINIMAL);

  QBoxLayout* versionLayout = new QVBoxLayout();
  picLayout->addLayout(versionLayout);
  picLayout->addStretch(FULL);

  QLabel* version = new QLabel( QString().sprintf(i18n("Version %s (KDE %s)"), KSYSV_VERSION, KDE_VERSION_STRING), this );
  version->setFont(normal_bold_font);
  MIN_SIZE(version);
  versionLayout->addWidget(version);

  QBoxLayout* copyLayout = new QHBoxLayout(0);
  versionLayout->addLayout(copyLayout);

  QLabel* copyright1 = new QLabel( i18n("© 1997-1999 "), this);
  copyright1->setFont(normal_bold_font);
  copyright1->setAlignment( AlignLeft | AlignVCenter | WordBreak );
  MIN_SIZE(copyright1);
  copyLayout->addWidget(copyright1);

  QBoxLayout* authorLayout = new QVBoxLayout(0);
  copyLayout->addLayout(authorLayout);
  authorLayout->addSpacing(3);
  KURLLabel* copyright2 = new KURLLabel(this, "CopyrightLabel");
  copyright2->setText(i18n("Peter Putzer"));
  copyright2->setURL("mailto:putzer@kde.org");
  copyright2->setFloat(true);
  copyright2->setUnderline(false);
  copyright2->setTipText(i18n("Mail bug report/success story/lots of money/..."));
  copyright2->setUseTips(TRUE);
  copyright2->setFont(normal_bold_font);
  copyright2->setAlignment( AlignLeft | AlignVCenter | WordBreak );

  // react on click
  connect(copyright2, SIGNAL(leftClickedURL(const char*)), this, SLOT(callURL(const char*)));

  // insert into layout
  MIN_SIZE(copyright2);
  authorLayout->addWidget(copyright2);

  QLabel* copyright3 = new QLabel( i18n(". All rights reserved."), this);
  copyright3->setFont(normal_bold_font);
  copyright3->setAlignment( AlignLeft | AlignVCenter | WordBreak );
  MIN_SIZE(copyright3);
  copyLayout->addWidget(copyright3);
  copyLayout->addStretch(FULL);

  QLabel* additionalCopyright = new QLabel( i18n("IPC parts taken from kdehelp © 1997 Martin Jones.\n" \
						 "Uses the KDE libraries © 1997-1999 The KDE Team.\n"
						 "Uses the Qt library © 1992-1998 Troll Tech AS."), this);
  additionalCopyright->setFont(small_bold_font);
  MIN_SIZE(additionalCopyright);
  topLayout->addWidget(additionalCopyright);

  QLabel* description = new QLabel( i18n("An editor for SysV-style init configuration, similar to Red Hat's \"tksysv\",\n" \
					 "only ksysv allows drag-and-drop, as well as keyboard use."), this );

  description->setAlignment( AlignLeft | AlignVCenter | WordBreak );
  description->setMargin(FULL);
  description->setFrameStyle( QFrame::Box | QFrame::Sunken );
  MIN_SIZE(description);
  topLayout->addWidget(description, FULL);

  // this is optimized for a 10 point bold helvetica
  // unfortunately I don't know how to get reasonbly minimum sizes
  // for using layout-management and WordBreak, we gain nothing by using
  // WordBreak here (or above)... :(
  QLabel* license = new QLabel( i18n("This program is free software; you can redistribute it and/or modify it under the\n" \
				     "terms of version 2 of the GNU General Public License as published by the Free\n" \
				     "Software Foundation." \
				     "\n\n" \
				     "This program is distributed in the hope that it will be useful, but WITHOUT ANY\n" \
				     "WARRANTY; without even the implied warranty of MERCHANTABILITY or\n" \
				     "FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License\n" \
				     "for more details."), this);

  license->setAlignment( AlignLeft | AlignVCenter | WordBreak );
  license->setFont(small_bold_font);
  license->setFrameStyle(QFrame::Box | QFrame::Sunken);
  license->setMargin(FULL);
  MIN_SIZE(license);

  topLayout->addWidget(license, FULL);

  topLayout->addSpacing(HALF);

  QPushButton* ok = new QPushButton( i18n("OK"), this);
  KSVCore::goodSize(ok);
  ok->setFixedSize(ok->size());
  topLayout->addWidget(ok);

  // connect stuff and set as default
  connect( ok, SIGNAL(clicked()), this, SLOT(accept()) );
  ok->setDefault(TRUE);
  ok->setAutoDefault(TRUE);
  ok->setFocus();

  topLayout->activate();

  // setFixedSize
  topLayout->freeze(0,0);
}

AboutDlg::~AboutDlg() {
  // nothing
}

void AboutDlg::callURL( const char* url ) {
  KFM kfm;
  kfm.allowKFMRestart(false); // not good when started by non-root user
                              // (via su or sudo for example)
  kfm.openURL(url);
}
