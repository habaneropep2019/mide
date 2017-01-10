/*
 * Mouspedometa
 *      Based on the original Xodometer VMS/Motif sources.
 *
 * Written by Armen Nakashian
 *            Compaq Computer Corporation
 *            Houston TX
 *            22 May 1998
 *
 * If you make improvements or enhancements to Mouspedometa, please send
 * them back to the author at any of the following addresses:
 *
 *              armen@nakashian.com
 *
 * Thanks to Mark Granoff for writing the original Xodometer, and
 * the whole KDE team for making such a nice environment to write
 * programs in.
 *
 *
 * This software is provided as is with no warranty of any kind,
 * expressed or implied. Neither Digital Equipment Corporation nor
 * Armen Nakashian will be held accountable for your use of this
 * software.
 */

/*	 
**  Xodometer
**  Written by Mark H. Granoff/mhg
**             Digital Equipment Corporation
**             Littleton, MA, USA
**             17 March 1993
**  
**  If you make improvements or enhancements to Xodometer, please send them
**  back to the author at any of the following addresses:
**
**		granoff@keptin.lkg.dec.com
**		granoff@UltraNet.com
**		72301.1177@CompuServe.com
**
**  Thanks to my friend and colleague Bob Harris for his suggestions and help.
**  
**  This software is provided as is with no warranty of any kind, expressed or
**  implied. Neither Digital Equipment Corporation nor Mark Granoff will be
**  held accountable for your use of this software.
**  
**  This software is released into the public domain and may be redistributed
**  freely provided that all source module headers remain intact in their
**  entirety, and that all components of this kit are redistributed together.
**  
**  Modification History
**  --------------------
**  19 Mar 1993	mhg	v1.0	Initial release.
**   3 Aug 1993 mhg	v1.2	Added automatic trip reset feature.
**   5 Jan 1994 mhg	v1.3	Ported to Alpha; moved mi/km button into popup
**				menu; removed 'Trip' fixed label; added font
**				resource.
**   6 Jan 1994 mhg	x1.4	Main window no longer grabs input focus when
**				realized.
**  21 Nov 1994	mhg	x1.4	Added saveFile resource to make location and
**				name of odometer save file customizable.
**   6 Mar 1995 mhg	x1.5	Added automatic html generation.
**   9 Mar 1995 mhg	x1.6	Converted file to format with verion info!
**				Changed .format resource to .units.
**				Improved measurement unit handling and update
**				accuracy. Removed unneccesary label updates.
**  19 Apr 1995 mhg	V2.0	Removed OpenVMS-specific AST code in favor of
**				XtAppAddTimeout. (Works a lot better now, too.)
**  26 Apr 1995 mhg	X2.1-1  Fix trip reset so units is also reset.
**  27 Apr 1995 mhg	X2.1-2  Changed AutoReset timer to be its own timeout
**				on a relative timer based on seconds until
**				midnight.
**  28 Apr 1995	mhg	X2.1-3  Added pollInterval and saveFrequency resources.
**   1 May 1995 mhg	X2.1-4	Make disk writing (data, html) deferred so as
**				to allow for "continuous" display update while
**				mouse is in motion.
**  27 Oct 1995 mhg	V2.1	Final cleanup for this version and public
**                              release.
**
**  18 May 1998 asn     V3.0    Broke the code in all sorts of ways to turn
**                              the mild-mannered Motif version into a
**				modern KDE program.  Code turned into C++,
**				all Xm references were removed, and VMS
**				support was dumped!
**
** 22 Aug 1998 asn      V3.1    Minor updates to make it compile more 
**                              gracefully on modern C++ compilers,
**                              and updated things to KDE 1.0 specs.
**                              Added a proper About Box.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <iostream.h>

#include <qkeycode.h>
#include <qtooltip.h>
#include <qmessagebox.h>

#include <kapp.h>
#include <kwm.h>
#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kiconloader.h>


#include "kodo.h"

// My port of java's gridbaglayout.  Overkill here, but I need test-cases.
#include "gridbag.h"


/*
 * Set the program up, do lots of ugly initialization.
 * Note that we use installEventFilter on the two KImgNum's
 * to make clicks on them bring up the context-menu.
 */
