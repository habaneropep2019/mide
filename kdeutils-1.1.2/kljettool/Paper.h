/**********************************************************************

	--- Dlgedit generated file ---

	File: Paper.h
	Last generated: Tue Jun 24 10:35:56 1997

 *********************************************************************/

#ifndef Paper_included
#define Paper_included

#include "PaperData.h"
#include "Data.h"

class Paper : public PaperData
{
    Q_OBJECT

public:

    Paper
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~Paper();

private:


public slots:

    void setWidgets();
    void setData(Data* data);

};
#endif // Paper_included
