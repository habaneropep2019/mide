/**********************************************************************

	--- Qt Architect generated file ---

	File: ssfeprompt.h
	Last generated: Thu Jan 15 20:49:55 1998

 *********************************************************************/

#ifndef ssfePrompt_included
#define ssfePrompt_included

#include "ssfepromptdata.h"

class ssfePrompt : public ssfepromptdata
{
    Q_OBJECT

public:
    
    ssfePrompt
    (
     QString prompttext,
     QWidget* parent = NULL,
     const char* name = NULL
     );
  
  virtual ~ssfePrompt();

  QString text();
  void setPassword(bool);
  
protected slots:
 virtual void terminate();
  
};
#endif // ssfePrompt_included
