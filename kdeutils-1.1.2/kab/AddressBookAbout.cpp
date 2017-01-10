/* -*- C++ -*-
 * This file implements the about-dialog.
 * 
 * the KDE addressbook.
 * copyright:  (C) Mirko Sucker, 1998, 1999
 * license:    GNU Public License, Version 2
 * mail to:    Mirko Sucker <mirko.sucker@unibw-hamburg.de>
 * requires:   C++-compiler, STL, string class,
 *             Nana for debugging
 * $Revision: 1.2.2.1 $
 */

#include "AddressBookAbout.h"
#include <qpixmap.h>
#include <qmsgbox.h>
#include <string>
#include <kapp.h>
#include "debug.h"

AddressBookAbout::AddressBookAbout(QWidget* parent, const char* name)
  : AddressBookAboutData(parent, name)
{
  // ############################################################################
  const string AddressBookLogo="addressbook_logo.jpg";
  setCaption(i18n("About addressbook"));
  QPixmap pixmap;
  string path;
  path=KApplication::getKApplication()->kde_datadir();
  path+=(string)"/kab/pics/"+AddressBookLogo;
  // this needs the installed JPEG reader:
  if(!pixmap.load(path.c_str()))
    {
      QMessageBox::information
	(parent, // we are invisible here!
	 i18n("Image load failure"),
	 i18n("Could not load addressbook logo image!"));
      labelLogo->setText(i18n("Here you would see\nthe addressbook logo."));
    } else {
      labelLogo->setPixmap(pixmap);
      ix=pixmap.width(); iy=pixmap.height();
    }
  buttonOK->setText(i18n("OK"));
  buttonOK->setFocus();
  labelAuthor->setText(i18n("written and maintained by")); 
  urlName->setText("mirko@kde.org");
  urlName->setAlignment(AlignCenter);
  // -----
  connect(urlName, SIGNAL(leftClickedURL(const char*)), SLOT(mail(const char*)));
  initializeGeometry();
  // ############################################################################
}


AddressBookAbout::~AddressBookAbout()
{
}

void AddressBookAbout::initializeGeometry()
{
  // ############################################################################
  const int Grid=5;
  int tempx, tempy, cx, cy;
  // ----- first determine the size of the label containing the logo:
  tempx=6+ix;
  tempy=6+iy;
  // ----- then determine dialog width:
  cx=QMAX(labelVersion->sizeHint().width(), labelAuthor->sizeHint().width());
  cx=QMAX(tempx, cx)+4*Grid;
  cy=2*Grid;
  // ----- now show image, version and author label:
  labelLogo->setGeometry((cx-tempx)/2, cy, ix+6, iy+6);
  cy+=Grid+labelLogo->height();
  labelVersion->setGeometry
    (2*Grid, cy, cx-4*Grid, labelVersion->sizeHint().height());
  cy+=Grid+labelVersion->sizeHint().height();
  labelAuthor->setGeometry
    (2*Grid, cy, cx-4*Grid, labelAuthor->sizeHint().height());
  cy+=Grid+labelAuthor->sizeHint().height();
  urlName->setGeometry(2*Grid, cy, cx-4*Grid, labelAuthor->sizeHint().height());
  cy+=Grid+labelAuthor->sizeHint().height();
  // ----- now we know how to resize the inner frame:
  labelFrame->setGeometry(Grid, Grid, cx-2*Grid, cy-Grid);
  cy+=Grid;
  // ----- now resize the button:
  buttonOK->setGeometry(Grid, cy, cx-2*Grid, buttonOK->sizeHint().height());
  cy+=Grid+buttonOK->sizeHint().height();
  frameBase->setGeometry(0, 0, cx, cy);
  setFixedSize(cx, cy);
  // ############################################################################
}
  
void AddressBookAbout::mail(const char* address)
{
  // ############################################################################
  emit(sendEmail(address));
  // ############################################################################
}
#include "AddressBookAbout.moc"
#include "AddressBookAboutData.moc"

