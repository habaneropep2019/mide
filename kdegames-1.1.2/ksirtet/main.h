#ifndef MAIN_H
#define MAIN_H

#include <ktopwidget.h>
#include <kmenubar.h>
#include <qpopmenu.h>
#include <kconfig.h>
#include "field.h"

class KTetris : public KTopLevelWidget
{
 Q_OBJECT

 public:
	KTetris();
        virtual ~KTetris();
	bool menu_visible;
	
 private slots:
	void configKeys();
	void toggleMenu();
	void quit();
	
 private:
	KMenuBar *menu;
	QPopupMenu *popup;
	int tog_id;
	
	KConfig *kconf;
	Field *field;
};

#endif // MAIN_H
