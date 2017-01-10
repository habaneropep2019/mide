#include <qpixmap.h>
#include <qwindow.h>
#include <qpushbt.h>
#include <qlabel.h>

#include "defines.h"

class CardWidget : public QPushButton
{
	Q_OBJECT
	    
signals:	    
	void pClicked(CardWidget *);
	
public:
	CardWidget( QWidget *parent=0, const char *name=0 );
	
	void        paintCard(int cardType, int xPos, int yPos);
	    int         queryHeld();
	void        setHeld(int newheld);
	int         toggleHeld(); /* returns the new value of held*/
	QLabel      *heldLabel;
	
protected:
	void        paintEvent( QPaintEvent *e );
	
protected slots:
	void ownClick();
	

private:
	QPixmap     *pm;                     // the loaded pixmap
	int         held;
};

class CardImages : public QWindow
{
	Q_OBJECT
public:
	CardImages( QWidget *parent=0, const char *name=0 ) : QWindow( parent, name ) {
	}
	
	bool        loadCards( const char *path);
	
	QPixmap     cardP[52];
	QPixmap     deck;
};
