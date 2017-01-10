/**********************************************************************

	--- Dlgedit generated file ---

	File: Misc.h
	Last generated: Tue Jun 24 12:56:12 1997

 *********************************************************************/

#ifndef Misc_included
#define Misc_included

#include "MiscData.h"
#include "Data.h"

class Misc : public MiscData
{
    Q_OBJECT

public:

    Misc
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~Misc();
public slots:

      void setWidgets();
      void setData(Data* data);

private:



};
#endif // Misc_included
