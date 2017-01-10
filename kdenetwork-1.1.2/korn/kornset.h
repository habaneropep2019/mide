
#ifndef SSK_KORNSET_H
#define SSK_KORNSET_H

class KConfig;
class QString;

#include<qrect.h>

/**
*/
class KornSettings
{
public:
	enum layoutStyle { horizontal, vertical};

private:
	KConfig 	*_config;
	layoutStyle	_layout;
	int 		_numBoxes;
	int		_defaultPoll;
	QString		*_audio;
	QString		*_command;
	QString		*_boxes;
	QString		*_soundFile;

	bool		_valid;

	const char 	*_boxPtr;
	QString		*_thisBox;

	void getToken();

	int 	_posX;
	int	_posY;
	bool	_remember;
	bool	_posSet;
	QRect	_geom;

	bool	_noMouse;

	/** 
	  * Check if --nomouse is set.
	 */
	static bool KornSettings::checkNoMouse();

public:

	///
	KornSettings(KConfig *config, int defaultPoll=20);
	~KornSettings();

	///
	layoutStyle layout() const {return _layout;};
	/**
	*/
	bool geomSet() const { return _posSet; }
	/**
	*/
	const QRect& geom() const { return _geom; }
	/**
	*/
	bool valid() const { return _valid; }

	/** 
	  * Check if the "--nomouse" option was invoked.
	  * @return TRUE if --nomouse was requested.
	 */
	bool noMouse() const { return _noMouse; };
	
/**@name per-box functions
*/
//@{
	///
	bool nextBox();
	///
	bool done() const { return _thisBox->isEmpty(); };
	///
	QString file();
	///
	QString name();
	///
	QString caption();
	///
	int pollTime();
	///
	QString audioCmd();
	///
	QString clickCmd();
	///
	const char *soundFile(){ if(_soundFile==NULL) return NULL; 
			return _soundFile->data(); };

//@}

};

#endif
