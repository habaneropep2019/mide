
#include<ctype.h>
#include<qstring.h>
#include<kconfig.h>
#include<kapp.h>

#include"kornset.h"

KornSettings::KornSettings(KConfig *config, int defaultPoll)
{
	_config = config;

	_config->setGroup("Korn");

	_valid		= true;
	_layout 	= horizontal;
	_defaultPoll	= defaultPoll; 
	_boxPtr		= NULL;
	_audio		= new QString;
	_command	= new QString;
	_boxes		= new QString;
	_thisBox	= new QString;
	_soundFile	= NULL;

	_posSet = false;

// bail out if we haven't a proper .rc file

	if( !_config->hasKey("boxes" ) ) {
		_valid = false;
		return;
	}
	
	*_boxes = _config->readEntry("boxes");
	*_boxes = _boxes->stripWhiteSpace();
	_boxPtr = _boxes->data();

// set up defaults

	if(_config->hasKey("polltime") )
		_defaultPoll = _config->readNumEntry("polltime");

	if(_config->hasKey("layout") ){
		QString layoutStr = _config->readEntry("layout");
		layoutStr = layoutStr.stripWhiteSpace();

		if( toupper(layoutStr.data()[0]) == 'V' )
			_layout = vertical;
	}

	if( _config->hasKey("audio") )
		*_audio = _config->readEntry("audio");

	if( _config->hasKey("command") )
		*_command = _config->readEntry("command");

	if( _config->hasKey("sound") ) {
		_soundFile = new QString(_config->readEntry("sound") );
		_soundFile->detach();
	}

	_remember = _config->readNumEntry("rememberPos", 0);

	if( _remember && _config->hasKey("posX") ) {
		_geom.setX( _config->readNumEntry("posX") );
		_geom.setY( _config->readNumEntry("posY") );
//		_geom.setHeight( _config->readNumEntry("winHeight", 
		_posSet = TRUE;

	} else {
		_posY = _posX = 0;
	}

	getToken();

	if(!done())
		_config->setGroup(*_thisBox);

	_noMouse = checkNoMouse();

}

KornSettings::~KornSettings()
{
	delete _audio;
	delete _command; 
	delete _boxes;
	delete _thisBox;

	if(_soundFile != NULL)
		delete _soundFile;
}


QString KornSettings::file()
{
	return QString(_config->readEntry("box"));
}


QString KornSettings::caption()
{
	if( _config->hasKey("name") )
		return _config->readEntry("name");

	return QString(*_thisBox);
}

QString KornSettings::name()
{
	return QString(*_thisBox);
}

int KornSettings::pollTime()
{
	if( _config->hasKey("polltime") )
		return _config->readEntry("polltime").toInt();
	
	return _defaultPoll;
		
}

QString KornSettings::audioCmd()
{
	if( _config->hasKey("audio") )
		return _config->readEntry("audio");

	return QString(*_audio);
}

QString KornSettings::clickCmd()
{
	if( _config->hasKey("command") )
		return _config->readEntry("command");
	
	return QString(*_command);
}


void KornSettings::getToken()
{
	const char *startPtr= _boxPtr;

	if(!_boxPtr || !*_boxPtr){
		_boxPtr = 0;
		_thisBox->resize(0);
		return;
	}
	
	while(*_boxPtr && *_boxPtr != ',')
		_boxPtr++;
	
	if(_boxPtr == startPtr){
		_thisBox->resize(0);
		_boxPtr = 0;
		return;
	}
	
	*_thisBox = _boxes->left((unsigned)(_boxPtr-startPtr));
	*_thisBox = _thisBox->stripWhiteSpace();

	if( *_boxPtr == ',')
		_boxPtr++;
	
	_boxes->remove(0, (unsigned) (_boxPtr-startPtr) );
	*_boxes = _boxes->stripWhiteSpace();

	if( _boxes->isEmpty() )
		_boxPtr = 0;
	else
		_boxPtr = _boxes->data();
}

bool KornSettings::nextBox()
{
	if( !done() ) {
		getToken();
		if( !done() && strcmp(_thisBox->data(), "|") )
			_config->setGroup(*_thisBox);

		return true;
	}

	return false;
}

bool KornSettings::checkNoMouse()
{
	for(int i=1; i <= qApp->argc(); i++ ) {
		if( !strcmp( qApp->argv()[i],"--nomouse" ) ) {
			return true;
		}
	}

	return false;
}
