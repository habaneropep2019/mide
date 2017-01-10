/**********************************************************************

	--- Qt Architect generated file ---

	File: KSircColour.h
	Last generated: Fri Dec 12 22:54:52 1997

 *********************************************************************/

#ifndef KSircColour_included
#define KSircColour_included

#include "KSircColourData.h"

class KSircColour : public KSircColourData
{
    Q_OBJECT

public:

    KSircColour
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~KSircColour();

signals:
    void update();

protected slots:
    virtual void ok();
    virtual void cancel(); 
    virtual void colourChange(const QColor &);


};
#endif // KSircColour_included
