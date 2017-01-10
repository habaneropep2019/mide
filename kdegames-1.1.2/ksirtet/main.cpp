#include "main.h"

#include <config.h>
#include <qdatetm.h>
#include <qmsgbox.h>
#include <kmsgbox.h>
#include <kstdaccel.h>
#include <kkeydialog.h>

#include "defines.h"
#include "field.h"
#include "version.h"

#include "main.moc"

KTetris::KTetris() 
{
	QTime t = QTime::currentTime();
	srandom( t.hour()+t.minute()+t.second() );
	
	field = new Field(this);
	setView(field);
	
	/* menu */
	popup = new QPopupMenu;
	tog_id = popup->insertItem(i18n("Hide menu"), this, SLOT(toggleMenu()) );
	field->fillPopup(popup);
	popup->insertItem(i18n("&Quit"), this, SLOT(quit()) );
	
	QPopupMenu *options = new QPopupMenu;
	options->insertItem(i18n("&Keys"), this, SLOT(configKeys()) );
	
 	QPopupMenu *help = kapp->getHelpMenu(true, QString(KTETRIS_NAME)
					     + " " + KTETRIS_VERSION
					     + " (" + KTETRIS_DATE + ")\n\n"
					     + i18n("by") + " " + KTETRIS_AUTHOR);
	
	menu = new KMenuBar(this);
	menu->enableMoving(TRUE);
	menu->enableFloating(TRUE);
	menu->insertItem(i18n("&File"), popup);
	menu->insertItem(i18n("&Options"), options);
	menu->insertSeparator();
	menu->insertItem(i18n("&Help"), help);
	setMenu(menu);
	
	/* read the menu visible/invisible config */
	kconf = kapp->getConfig();
	kconf->setGroup("");
	if ( !kconf->hasKey(OP_MENUBAR_VIS) )
		kconf->writeEntry(OP_MENUBAR_VIS, 1);
	if ( kconf->readNumEntry(OP_MENUBAR_VIS)!=1 ) menu->show();
	else menu->hide();
	toggleMenu();
}

KTetris::~KTetris()
{
    delete field;
}

void KTetris::quit()
{
	kconf->setGroup("");
	kconf->writeEntry(OP_MENUBAR_VIS, menu->isVisible());
	kapp->quit();
}

void KTetris::toggleMenu()
{
	if ( menu->isVisible() ) {
		popup->changeItem(i18n("Show menu"), tog_id);
		menu->hide();
	} else {
		popup->changeItem(i18n("Hide menu"), tog_id);
		menu->show();
	}
	
	updateRects();
}

void KTetris::configKeys()
{
    field->configKeys();
}

/* MAIN */
int main( int argc, char **argv )
{
	KApplication a(argc, argv, KTETRIS_NAME);
	KTetris te;
	a.setMainWidget(&te);
	te.setCaption( kapp->getCaption() );
	te.show();
	return a.exec();
}