Kodometer::Kodometer(QWidget* parent,const char* name)
    : AnPanel(parent,name),
      speedInterval(500.0),
      distanceInterval(10.0),
      speedSamples(10)
{
    debug        = 0;

    display=KApplication::getKApplication()->getDisplay();
    FindAllScreens();

    root = RootWindow(display,DefaultScreen(display));

    UseMetric = TRUE;
    Enabled = TRUE;  
    AutoReset = TRUE;
    
    lastXCoord   = lastYCoord = XCoord = YCoord = 0;
    Distance     = 0.0;
    TripDistance = 0.0;
    lastDistance = 0.0;
    speed        = 0.0;
    
    
    pollInterval      = 10;
    saveFrequency     = 10;
    pointerScreen     = -1;
    lastPointerScreen = -1;

    cyclesSinceLastSave = 0;

    distanceUnit=inch;
    tripDistanceUnit=inch;

    lastDUnit=distanceUnit;
    lastTUnit=tripDistanceUnit;

    readSettings();


    QString libDir;
    libDir.setStr(KApplication::kde_datadir().copy());
    libDir.append("/kodo/");

    
    totalLabel=new KImageNumber(libDir+"57-2.gif",this,"Total");    
    tripLabel=new KImageNumber(libDir+"57-2.gif",this,"Trip");
//    speedLabel=new KImageNumber(libDir+"57-2.gif",this,"Speed");


    totalLabel->installEventFilter(this);
    tripLabel->installEventFilter(this);
//    speedLabel->installEventFilter(this);

    QToolTip::add(totalLabel, ConversionTable[distanceUnit].fromUnitTagPlural);
    QToolTip::add( tripLabel,  ConversionTable[tripDistanceUnit].fromUnitTagPlural);
    
    
    // Make the popup menu

    menu = new QPopupMenu();

    menu->insertItem("Mouspedometa");
    menu->insertSeparator();
    
    enabledID=menu->insertItem
	("&Enable",this,SLOT(toggleEnabled()),CTRL+Key_E);
    
    metricID=menu->insertItem
	("&Metric Display",this,SLOT(toggleUnits()),CTRL+Key_M);
    
    autoResetID=menu->insertItem
	("Auto &Reset Trip",this,SLOT(toggleAutoReset()),CTRL+Key_R);
    
    menu->insertItem("Reset &Trip",this,SLOT(resetTrip()),CTRL+Key_T);
    menu->insertItem("Reset &Odometer",this,SLOT(resetTotal()),CTRL+Key_O);
    menu->insertSeparator();
    
    menu->insertItem("&About Mouspedometa...",this,SLOT(about()),CTRL+Key_A);
    menu->insertItem("&Help",this,SLOT(help()),CTRL+Key_H);
    menu->insertSeparator();
    
    menu->insertItem("&Quit",this,SLOT(quit()),CTRL+Key_Q);
    menu->setCheckable( TRUE );

    
    menu->setItemChecked(metricID,UseMetric);
    menu->setItemChecked(autoResetID,AutoReset);
    menu->setItemChecked(enabledID,Enabled);

    distanceID=startTimer((int)distanceInterval);
    speedID=startTimer((int)speedInterval);


    // Evil GridBagLayout code, the way REAL men do layout.

    GridBagConstraints gc;

    gb->setInsets(Insets(0,0,0,0));

    gc.gridx=0;
    gc.gridy=0;
    gc.anchor=GridBagConstraints::CENTER;
    gc.fill=GridBagConstraints::HORIZONTAL;
    gc.weightx=1;
    gc.weighty=0;
    gc.gridwidth=1;    
    setConstraints(totalLabel,gc);

    gc.gridx=0;
    gc.gridy=1;
    gc.anchor=GridBagConstraints::NORTH;
    gc.fill=GridBagConstraints::HORIZONTAL;
    gc.weightx=1;
    gc.weighty=1;
    gc.gridwidth=1; 
    setConstraints(tripLabel,gc);


    
/*    gc.gridx=0;
    gc.gridy=2;
    gc.anchor=GridBagConstraints::NORTH;
    gc.fill=GridBagConstraints::HORIZONTAL;
    gc.weightx=1;
    gc.weighty=1;
    gc.gridwidth=1; 
    setConstraints(speedLabel,gc);
*/
    refresh();
}

