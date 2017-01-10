/**********************************************************************

	--- Qt Architect generated file ---

	File: startup.h
	Last generated: Sat Jan 17 13:28:04 1998

 *********************************************************************/

#ifndef StartUp_included
#define StartUp_included

#include "startupdata.h"

class StartUp : public startupdata
{
    Q_OBJECT

public:
    
    StartUp
    (
     QWidget* parent = NULL,
     const char* name = NULL
     );
  
  virtual ~StartUp();
  
public slots:
 
 void slot_apply();

protected slots:
 void nicks_delete();
 void nicks_add();
 void sle_add_update(const char *);
  

};
#endif // StartUp_included
