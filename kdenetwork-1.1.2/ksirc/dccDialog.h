/**********************************************************************

	--- Qt Architect generated file ---

	File: dccDialog.h
	Last generated: Wed Dec 17 21:00:35 1997

 *********************************************************************/

#ifndef dccDialog_included
#define dccDialog_included

#include "dccDialogData.h"

class dccDialog : public dccDialogData
{
  Q_OBJECT
public:
  
  dccDialog
  (
   QWidget* parent = NULL,
   const char* name = NULL
   );
  
  virtual ~dccDialog();

  inline QListBox *fileListing()
    {
      return fileList;
    }

signals:
  virtual void getFile();
  virtual void forgetFile();

};
#endif // dccDialog_included