/*
 * Now I'm not really sure what this does.  I assume its here to find
 * all the displays on your system, and measure them.  During the mouse
 * tracking phase, we use the information stored here to determine how
 * far the mouse moved on a given screen.
 *
 * The point is, since you might have one 17" screen and on 21" screen,
 * lets measure them differently.  Surely this level of accurasy is
 * only provide to prove that the original author was a man's man.
 */
void Kodometer::FindAllScreens(void)
{
    int i = 0;
    int Dh, DhMM, Dw, DwMM;
    double vPixelsPerMM, hPixelsPerMM;
    
    screenCount = ScreenCount(display);
//    if (debug) printf("Display has %d screen%s\n", screenCount, (screenCount == 1 ? "" : "s"));

    while (i < screenCount)
    {
//	if (debug) printf("Screen %d\n", i);
	screenInfo[i].root = RootWindow(display, i);
	screenInfo[i].scr = XScreenOfDisplay(display, i);
    
	screenInfo[i].height = Dh = HeightOfScreen(screenInfo[i].scr);
	DhMM = HeightMMOfScreen(screenInfo[i].scr);
	screenInfo[i].width = Dw = WidthOfScreen(screenInfo[i].scr);
	DwMM = WidthMMOfScreen(screenInfo[i].scr);
/*
	if (debug)
	{
	    printf("\tHeight is %d pixels (%d mm).\n", Dh, DhMM);
	    printf("\tWidth is %d pixels (%d mm).\n", Dw, DwMM);
	    }*/
	
	vPixelsPerMM = (double)Dh / (double)DhMM;
	hPixelsPerMM = (double)Dw / (double)DwMM;
	screenInfo[i].PixelsPerMM = (vPixelsPerMM + hPixelsPerMM) / 2.0;

/*	if (debug)
	{
	    printf("\tVertical pixels/mm  are %2.5f mm\n", vPixelsPerMM);
	    printf("\tHorizontal pixels/mm are %2.5f mm\n", hPixelsPerMM);	    printf("\tAverage pixels/mm are %.2f mm\n", screenInfo[i].PixelsPerMM);
	    }*/
	
	i++;
    }
}

/*
 * Here's where we override events to the KImgNum's to display
 * the context menu
 */
bool Kodometer::eventFilter( QObject *, QEvent *e )
{
    if ( e->type() == Event_MouseButtonPress ) {
	mousePressEvent((QMouseEvent*)e);
	return TRUE;                        
    }
    return FALSE; 
}

/*
 * Show the context menu
 */
void Kodometer::mousePressEvent(QMouseEvent* e)
{
    menu->move(mapToGlobal(e->pos()));
    menu->show();
}

/*
 * Called when the timer expires to query the pointer position,
 * compare it to the last known position, and then to calculate
 * the distance moved.
 */
void Kodometer::timerEvent(QTimerEvent* e)
{
    if (Enabled) {
	
	if(e->timerId()==distanceID) {
	    lastPointerScreen = pointerScreen;
	    lastXCoord = XCoord;
	    lastYCoord = YCoord;

	    XQueryPointer (display, root, &RootIDRet, &ChildIDRet, &XCoord,
			   &YCoord, &WinX, &WinY, &StateMask);
	    
	    if (CalcDistance())
	    {
		
		refresh();
		cyclesSinceLastSave++;
	    }
	    
	}
/*	else {

	    speeds.append(new double(Distance-lastDistance));
	    if(speeds.count()>speedSamples)
		speeds.removeFirst();

	    QListIterator<double> it(speeds);
	    double total=0;
	    
	    for ( ; it.current(); ++it)  {
		double* d = it.current();
		total+= (*d);
	    }

	    total/=speeds.count();
	    
	    speed = 36000*total/speedInterval;
	    
	    lastDistance=Distance;
	    
	    refresh();
	    }*/
    }
}


