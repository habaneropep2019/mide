// -*-C++-*-;
#ifndef __KODO__
#define __KODO__

#include <qlabel.h>
#include <qlist.h>
#include <qpopmenu.h>

#include <X11/Xlib.h>

#include "gridbag.h"
#include "kimgnum.h"

#define MAXARGS 25
#define K_Left 1
#define K_Right 2
#define K_Top 3
#define K_Bottom 4
#define MAX_SCREEN 16
#define MAJOR_VERSION 3
#define MINOR_VERSION 0

const int inch=0;
const int foot=1;
const int mile=2;
const int MAX_UNIT=3;
typedef int Units;


typedef struct
{
    Window root;
    Screen *scr;
    int height, width;
    double PixelsPerMM;
} _screenInfo;


typedef struct conversionEntry {
    Units fromUnit;
    char *fromUnitTag;
    char *fromUnitTagPlural;
    double maxFromBeforeNext;
    double conversionFactor;
    char *toUnitTag;
    char *toUnitTagPlural;
    double maxToBeforeNext;
    int printPrecision;
} conversionEntry;

static	struct conversionEntry ConversionTable[MAX_UNIT] = {
{ inch, "inch", "inches", 12.0,   2.54,     "cm",     "cm",     100.0,  3 },
{ foot, "foot", "feet",   5280.0, 0.3048,    "meter", "meters", 1000.0, 4 },
{ mile, "mile", "miles",  -1.0,   1.609344, "km",     "km",     -1.0,   5 }
};


class AnPanel : public QFrame
{
    Q_OBJECT
public:

    AnPanel(QWidget* parent, const char* name)
	: QFrame(parent,name) {

	gb=new GridBagLayout(Insets(2,2,2,2));

	ignoreResize=0;
    }

    ~AnPanel() {
	delete gb;
    }

    virtual void setConstraints(QWidget* w,GridBagConstraints& gc) {
	gb->setConstraints(w,gc);
    }
    
    virtual void resizeEvent(QResizeEvent* r) {
	QWidget::resizeEvent(r);
	
	if(ignoreResize==0)
	    gb->layoutContainer(this);
    }


    virtual QSize pack() {

	QSize s;
	gb->preferredLayoutSize(this,&s);
	return s;
    }
    
protected:
    
    GridBagLayout* gb;
    int ignoreResize;
};




class Kodometer : public AnPanel
{
    Q_OBJECT
public:

    const double speedInterval;
    const double distanceInterval;
    const int speedSamples;
    
    Kodometer(QWidget* ,const char* );
    ~Kodometer() {}
    void refresh(void);
    void readSettings(void);
    void saveSettings(void);

    void timerEvent(QTimerEvent*);
    void mousePressEvent(QMouseEvent*);
//    void paintEvent( QPaintEvent * );
    bool eventFilter( QObject *, QEvent *e );
    
public slots:
     
    void toggleEnabled();
    void toggleUnits();
    void toggleAutoReset();
    void resetTrip();
    void resetTotal();
    void quit();
    void help();
    void about();
    
protected:

    double multiplier(Units);
    int CalcDistance(void);
    void FormatDistance(double , Units , QString&);
    void FindAllScreens();

    KImageNumber* tripLabel;
    KImageNumber* totalLabel;
    KImageNumber* speedLabel;

    QPopupMenu* menu;
    int metricID;
    int autoResetID;
    int enabledID;

    double speed;
    QList<double> speeds;
    double lastDistance;
    int distanceID;
    int speedID;


    Display *display;
    Window root, RootIDRet, ChildIDRet;
    int WinX, WinY;
    unsigned int StateMask;
    int XCoord, YCoord;
    int lastXCoord, lastYCoord;

    Units lastDUnit, lastTUnit;
    
    int pointerScreen, lastPointerScreen;
    int Enabled;
    int UseMetric;
    int AutoReset;
    double Distance, TripDistance;
    Units distanceUnit, tripDistanceUnit;
    int debug;
    char startDate_s[128];
    int cyclesSinceLastSave;
    int pollInterval;
    int saveFrequency;

    _screenInfo screenInfo[MAX_SCREEN];
    int screenCount;
    
};

#endif
