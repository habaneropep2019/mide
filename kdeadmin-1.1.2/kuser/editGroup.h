/**********************************************************************

	--- Dlgedit generated file ---

	File: editGroup.h
	Last generated: Tue Nov 25 21:00:34 1997

 *********************************************************************/

#ifndef editGroup_included
#define editGroup_included

#include "editGroupData.h"
#include "kgroup.h"

class editGroup : public editGroupData
{
  Q_OBJECT

public:

  editGroup(KGroup *akg,
	     QWidget* parent = NULL,
	     const char* name = NULL);

  virtual ~editGroup();

protected slots:
  virtual void ok();
  virtual void cancel(); 

  virtual void add();
  virtual void del();

protected:
  KGroup *kg;
};
#endif // editGroup_included