// Guess!
void Kodometer::toggleEnabled()
{
    Enabled = !Enabled;

    menu->setItemChecked(enabledID,Enabled);
    
    refresh();
}

// Try again!
void Kodometer::toggleAutoReset()
{
    AutoReset = !AutoReset;

    menu->setItemChecked(autoResetID,AutoReset);
    
    refresh();
}


// You're getting warm!
void Kodometer::toggleUnits()
{
    UseMetric=!UseMetric;

    menu->setItemChecked(metricID,UseMetric);

    
    QToolTip::remove(totalLabel);
    
    if(UseMetric==0)
	QToolTip::add(totalLabel,
		      ConversionTable[distanceUnit].fromUnitTagPlural);
    else
	QToolTip::add(totalLabel,
		      ConversionTable[distanceUnit].toUnitTagPlural);


    
    QToolTip::remove(tripLabel);
    
    if(UseMetric==0)
	QToolTip::add(tripLabel,
		      ConversionTable[tripDistanceUnit].fromUnitTagPlural);
    else
	QToolTip::add(tripLabel,
		      ConversionTable[tripDistanceUnit].toUnitTagPlural);
    
    refresh();
}


// Were you dropped on your head as a child?
void Kodometer::resetTrip()
{
    TripDistance = 0.0;
    tripDistanceUnit = inch;
    refresh();
}

// I was!
void Kodometer::resetTotal()
{
    resetTrip();
    
    Distance=0.0;
    distanceUnit=inch;
    
    TripDistance = 0.0;
    tripDistanceUnit = inch;
    
    refresh();
}

// What in the world can this do?
void Kodometer::quit()
{
    saveSettings();
    exit(0);
}

/*
 * Set the values in all the KImgNums, do metric conversions,
 * and make the screen look like reality.
 */
void Kodometer::refresh(void)
{
    
    if(distanceUnit!=lastDUnit) {

	distanceUnit=lastDUnit;
	
	QToolTip::remove(totalLabel);

	if(UseMetric==0)
	    QToolTip::add(totalLabel,
			  ConversionTable[distanceUnit].fromUnitTagPlural);
	else
	    QToolTip::add(totalLabel,
			  ConversionTable[distanceUnit].toUnitTagPlural);
    }

    if(tripDistanceUnit!=lastTUnit) {

	lastTUnit=tripDistanceUnit;
	
	QToolTip::remove(tripLabel);

	if(UseMetric==0)
	    QToolTip::add(tripLabel,
			  ConversionTable[tripDistanceUnit].fromUnitTagPlural);
	else
	    QToolTip::add(tripLabel,
			  ConversionTable[tripDistanceUnit].toUnitTagPlural);
    }
    
    
    QString distance_s;
    QString trip_s;

    if (Enabled) {
	
	FormatDistance(Distance, distanceUnit, distance_s);
	FormatDistance(TripDistance, tripDistanceUnit, trip_s);
    }
    else {
	
	distance_s="------";
	trip_s="------";
    }

//    if (strcmp(distance_s, prev_distance_s) != 0)

    double d=atof(distance_s);
    totalLabel->setValue(d);

//    if (strcmp(trip_s, prev_trip_s) != 0)

    d=atof(trip_s);
    tripLabel->setValue(d);

//    speedLabel->setValue(speed);
}

/*
 * Not sure what this does, its from the original program.
 */
double Kodometer::multiplier(Units unit)
{
    double m = 10;

    switch (unit)
    {
	case mile : m *= 10.0;
	case foot : m *= 10.0;
	case inch : m *= 10.0; break;
    }
    return m;
}

/*
 * This is the bitch function where the _real_ work is done.  I
 * could have re-invented the query_pointer code, but this one is a best.
 *
 * This is code from the original program, responsible for converting the
 * number of pixels traveled into  a real-world coordinates.
 */
