#include "aboutDlg.h"
#include <kapp.h>
#include <qlistbox.h>
#include <qpushbt.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qlabel.h>
#include <kiconloader.h>

#define Inherited QDialog

#include "tlform.h"
#include "typelayout.h"

#include "aboutDlg.moc"

aboutDlg::aboutDlg
    (
     QWidget* parent,
     const char* name
    )
    :
    Inherited( parent, name, TRUE )
{

    setBackgroundColor(QColor("white"));
    TLForm *f=new TLForm("about",
                         klocale->translate("About KRN"),
                         this);
    
    KTypeLayout *l=f->layout;

    QPixmap p=kapp->getIconLoader()->loadIcon("logo.xpm");

    l->addButton ("logo",p);
    l->newLine();
    l->addLabel("l3","0.6.0");
    l->newLine();
    l->addLabel("l1",klocale->translate("Part of the KDE project."));
    l->newLine();

    l->addLabel("l2",klocale->translate("Credits & Thanks") );
    l->newLine();

    
    QListBox* tmpQListBox=(QListBox*)(l->addListBox("list")->widget);
    QColorGroup normal( ( QColor( QRgb(16711680) ) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(255) ), QColor( QRgb(16777215) ) );
    QColorGroup disabled( ( QColor( QRgb(8421504) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
    QColorGroup active( ( QColor( QRgb(0) ) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
    QPalette palette( normal, disabled, active );
    tmpQListBox->setPalette( palette );
    tmpQListBox->setFrameStyle( 51 );
    tmpQListBox->setLineWidth( 2 );
    tmpQListBox->insertItem( "Roberto Alsina <ralsina@unl.edu.ar>" );
    tmpQListBox->insertItem( "Magnus Reftel <d96reftl@dtek.chalmers.se>" );
    tmpQListBox->insertItem( "Sander Alberink <sander.alberink@bigfoot.com>" );
    tmpQListBox->insertItem( "Stefan Taferner <taferner@alpin.or.at>" );
    tmpQListBox->insertItem( "Robert Cope <robert@bga.com>" );
    tmpQListBox->insertItem( "Nico Schirwing" );
    tmpQListBox->insertItem( "César Ballardini <cballard@santafe.com.ar>" );
    tmpQListBox->insertItem( "Jacek Konieczny <jajcus@zeus.polsl.gliwice.pl>" );
    tmpQListBox->insertItem( "Hans Zoebelein <zocki@goldfish.cube.net>" );
    tmpQListBox->insertItem( "Johannes Sixt <Johannes.Sixt@telecom.at>" );
    tmpQListBox->insertItem( "Torsten Neumann <torsten@londo.rhein-main.de>" );
    tmpQListBox->insertItem( "" );
    tmpQListBox->insertItem( klocale->translate("And Many thanks to:") );
    tmpQListBox->insertItem( "" );
    tmpQListBox->insertItem( "Doug Sauder <dwsauder@fwb.gulf.net>" );
    tmpQListBox->insertItem( "Stephan Kulow <coolo@kde.org>" );
    tmpQListBox->insertItem( "Bernd Wuebben <wuebben@math.cornell.edu>" );
    tmpQListBox->insertItem( i18n("All KDE developers") );
    tmpQListBox->insertItem( "Troll Tech" );
    tmpQListBox->insertItem( i18n("Chuck, for helping in time of need") );
    tmpQListBox->insertItem( i18n("All testers") );
    tmpQListBox->setScrollBar( TRUE );

    l->newLine();
    QPushButton *b1=(QPushButton *)(l->addButton("b1",klocale->translate("OK"))->widget);

    l->findWidget("l1")->setBackgroundColor("white");
    l->findWidget("l2")->setBackgroundColor("white");
    l->findWidget("l3")->setBackgroundColor("white");
    l->setAlign("l1",AlignLeft|AlignRight);
    l->setAlign("l2",AlignLeft|AlignRight);
    l->setAlign("l3",AlignLeft|AlignRight);
    ((QLabel *)l->findWidget("l1"))->setAlignment(AlignHCenter);
    ((QLabel *)l->findWidget("l2"))->setAlignment(AlignHCenter);
    ((QLabel *)l->findWidget("l3"))->setAlignment(AlignHCenter);
    
    l->setAlign("list",AlignLeft|AlignRight|AlignTop|AlignBottom);
    l->setAlign("b1",AlignLeft|AlignRight);
    
    l->activate();

    b1->setDefault(true);
    connect ((QPushButton *)l->findWidget("logo"),SIGNAL(clicked()),
             b1,SIGNAL(clicked()));
    connect(b1,SIGNAL(clicked()),SLOT(accept()));

    resize(350,200);
}


aboutDlg::~aboutDlg()
{
}
