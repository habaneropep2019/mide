
#ifndef __LJET_DATA__
#define __LJET_DATA__

#include <qobject.h>
#include <qstring.h>
#include <qtstream.h>
#include <qmsgbox.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>


class Data : public QObject
{
    Q_OBJECT

public:

    Data();
    ~Data();

    bool print();
    void setPrinter(char* printer);
    bool eject();
    bool initialize();
    bool reset();
    void readSettings();
    void writeSettings();

public:

    QString Printer;

    QString Job_name;
    QString Density;
    QString Fontnumber;
    QString Format;
    QString Termination;
    QString RET;
    QString Copies;
    QString Powersave;
    QString Resolution;
    QString Language;
    QString Pageprotect;
    QString Ptsize;
    QString Pitch;
    QString Economode;
    QString Manualfeed;
    QString Orientation;
    QString Symset;
    QString Autocont;
    QString Formlines;

    Data operator=(Data data);
};
#endif 
