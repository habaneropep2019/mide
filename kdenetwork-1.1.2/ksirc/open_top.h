/**********************************************************************

	--- Qt Architect generated file ---

	File: open_top.h
	Last generated: Sun Nov 23 17:55:22 1997

 *********************************************************************/

#ifndef open_top_included
#define open_top_included

#include "open_topData.h"
#include "messageReceiver.h"
#include "toplevel.h"
#include "iocontroller.h"

class open_top : public open_topData
{
    Q_OBJECT

public:

    open_top
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~open_top();

signals:

    void open_toplevel(QString);

protected slots:
    virtual void create_toplevel();
    void terminate()
      {
        reject();
      }

private:


};
#endif // open_top_included
