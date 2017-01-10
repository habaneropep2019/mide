
#include <ktopwidget.h>
#include <qwidget.h>


class KMenuBar;
class KToolBar;
class KStatusBar;
class kpok;


class PokerWindow : public KTopLevelWidget
{
	Q_OBJECT
private:
	KMenuBar        *menu;
	
	QPopupMenu      *optionsPopup;
/*	QPopupMenu      *helpPopup;*/
	QPopupMenu      *filePopup;

	kpok            *_kpok;
	
protected slots:
	int toggleSound();
        void saveProperties(KConfig*);
        void readProperties(KConfig*);

	
public:
	PokerWindow();
	~PokerWindow();
};