#define MAX(_a,_b) ((_a) > (_b) ? (_a) : (_b))
int Kodometer::CalcDistance(void)
{
    double dist, sum;
    int X, Y;
    double distMM, distInches, finalNewDist;
    double oldDistance, oldTripDistance;
    double newDistance, newTripDistance;
    int i = 0, j, finalScreen, increment;
    Units oldDistanceUnit, oldTripDistanceUnit, currentUnit;
    int distanceChanged, tripDistanceChanged;

    int screenOrientation=K_Left;
    
    if (lastXCoord == 0 && lastYCoord == 0)
	return FALSE;

    if (lastXCoord == XCoord && lastYCoord == YCoord)
	return FALSE;
	
    /* Figure out which screen the pointer is on */
    if (screenCount > 1)
	while (i < screenCount)
	    if (RootIDRet == screenInfo[i].root)
		break;
	    else
		i++;

    pointerScreen = i;
    
//    if (debug) printf("CalcDistance: screen: %d, x: %d, y: %d\n", pointerScreen, XCoord, YCoord);

    /*	 
    **  Adjust XCoord or YCoord for the screen its on, relative to screen 0
    **	and screenOrientation.
    */

    if (lastPointerScreen != -1 && pointerScreen != lastPointerScreen)
    {
	switch (screenOrientation)
	{
	    case K_Left:
	    case K_Top:
		finalScreen = 0;
		j = MAX(pointerScreen,lastPointerScreen) - 1;
		increment = -1;
		break;
	    case K_Right:
	    case K_Bottom:
		finalScreen = MAX(pointerScreen,lastPointerScreen) - 1;
		j = 0;
		increment = 1;
		break;
	}
	
	do
	{
	    switch (screenOrientation)
	    {
		case K_Left:
		case K_Right:
		    if (pointerScreen > lastPointerScreen)
			XCoord += screenInfo[j].width;
		    else
			lastXCoord += screenInfo[j].width;
		    break;
		case K_Top:
		case K_Bottom:
		    if (pointerScreen > lastPointerScreen)
			YCoord += screenInfo[j].height;
		    else
			lastYCoord += screenInfo[j].height;
		    break;
	    }
	    if (j != finalScreen)
		j += increment;
	} while (j != finalScreen);
//    	if (debug) printf("  Adjusted for screen ch: x: %d, y: %d\n", XCoord, YCoord);
    }

//    if (debug) printf("In: Distance: %.5f, Trip Distance: %.5f\n", Distance, TripDistance);

    /* Calculate distance in pixels first */

    X = XCoord - lastXCoord;
    X = X*X;
    
    Y = YCoord - lastYCoord;
    Y = Y*Y;
    
    sum = (double)X + (double)Y;
    dist = sqrt(sum);

//    if (debug) printf("  New dist: %.5fp, ", dist);

    /* Convert to millimeters */
    distMM = dist / screenInfo[pointerScreen].PixelsPerMM;
//    if (debug) printf("%.5fmm, ", distMM);

    /* Convert to inches */
    distInches = distMM * 0.04;
//    if (debug) printf("%.5f\"", distInches);

    /* Add an appropriate value to Distance, which may be in a unit other than inches */
    currentUnit = inch;
    finalNewDist = distInches;

    while (currentUnit < distanceUnit)
    {
	finalNewDist =
	    finalNewDist / ConversionTable[currentUnit].maxFromBeforeNext;
	
//	if (debug) printf(", %.5f %s", finalNewDist,
//			  ConversionTable[currentUnit+1].fromUnitTagPlural);

	
	currentUnit++;
    }
    
//    if (debug) printf("\n Next part \n");

    
    oldDistance = Distance * multiplier(distanceUnit);
    
    Distance += finalNewDist;
    oldDistanceUnit = distanceUnit;

    
    if (ConversionTable[distanceUnit].maxFromBeforeNext != -1.0 &&
	Distance >= ConversionTable[distanceUnit].maxFromBeforeNext)
    {
	Distance = Distance / ConversionTable[distanceUnit].maxFromBeforeNext;
	distanceUnit++;
    }
    
    newDistance = Distance * multiplier(distanceUnit);
    distanceChanged = (distanceUnit != oldDistanceUnit ||
		       (unsigned int)oldDistance != (unsigned int)newDistance);

    /* Add an appropriate value to TripDistance, which may be in a unit other than inches */
    currentUnit = inch;
    finalNewDist = distInches;
    
    while (currentUnit < tripDistanceUnit)
    {
	finalNewDist = finalNewDist / ConversionTable[currentUnit].maxFromBeforeNext;
	currentUnit++;
    }
	
    oldTripDistance = TripDistance * multiplier(tripDistanceUnit);
    TripDistance += finalNewDist;
    oldTripDistanceUnit = tripDistanceUnit;
    
    if (ConversionTable[tripDistanceUnit].maxFromBeforeNext != -1.0 &&
	TripDistance >= ConversionTable[tripDistanceUnit].maxFromBeforeNext)
    {
	TripDistance = TripDistance / ConversionTable[tripDistanceUnit].maxFromBeforeNext;
	tripDistanceUnit++;
    }

    
    newTripDistance = TripDistance * multiplier(tripDistanceUnit);
    tripDistanceChanged = (tripDistanceUnit != oldTripDistanceUnit ||
		       (unsigned int)oldTripDistance != (unsigned int)newTripDistance);

    
//    if (debug) printf("Out: Distance: %.5f, Trip Distance: %.5f\n", Distance, TripDistance);

    if (distanceChanged || tripDistanceChanged)
	return TRUE;
    else
	return FALSE;
}


