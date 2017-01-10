
#ifndef SSK_KORNBUTT_H
#define SSK_KORNBUTT_H

#include"kbutton.h"

class KMailBox;
class KornSettings;

///
class KornButton: public KButton
{
	Q_OBJECT

private:
	class KMailBox *_box;
	QString *_audio, *_cmd, *_sound;
	int _lastNum;
	bool _reversed;

//	static QFrame *_header;
public:
	
	///
	KornButton(QWidget *parent, KornSettings *settings);
	///
	~KornButton();

/**@name slots
*/
//@{

public slots:

	/// draws button with the number.
	void setNumber( int );
	/// runs the click command associated with this mailbox.
	void runCommand();
	/// runs the click command if there is unread mail
	void runIfUnread();

//@}

protected:
	///
	virtual void drawButton( QPainter * );
	///
	virtual void mouseDoubleClickEvent( QMouseEvent * );
	///
	virtual void mouseReleaseEvent( QMouseEvent * );

/**@name signals
*/
//@{

signals:
	/// raised on mouse double click
	void doubleClick();
	/// raised on button rightclick
	void rightClick();
	/// raised on buttom leftclick
	void leftClick();

//@}

};

#endif
