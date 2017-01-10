/**********************************************************************

	--- Dlgedit generated file ---

	File: Printer.h
	Last generated: Tue Jun 24 12:34:51 1997

 *********************************************************************/

#ifndef Printer_included
#define Printer_included

#include "PrinterData.h"
#include "Data.h"

class Printer : public PrinterData
{
    Q_OBJECT

public:

    Printer
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~Printer();

public slots:

      void setWidgets();
      void setData(Data* data);
      void addPrinter(char* name);

private:




};
#endif // Printer_included