/*
 * This code can probably go away.  Its doing conversions from inches to
 * other units.  Its ugly C-style stuff, that  should't be done in a
 * pretty OO world.
 */
#define THERE_IS_A_NEXT (ConversionTable[unit].maxToBeforeNext != -1.0)
void Kodometer::FormatDistance(double in_dist, Units unit,QString& string)
{
    double out_dist = in_dist;
    char *tag;
    int precision;

    if (UseMetric)
    {
	out_dist = in_dist * ConversionTable[unit].conversionFactor;
	if (THERE_IS_A_NEXT && out_dist > ConversionTable[unit].maxToBeforeNext)
	{
	    out_dist = out_dist / ConversionTable[unit].maxToBeforeNext;
	    unit++;
	}
	if (out_dist == 1.0)
	    tag = ConversionTable[unit].toUnitTag;
	else
	    tag = ConversionTable[unit].toUnitTagPlural;
    }
    else
    {
	if (out_dist == 1.0)
	    tag = ConversionTable[unit].fromUnitTag;
	else
	    tag = ConversionTable[unit].fromUnitTagPlural;
    }
    precision = ConversionTable[unit].printPrecision;

    string.sprintf ("%.*f %s", precision, out_dist, tag);
}


/*
 * Use KConfig to read all settings from disk.  Note that whatever
 * happens here overrides the defaults, but there's not much
 * sanity-checking.
 */
void Kodometer::readSettings(void)
{
    KConfig *config = KApplication::getKApplication()->getConfig();
    config->setGroup( "Settings" );

    UseMetric = config->readNumEntry("UseMetric");

    AutoReset = config->readNumEntry("AutoReset");

    TripDistance = config->readDoubleNumEntry("Trip");
    Distance = config->readDoubleNumEntry("Distance");

    tripDistanceUnit=config->readNumEntry("TripUnit");
    distanceUnit=config->readNumEntry("DistanceUnit");


    // If the user has AutoReset on, set the trip value to 0.
    // This is REALLY not a good plce to be doing this.
    if(AutoReset) {
	TripDistance=0.0;
	tripDistanceUnit=inch;
    }

    lastDistance=Distance;
    
    lastDUnit=distanceUnit;
    lastTUnit=tripDistanceUnit;
}


/*
 * Save reality for use in the next session.
 */
