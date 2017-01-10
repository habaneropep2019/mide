/**********************************************************************

	--- Qt Architect generated file ---

	File: defaultfilters.h
	Last generated: Mon Feb 16 20:17:25 1998

 *********************************************************************/

#ifndef DefaultFilters_included
#define DefaultFilters_included

#include "defaultfiltersdata.h"

class DefaultFilters : public defaultfiltersdata
{
    Q_OBJECT

public:

    DefaultFilters
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~DefaultFilters();

public slots:
    void slot_apply();

protected slots:
    void slot_setSampleNickColour();
    void slot_setSampleUsColour();

private:

};
#endif // DefaultFilters_included
