
#include<stdlib.h>

#include<qstring.h>
#include<qtooltip.h>
#include<qpainter.h>
#include<qcolor.h>
#include<qapp.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

//#include"qaudio.h"

#include"kornbutt.h"
#include"kmailbox.h"
#include"kornset.h"

#include"X11/Xlib.h"

KornButton::KornButton(QWidget *parent, KornSettings *settings)
	: KButton(parent, "kornbutton")
{
	_box= new KMailBox(settings->file(), settings->pollTime() );

	_audio= new QString( settings->audioCmd() );
	_cmd= new QString( settings->clickCmd() );
	
	if( settings->soundFile() != NULL )
		_sound = new QString( settings->soundFile() );	
	else
		_sound = 0;


	_lastNum = 0;
	_reversed = false;
	
	QToolTip::add( this, settings->caption() );

	connect(_box, SIGNAL( changed( int ) ), this, SLOT( setNumber( int )));
	connect(this, SIGNAL( leftClick() ), this, SLOT( runIfUnread() ));
	connect(this, SIGNAL( doubleClick() ), SLOT( runCommand() ));
//	connect(this, SIGNAL( rightClick() ), SLOT( runCommand() ));

	_box->start();

	if( settings->noMouse() ) {
		XSelectInput( x11Display(), handle(), ExposureMask 
				| StructureNotifyMask );
	}
}

KornButton::~KornButton()
{
	delete _audio;
	delete _cmd;
	delete _box;

	if( _sound != 0 )
		delete _sound;
}

void KornButton::setNumber(int num)
{
//	debug("setNumber called with %d", num);

	if( num == _lastNum ) 
		return;

	if( (num && !_lastNum) || (!num && _lastNum) ) {
		_reversed = ! _reversed;	
//		debug("reversed now %d", _reversed);

#if 0
		if( _reversed ){
			setPalette( QPalette( QColor(80,80,80) ) );
		} 
		else 
			setPalette( *(QApplication::palette()) );
#endif
	}

#if 0
	if( _sound != NULL )
		QAudio::play(_sound->data(), 50 );
#endif
	
	if(num > _lastNum && !_audio->isEmpty() )
		system( _audio->data() );
		
	_lastNum = num;

	adjustSize();
	repaint();

}

#if 0
void KornButton::drawButton( QPainter *painter )
{
	debug("drawButton called");
}
#endif

void KornButton::drawButton( QPainter *painter )
{
	QString *str= new QString;

	str->setNum(_lastNum);

//	debug( "drawButtonLabel called (lastNum = %s)", str->data() );

	if( _reversed ){

		painter->fillRect(1, 1, width()-1, height()-1,
			colorGroup().foreground() );

		painter->setPen( colorGroup().background() );
	}
	else {
		painter->setPen( colorGroup().foreground() );
	}

	painter->drawText(0, 0, width(), height(), AlignCenter, 
		str->data() );

	delete str;

	KButton::drawButton ( painter );
}

void KornButton::runCommand()
{
	if(!_cmd->isEmpty() )
		system(_cmd->data() );
}

void KornButton::mouseDoubleClickEvent( QMouseEvent * )
{
	emit doubleClick();
}

void KornButton::mouseReleaseEvent( QMouseEvent *event )
{
	KButton::mouseReleaseEvent( event );
	KButton::leaveEvent( event );

	switch ( event->button() ) {
		case LeftButton:	
			emit leftClick();
			break;

		case RightButton:
			emit rightClick();
			break;
	}
}

void KornButton::runIfUnread()
{
	_box->reread();

	if( _box->unreadMessages() != 0 )
		runCommand();
}
