#include"config.h"
#include<qlayout.h>
#include<qpainter.h>
#include<qdrawutl.h>
#include<qlist.h>
#include<qcursor.h>
#include<qapp.h>
#include<qpopmenu.h>
#include<qcursor.h>

#include<kapp.h>
#include<kconfig.h>
#include<kmsgbox.h>


#include"korn.h"
#include"kornset.h"
#include"kornbutt.h"
#include"kornopt.h"

#ifdef HAVE_UNISTD_H
#include<unistd.h>
#endif
#include<stdlib.h>

#include"X11/Xlib.h"

KornBox::KornBox(KornBox *other)
{
	name	= other->name.data();
	caption = other->caption.data();
	path	= other->path.data();
	poll	= other->poll;
	notify	= other->notify.data();
	click	= other->click.data();
}

Korn::Korn(QWidget *parent, KornSettings *settings, WFlags f)
	: QFrame(parent, "korn", f)
{
	int count=0;
	KornButton *button;
	QBoxLayout::Direction dir= QBoxLayout::TopToBottom;

	_optionsDialog = 0;
	_noConfig = false;

	// popup menu

	menu = new QPopupMenu();
	menu->insertItem(i18n("&Setup"), this, SLOT( optionDlg() ));
	menu->insertSeparator();
	menu->insertItem(i18n("&Help"), this, SLOT( help() ));
	menu->insertItem(i18n("&About"), this, SLOT( about() ));
	menu->insertSeparator();
	menu->insertItem(i18n("E&xit"), qApp, SLOT( quit() ));

	// widget decorations
	if(settings->layout() == KornSettings::horizontal )
		dir = QBoxLayout::LeftToRight;

	QBoxLayout *layout = new QBoxLayout(this, dir, 2);

	setFrameStyle(Panel | Raised);
	setLineWidth(1);

	// read settings and create boxes
	options = new QList<KornBox>;
	options->setAutoDelete( true );

	while( !settings->done() ) {
		// create a new button with these settings

		button = new KornButton(this, settings);	
		button->resize(20,20);
		layout->addWidget(button);

		connect( button, SIGNAL( rightClick() ), 
			this, SLOT( popupMenu() ));

		button->show();

		/// keep track of settings locally

		KornBox *thisBox = new KornBox;

		thisBox->name = settings->name().data();
		thisBox->caption = settings->caption().data();
		thisBox->path = settings->file().data();
		thisBox->poll = settings->pollTime();
		thisBox->notify = settings->audioCmd().data();
		thisBox->click = settings->clickCmd().data();

		options->append( thisBox );

		count++;
		settings->nextBox();
	}

	// dont layout or resize if no buttons were created
	// This should be changed, perhaps to a default single button

	if( !settings->valid() || count == 0 ) {
	
		// no valid config
		_noConfig = true;

		int result =	KMsgBox::yesNo(0, 
				i18n("korn: configure now?"), 
				i18n("you have not yet configured korn.\n"
				"would you like to do this now?"),
				KMsgBox::QUESTION,
				i18n("Yes"),
				i18n("No - Quit"));

		if( result == 1 ) {
			// create a list with the default mailbox in it.
			KornBox *box = new KornBox;
			box->name	= i18n("personal");
			box->caption	= i18n("Inbox");
			box->path	= (const char *)getenv("MAIL");
			box->poll	= 240;
			
			options->append( box );

			optionDlg();
		} else {
			qApp->quit();
		}

		return;
	} 

	if( dir == QBoxLayout::LeftToRight )
		resize(count*(2+20)+2,4+20);
	else
		resize(4+20, count*(2+20)+2);

	// move to the saved position if a position was saved

	if( settings->geomSet() ) {
		setGeometry( settings->geom().x(), settings->geom().y(),
				width(), height() );
	} else {
		// save position
		KConfig *config = KApplication::getKApplication()->getConfig();

		config->setGroup("Korn");
		config->writeEntry("PosX", frameGeometry().x(), true);
		config->writeEntry("PosY", frameGeometry().y(), true);
		config->writeEntry("winHeight", frameGeometry().height(), true);
	}

	if( settings->noMouse() ) {

		XSelectInput( x11Display(), handle(), ExposureMask 
				| StructureNotifyMask );
	}
}

