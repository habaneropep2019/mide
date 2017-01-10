#include "fontsDlg.h"

#define Inherited QDialog

#include <kapp.h>
#include <kconfig.h>
#include <qfont.h>
#include <qstrlist.h>
#include <X11/Xlib.h>


#include "tlform.h"
#include "typelayout.h"

#include "fontsDlg.moc"

//Prototypes for utility functions
void addFont( QStrList &list, const char *xfont );
void getFontList( QStrList &list, const char *pattern );


extern KConfig *conf;

fontsDlg::fontsDlg(QWidget* parent,const char* name):Inherited( parent, name, TRUE )
{


    QStrList *stdfl=new QStrList;
    QStrList *fixedfl=new QStrList;
    getFontList( *stdfl, "-*-*-*-*-*-*-*-*-*-*-p-*-*-*" );
    //I add these, because some people may prefer all-fixed width fonts
    //(for ascii art, I suppose)
    getFontList( *stdfl, "-*-*-*-*-*-*-*-*-*-*-m-*-*-*" );
    getFontList( *fixedfl, "-*-*-*-*-*-*-*-*-*-*-m-*-*-*" );



    TLForm *f=new TLForm("expiration",
                         klocale->translate("Message Display Settings"),
                         this);

    KTypeLayout *l=f->layout;

    l->addGroup("entries","",true);

    l->addLabel("l7",klocale->translate("Header Style"));
    QStrList *hdrstyles=new QStrList;
    hdrstyles->append(klocale->translate("Fancy"));
    hdrstyles->append(klocale->translate("Brief"));
    hdrstyles->append(klocale->translate("Standard"));
    hdrstyles->append(klocale->translate("Long"));
    hdrstyles->append(klocale->translate("All"));
    hdrstyle=(QComboBox *)(l->addComboBox("hdrstyle",hdrstyles)->widget);
    delete hdrstyles;
    l->newLine();
    l->addLabel("l7",klocale->translate("Attachment Style"));
    QStrList *atmstyles=new QStrList;
    atmstyles->append(klocale->translate("Iconic"));
    atmstyles->append(klocale->translate("Smart"));
    atmstyles->append(klocale->translate("Inline"));
    atmstyle=(QComboBox *)(l->addComboBox("atmstyle",atmstyles)->widget);
    delete atmstyles;
    l->newLine();

    conf->setGroup("ArticleListOptions");
    
    l->addLabel("l1",klocale->translate("Font Size:"));

    QStrList *sizel=new QStrList;
    sizel->append(klocale->translate( "Small") );
    sizel->append(klocale->translate( "Normal" ));
    sizel->append(klocale->translate( "Large" ));
    sizel->append(klocale->translate( "Huge" ));
    fontSize=(QComboBox *)(l->addComboBox("fontSize",sizel)->widget);
    delete sizel;
    
    l->newLine();
    l->addLabel("l2", klocale->translate("Standard Font"));
    stdFontName=(QComboBox *)(l->addComboBox("stdFontName",stdfl)->widget);
    l->newLine();
    l->skip();
    samp1=(QLineEdit *)(l->addLineEdit("samp1",
                                       conf->readEntry("StandardFontTest",
                                                       QString(klocale->translate("Standard Font Test")).data()))->widget);
    l->newLine();
    l->addLabel("l3", klocale->translate("Fixed Font"));
    fixedFontName=(QComboBox *)(l->addComboBox("fixedFontName",fixedfl)->widget);
    l->newLine();
    l->skip();
    samp2=(QLineEdit *)(l->addLineEdit("samp2",
                                       conf->readEntry("FixedFontTest",QString(klocale->translate("Fixed Font Test")).data()))->widget);
    l->newLine();
    
    l->addLabel("l4", klocale->translate("Background Color"));
    bgColor=(KColorButton *)(l->addColorButton("bgColor")->widget);
    l->newLine();
    l->addLabel("l5", klocale->translate("Foreground Color"));
    fgColor=(KColorButton *)(l->addColorButton("fgColor")->widget);
    l->newLine();
    l->addLabel("l6", klocale->translate("Link Color"));
    linkColor=(KColorButton *)(l->addColorButton("linkColor")->widget);
    l->newLine();
    l->addLabel("l7", klocale->translate("Followed Color"));
    followColor=(KColorButton *)(l->addColorButton("followColor")->widget);
    l->newLine();

    singlewin=(QCheckBox *)(l->addCheckBox("singlewin",klocale->translate("Use only one window"),
                   conf->readNumEntry("SingleWindow",true))->widget);
    vertsplit=(QCheckBox *)(l->addCheckBox("vertsplit",klocale->translate("Split window vertically"),
                                           conf->readNumEntry("VerticalSplit",false))->widget);

    
    l->endGroup();

    l->newLine();

    l->addGroup("buttons","",false);
    b1=(QPushButton *)(l->addButton("b1",klocale->translate("OK"))->widget);
    b1->setDefault(true);
    b2=(QPushButton *)(l->addButton("b2",klocale->translate("Cancel"))->widget);
    l->endGroup();

    l->activate();
    

    l->setAlign("fontSize",AlignRight|AlignLeft);
    l->setAlign("stdFontName",AlignRight|AlignLeft);
    l->setAlign("fixedFontName",AlignRight|AlignLeft);
    l->setAlign("hdrstyle",AlignRight|AlignLeft);
    l->setAlign("atmstyle",AlignRight|AlignLeft);

    connect (b1,SIGNAL(clicked()),this,SLOT(accept()));
    connect (b1,SIGNAL(clicked()),this,SLOT(save()));
    connect (b2,SIGNAL(clicked()),this,SLOT(reject()));
    fontSize->setCurrentItem(conf->readNumEntry("DefaultFontBase",3)-2);

    connect (fixedFontName,SIGNAL(activated(int)),this,SLOT(syncFonts(int)));
    connect (stdFontName,SIGNAL(activated(int)),this,SLOT(syncFonts(int)));
    stdFontName->setCurrentItem(stdfl->find
                                (conf->readEntry("StandardFont",QString("helvetica").data())));
    fixedFontName->setCurrentItem(fixedfl->find
                                  (conf->readEntry("FixedFont",QString("courier").data())));

    conf->setGroup("Reader");
    hdrstyle->setCurrentItem(conf->readNumEntry
                             ("hdr-style", 1)-1);
    atmstyle->setCurrentItem(conf->readNumEntry
                             ("attmnt-style",2)-1);
    syncFonts(0);
    delete stdfl;
    delete fixedfl;
    
    QColor c;
    c=QColor("white");
    bgColor->setColor(conf->readColorEntry("BackgroundColor",&c));
    c=QColor("black");
    fgColor->setColor(conf->readColorEntry("ForegroundColor",&c));
    c=QColor("blue");
    linkColor->setColor(conf->readColorEntry("LinkColor",&c));
    c=QColor("red");
    followColor->setColor(conf->readColorEntry("FollowedColor",&c));
}


