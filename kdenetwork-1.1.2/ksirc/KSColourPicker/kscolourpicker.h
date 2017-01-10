/**********************************************************************

	--- Qt Architect generated file ---

	File: kscolourpicker.h
	Last generated: Tue Jul 28 03:49:33 1998

 *********************************************************************/

#ifndef kscolourpicker_included
#define kscolourpicker_included

#include "kscolourpickerData.h"
#include <qvector.h>
#include <qpushbutton.h>

typedef QVector<QPushButton> PushButtons;

class kscolourpicker : public kscolourpickerData
{
    Q_OBJECT

public:

    kscolourpicker
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~kscolourpicker();

signals:
  void picked( QString );

protected slots:
  void slot_update();
  void slot_pick();
  void slot_cancel();

private:
  int foregroundColour;
  int backgroundColour;
  PushButtons *MappedButtons;

};
#endif // kscolourpicker_included