void Kodometer::saveSettings(void)
{
    KConfig *config = KApplication::getKApplication()->getConfig();
    config->setGroup( "Settings" );
    
    QString str;

    config->writeEntry( "UseMetric", UseMetric );

    config->writeEntry( "AutoReset", AutoReset);

    config->writeEntry( "Trip", TripDistance);
    config->writeEntry( "Distance", Distance);

    config->writeEntry( "TripUnit", tripDistanceUnit);
    config->writeEntry( "DistanceUnit", distanceUnit);
    
    config->sync();
}


// Callback to display help
void Kodometer::help()
{
    KApplication::getKApplication()->invokeHTMLHelp( "" , "" );
}


void Kodometer::about()
{
/*    if(aboutDialog==NULL) {

	aboutDialog=new AnPanel(0,"about");

	QLabel* author=new QLabel(aboutDialog,"author");
	author->setText("By Armen Nakashian <armen@nakashian.com>");

	QLabel* version=new QLabel(aboutDialog,"version");
	version->setText("Mouspedometa V0.2");

	QLabel* homepage=new QLabel(aboutDialog,"homepage");
	homepage->setText("http://yawara.anime.net/kodo/");

	QPushButton* done=new QPushButton(aboutDialog,"done");
	done->setText("OK");
	
	GridBagConstraints gc;
	
	gb->setInsets(Insets(3,3,3,3));
	
	gc.gridx=0;
	gc.gridy=0;
	gc.anchor=GridBagConstraints::CENTER;
	gc.fill=GridBagConstraints::NONE;
	gc.weightx=1;
	gc.weighty=0;
	gc.gridwidth=1;    
	aboutDialog->setConstraints(version,gc);
	
	gc.gridx=0;
	gc.gridy=1;
	gc.anchor=GridBagConstraints::NORTH;
	gc.fill=GridBagConstraints::NONE;
	gc.weightx=1;
	gc.weighty=0;
	gc.gridwidth=1; 
	aboutDialog->setConstraints(author,gc);
	

	gc.gridx=0;
	gc.gridy=2;
	gc.anchor=GridBagConstraints::NORTH;
	gc.fill=GridBagConstraints::NONE;
	gc.weightx=1;
	gc.weighty=0;
	gc.gridwidth=1; 
	aboutDialog->setConstraints(homepage,gc);
	
	
	gc.gridx=0;
	gc.gridy=3;
	gc.anchor=GridBagConstraints::SOUTH;
	gc.fill=GridBagConstraints::NONE;
	gc.weightx=1;
	gc.weighty=1;
	gc.gridwidth=1; 
	aboutDialog->setConstraints(done,gc);

	cout << done->sizeHint().width() << ", "
	     << done->sizeHint().height()
	     << endl;


	connect(done,SIGNAL(clicked()),
		this,SLOT(aboutdone()));	
    }


    aboutDialog->resize(aboutDialog->pack());
    aboutDialog->setMaximumSize(aboutDialog->pack());
    
    aboutDialog->show();
*/

    
    QMessageBox::about( this, "About Mouspedometa",
			"Mouspedometa measures your desktop mileage.\n"
			"Copyright 1998 Armen Nakashian\n\n"
			"This program is free, do with it what you will.\n"
			"This program is part of the K Desktop Environment.\n\n"
			"http://yawara.anime.net/kodo/\n\n");
}

int main(int argc,char **argv)
{
    
    /* Top level details */

    KApplication a(argc,argv);
    // We need a top-level widget for the GridBag panel.
    KTopLevelWidget w;
    
    Kodometer o(&w,"Kodo");

    // Get rid of the title bar.
    KWM::setDecoration(w.winId(),KWM::tinyDecoration);
    a.setMainWidget(&w);

    // session management!
    a.enableSessionManagement(true);
    a.setWmCommand(argv[0]);      
    a.setTopWidget(&w);

    
    w.setView(&o);

    // Resize the window to the size needed, and then fix it there.
    w.resize(o.pack());
    w.setMaximumSize(o.pack());

    w.show();
    
    return a.exec();
}