void fontsDlg::syncFonts(int)
{
    samp1->setFont(QFont(stdFontName->currentText()));
    samp2->setFont(QFont(fixedFontName->currentText()));
}

fontsDlg::~fontsDlg()
{
}

void fontsDlg::save()
{
    conf->setGroup("ArticleListOptions");
    conf->writeEntry ("DefaultFontBase",fontSize->currentItem()+2);
    conf->writeEntry ("StandardFont",stdFontName->currentText());
    conf->writeEntry ("FixedFont",fixedFontName->currentText());
    conf->writeEntry ("StandardFontTest",samp1->text());
    conf->writeEntry ("FixedFontTest",samp2->text());
    conf->writeEntry ("BackgroundColor",bgColor->color());
    conf->writeEntry ("ForegroundColor",fgColor->color());
    conf->writeEntry ("LinkColor",linkColor->color());
    conf->writeEntry ("FollowedColor",followColor->color());
    conf->writeEntry ("SingleWindow",singlewin->isChecked());
    conf->writeEntry ("VerticalSplit",vertsplit->isChecked());
    conf->setGroup("Reader");
    conf->writeEntry("hdr-style",hdrstyle->currentItem()+1);
    conf->writeEntry("attmnt-style",atmstyle->currentItem()+1);
    conf->sync();
}

void addFont( QStrList &list, const char *xfont )
{
        const char *ptr = strchr( xfont, '-' );
        if ( !ptr )
                return;

        ptr = strchr( ptr + 1, '-' );
        if ( !ptr )
                return;

        QString font = ptr + 1;

        int pos;
        if ( ( pos = font.find( '-' ) ) > 0 )
        {
                font.truncate( pos );

                if ( font.find( "open look", 0, false ) >= 0 )
                        return;


                QStrListIterator it( list );

                for ( ; it.current(); ++it )
                        if ( it.current() == font )
                                return;

                list.inSort( font );
        }
}


void getFontList( QStrList &list, const char *pattern )
{
        int num;

        char **xFonts = XListFonts( qt_xdisplay(), pattern, 2000, &num );

        for ( int i = 0; i < num; i++ )
        {
                addFont( list, xFonts[i] );
        }

        XFreeFontNames( xFonts );
}
