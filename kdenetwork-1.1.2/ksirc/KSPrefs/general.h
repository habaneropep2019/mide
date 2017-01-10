/**********************************************************************

	--- Qt Architect generated file ---

	File: general.h
	Last generated: Sun Jul 26 16:28:55 1998

 *********************************************************************/

#ifndef general_included
#define general_included

#include "generalData.h"

class general : public generalData
{
    Q_OBJECT

public:

    general
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~general();

protected slots:
  void slot_setBackgroundFile(const char*);

public slots:
  void slot_apply();

private slots:
  void slot_openBrowser();
  

};
#endif // general_included
