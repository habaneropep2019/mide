/**********************************************************************

	--- Dlgedit generated file ---

	File: Fonts.h
	Last generated: Tue Jun 24 11:41:55 1997

 *********************************************************************/

#ifndef Fonts_included
#define Fonts_included

#include "FontsData.h"
#include "Data.h"

class Fonts : public FontsData
{
    Q_OBJECT

public:

    Fonts
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~Fonts();
private:


public slots:

     void setWidgets();
     void setData(Data *data);


};
#endif // Fonts_included