Korn::~Korn()
{
	delete options;
	delete menu;
}

void Korn::popupMenu()
{
	menu->popup( QCursor::pos() );
}

void Korn::optionDlg()
{
	// since this is a modeless dialog, we don't want to pop
	// up more than one, even though we can.
	 if( _optionsDialog != 0 )
	 	return;

	// 
	_optionsDialog = new KornOptions();
	QList<KornBox> *newList = new QList<KornBox>;
	
	cloneBoxList( newList, options );

	_optionsDialog->setList ( newList );

	connect( _optionsDialog, SIGNAL( finished(int) ),
		this, SLOT( dialogClosed(int) ));

	_optionsDialog->show();
	
}

int Korn::writeOut( QList<KornBox> *list )
{
	KConfig *config = KApplication::getKApplication()->getConfig();
	KornBox *box;

	QString boxList;

	if( config == 0 )
		return -1;

	for( box = list->first(); box != 0; box = list->next() )  {
		// add to box list
		boxList.append( box->name );
		boxList.append(", ");

		// update box entry
		config->setGroup( box->name );
		config->writeEntry( "Name",	box->caption,	true );
		config->writeEntry( "box", 	box->path,	true );
		config->writeEntry( "polltime", box->poll,	true );
		config->writeEntry( "audio",	box->notify,	true );
		config->writeEntry( "command",	box->click,	true );
	}
	config->setGroup( "Korn" );
	config->writeEntry( "boxes", boxList );
	config->sync();

	return 0;
}

void Korn::moveEvent( QMoveEvent * )
{
	KConfig *config = KApplication::getKApplication()->getConfig();

	config->setGroup("Korn");
	config->writeEntry("PosX", frameGeometry().x(), true);
	config->writeEntry("PosY", frameGeometry().y(), true);
	config->writeEntry("Width",	width(), 	true);
	config->writeEntry("Height",	height(), 	true);
	config->sync();
}

void Korn::help()
{
	KApplication::getKApplication()->invokeHTMLHelp(
		"korn/index.html", "");
}

void Korn::about()
{
	QString str;
	str.sprintf(i18n("Korn %s -- Sirtaj Singh Kang\n" 
					"taj@kde.org, 1997\n\n"
					"The K Desktop Environment"),
					VERSION);
	KMsgBox::message(0, i18n("About kOrn"), str);
}

void Korn::dialogClosed(int ret )
{
	QList<KornBox> *list = _optionsDialog->list();

	if( _noConfig && !ret) {
		ret = KMsgBox::yesNo(0, i18n("korn: no config set"), 
				i18n("Would you like to restart korn with\n"
				"the automatically generated configuration?"),
				KMsgBox::QUESTION,
				i18n("Yes, Restart"), i18n("No, Exit"));

		if( ret == 2 ) {
			delete _optionsDialog;
			_optionsDialog = 0;
			qApp->quit();
			return;
		}
	}

	if ( ret ) {
		// Here we should compare lists to see if any have
		// been deleted. If they have, they should be deleted
		// from the config as well.
		// NOTE: No such KConfig feature exists as yet. 9/Jun/97

		writeOut ( list );	

		list->setAutoDelete( TRUE );
		list->clear();
		delete list;

		execvp( qApp->argv()[0], qApp->argv() );
	}

	list->setAutoDelete( TRUE );
	list->clear();
	delete list;
	delete _optionsDialog;

	_optionsDialog = 0;
}

void Korn::cloneBoxList(QList<KornBox> *dest, QList<KornBox> *src)
{
	KornBox *newBox;

	dest->setAutoDelete( TRUE );
	dest->clear();


	if( src->first() != 0 ) {
		do {

			newBox = new KornBox( src->current() );
			dest->append( newBox );

		} while ( src->next() != 0 );
	}
}
