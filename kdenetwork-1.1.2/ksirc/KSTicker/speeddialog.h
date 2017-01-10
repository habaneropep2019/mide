/**********************************************************************

	--- Qt Architect generated file ---

	File: speeddialog.h
	Last generated: Sun Dec 21 08:52:31 1997

 *********************************************************************/

#ifndef SpeedDialog_included
#define SpeedDialog_included

#include "speeddialogData.h"

class SpeedDialog : public speeddialogData
{
    Q_OBJECT

public:

    SpeedDialog
    (
     int tick,
     int step,
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~SpeedDialog();
    void setLimit(int, int, int, int);

signals:
    void stateChange(int, int);

protected slots:
    virtual void updateTick(int);
    virtual void updateStep(int);
    virtual void terminate();
 

};
#endif // SpeedDialog_included
