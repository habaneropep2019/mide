
#ifndef __LJET_MAIN__
#define __LJET_MAIN__

#include <qobject.h>
#include <qstring.h>
#include <qdstream.h>
#include <qmsgbox.h>
#include <qstrlist.h> 
#include <qtabdlg.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>


#include <kapp.h>
#include "Data.h"
#include "Paper.h"
#include "PaperData.h"
#include "Fonts.h"
#include "FontsData.h"
#include "Printer.h"
#include "PrinterData.h"
#include "Misc.h"
#include "MiscData.h"




class LJET : public QObject
{
    Q_OBJECT

public:

    LJET();
    ~LJET();


public slots:
    void apply();
    void addPrinter(char* name);
      
public:

    QTabDialog *mainWindow;
    Data *data;
    Paper *paper;
    Fonts *fonts;
    Printer *printer;
    Misc *misc;

};
#endif 
