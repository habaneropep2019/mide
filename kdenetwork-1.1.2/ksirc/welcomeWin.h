/**********************************************************************

	--- Qt Architect generated file ---

	File: welcomeWin.h
	Last generated: Sat Oct 24 23:35:42 1998

 *********************************************************************/

#ifndef welcomeWin_included
#define welcomeWin_included

#include "welcomeWinData.h"

class welcomeWin : public welcomeWinData
{
    Q_OBJECT

public:

    welcomeWin
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~welcomeWin();

    virtual void show();

protected slots:

    virtual void dismiss();

};
#endif // welcomeWin_included
