/**********************************************************************

	--- Qt Architect generated file ---

	File: KSCutDialog.h
	Last generated: Mon Dec 29 21:40:17 1997

 *********************************************************************/

#ifndef KSCutDialog_included
#define KSCutDialog_included

#include "KSCutDialogData.h"

class KSCutDialog : public KSCutDialogData
{
    Q_OBJECT

public:

    KSCutDialog
    (
     QWidget* parent = NULL,
     const char* name = NULL
     );
  
  virtual ~KSCutDialog();

  virtual void setText(QString);
  virtual void scrollToBot();
  
protected:
  virtual void closeEvent(QCloseEvent *);

protected slots:
      
  virtual void terminate();
  virtual void copy(); 
  
private:
};
#endif // KSCutDialog_included
