/**********************************************************************

	--- Qt Architect generated file ---

	File: FirstStart.h
	Last generated: Sat Feb 7 00:45:21 1998

 *********************************************************************/

#ifndef FirstStart_included
#define FirstStart_included

#include "FirstStartData.h"

class FirstStart : public FirstStartData
{
    Q_OBJECT
public:
    FirstStart( QWidget* parent = 0L, const char* name = 0L );
    virtual ~FirstStart();

    QString getSpooler();

};
#endif // FirstStart_included
