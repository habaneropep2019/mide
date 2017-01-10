/****************************************************************************
**
** A Qt PostScript widget.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <qdrawutl.h>
#include <config.h>
#include "kpswidget.h"
#include "kpswidget.moc"

static Bool broken_pipe = False;

int handler(Display *d, XErrorEvent *e){
    char msg[80], req[80], number[80];

    XGetErrorText(d, e->error_code, msg, sizeof(msg));
    sprintf(number, "%d", e->request_code);
    XGetErrorDatabaseText(d, "XRequest", number, "<unknown>", req, sizeof(req));

    fprintf(stderr, "kghostview: %s(0x%lx): %s\n", req, e->resourceid, msg);

    return 0;
}

KPSWidget::KPSWidget( QWidget *parent ) : QWidget( parent )
{
	//printf("KPSWidget::KPSWidget\n");
	
	XSetErrorHandler(handler);
	
  	fullView = new QWidget( this );
	CHECK_PTR( fullView );
	gs_window = fullView->winId();
	gs_display = fullView->x11Display();
	
	topFrame = new QFrame ( this );
	CHECK_PTR( topFrame );
	topFrame->setFrameStyle( QFrame::HLine | QFrame::Raised );
	topFrame->setLineWidth(1);
	
	leftFrame = new QFrame ( this );
	CHECK_PTR( leftFrame );
	leftFrame->setFrameStyle( QFrame::VLine | QFrame::Raised );
	leftFrame->setLineWidth(1);
	
	rightFrame = new QFrame ( this );
	CHECK_PTR( rightFrame );
	rightFrame->setFrameStyle( QFrame::VLine | QFrame::Sunken );
	rightFrame->setLineWidth(1);
	
	bottomFrame = new QFrame ( this );
	CHECK_PTR( bottomFrame );
	bottomFrame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	bottomFrame->setLineWidth(1);
	
	horScrollBar = new QScrollBar( QScrollBar::Horizontal, this );
	CHECK_PTR( horScrollBar );
	connect( horScrollBar, SIGNAL( valueChanged(int) ),
		SLOT( slotHorScroll(int) ) );
	
	vertScrollBar = new QScrollBar( QScrollBar::Vertical, this );
	CHECK_PTR( vertScrollBar );
	connect( vertScrollBar, SIGNAL(valueChanged(int)),
		SLOT( slotVertScroll(int) ) );
		
	patch = new QWidget( this );
	
	//
	//	MESSAGES DIALOG
	//
	
	messages = new MessagesDialog( 0, "messages" );
	
	//
	//	INTERPRETER CONFIG DIALOG
	//
	
	intConfig = new InterpreterDialog( 0, "intConfig" );

	// Initialise class variables
	// I usually forget a few important ones resulting in nasty seg. faults.

	background_pixmap = None;
	llx = 0;
	lly = 0;
	urx = 0;
	ury = 0;
	buf = (char *)malloc(BUFSIZ);
	left_margin = 0;
	right_margin = 0;
	bottom_margin = 0;
	top_margin = 0;
	foreground = 0;
	background_pixel = 1;
	fullWidth=0;
	fullHeight=0;
	scroll_x_offset=0;
	scroll_y_offset=0;
	antialias=False;
	show_messages=False;
	xdpi=75.0;
	ydpi=75.0;
	disable_start = False;
	interpreter_pid = -1;
	input_buffer = 0;
	bytes_left = 0;
	input_buffer_ptr = 0;
	buffer_bytes_left = 0;
	ps_input = 0;
	interpreter_input = -1;
	interpreter_output = -1;
	interpreter_error = -1;
	interpreter_input_id = None;
	interpreter_output_id = None;
	interpreter_error_id = None;
	orientation = KPSPortrait;
	changed = False;
	busy = False;
	fullView->setCursor( arrowCursor );

	// Compute fullView widget size

	KPSLayout( False );
}

KPSWidget::~KPSWidget()
{
	stopInterpreter();
	
	if ( background_pixmap != None )
		XFreePixmap( gs_display, background_pixmap );
}

//*********************************************************************************
//
//	PUBLIC METHODS
//
//*********************************************************************************


void KPSWidget::disableInterpreter()
{
	////printf("KPSWidget::disableInterpreter\n");

	disable_start = True;
	stopInterpreter();
}


void KPSWidget::enableInterpreter()
{
	//printf("KPSWidget::enableInterpreter\n");

	disable_start = False;
	startInterpreter();
}


Bool KPSWidget::isInterpreterReady()
{
	//printf("KPSWidget::isInterpreterReady\n");

	return interpreter_pid != -1 && !busy && ps_input == 0;
}


Bool KPSWidget::isInterpreterRunning()
{
	//printf("KPSWidget::isInterpreterRunning\n");

	return interpreter_pid != -1;
}

Bool KPSWidget::nextPage()
{
	//debug("KPSWidget::nextPage");

    XEvent ev;

    if (interpreter_pid < 0) return False;
    if (mwin == None) return False;
	
	//debug("gs_window = %x, mwin = %x", gs_window, mwin);

    if (!busy) {
		busy = True;
		//startTimer(200);
		fullView->setCursor( waitCursor );
		
		ev.xclient.type = ClientMessage;
		ev.xclient.display = gs_display;
		ev.xclient.window = mwin;
		ev.xclient.message_type =
			next;
		ev.xclient.format = 32;
		
		XSendEvent(gs_display, mwin, False, 0, &ev);
		XFlush(gs_display);

		return True;
		
    } else {
		return False;
		
    }
}

Bool KPSWidget::sendPS( FILE *fp, long begin, unsigned int len, Bool close )
{
	//printf("KPSWidget::sendPS\n");

	struct record_list *ps_new;
	
	//printf("sendPS input = %d\n", interpreter_input);
	
    if (interpreter_input < 0) return False;

    ps_new = (struct record_list *) malloc(sizeof (struct record_list));
    ps_new->fp = fp;
    ps_new->begin = begin;
    ps_new->len = len;
    ps_new->seek_needed = True;
    ps_new->close = close;
    ps_new->next = 0;

    if (input_buffer == 0) {
		input_buffer = (char *) malloc(BUFSIZ);
    }

    if (ps_input == 0) {
		input_buffer_ptr = input_buffer;
		bytes_left = len;
		buffer_bytes_left = 0;
		ps_input = ps_new;
		interpreter_input_id = 1;
		sn_input->setEnabled(True);
		
		//printf("enabled INPUT\n");

    } else {
		struct record_list *p = ps_input;
		while (p->next != 0) {
			p = p->next;
		}
		p->next = ps_new;
    }
    
    return False;
}    

//*********************************************************************************
//
//	QT WIDGET REIMPLIMENTED METHODS
//
//*********************************************************************************

void KPSWidget::paintEvent(QPaintEvent *)
{
}


void KPSWidget::resizeEvent(QResizeEvent *)
{
	//printf("KPSWidget::resizeEvent\n");
	
	//printf("fullview width = %d height=%d \n", fullView->width(),
	//fullView->height() );
	
	int frame_width, frame_height;
	
	if( width() > fullView->width() &&  height() > fullView->height() ) {
		horScrollBar->hide();
		vertScrollBar->hide();
		clip_x= (int)( width()- fullView->width() )/2;
		clip_y= (int)( height()- fullView->height() )/2;
		clip_width= fullView->width();
		clip_height= fullView->height();
		
	} else if ( width() > fullView->width()+SCROLLBAR_SIZE ) {
		horScrollBar->hide();
		vertScrollBar->show();
		clip_x= (int)( width()-( fullView->width()+SCROLLBAR_SIZE) )/2;
		clip_y= 0;
		clip_width= width()-SCROLLBAR_SIZE;
		clip_height= height();
		
	}else if ( height() > fullView->height()+SCROLLBAR_SIZE ) {
		horScrollBar->show();
		vertScrollBar->hide();
		clip_x= 0;
		clip_y= (int)( height()-( fullView->height()+SCROLLBAR_SIZE ))/2;
		clip_width= width();
		clip_height= height()-SCROLLBAR_SIZE;
	}else {
		horScrollBar->show();
		vertScrollBar->show();
		clip_x= 0;
		clip_y= 0;
		clip_width= width()-SCROLLBAR_SIZE;
		clip_height= height()-SCROLLBAR_SIZE;
	} 
	
    horScrollBar->setGeometry( 0, height()-SCROLLBAR_SIZE,
    	clip_width, SCROLLBAR_SIZE);
    if(fullView->width()- width() > 0) {
    	if( vertScrollBar->isVisible() )
			horScrollBar->setRange( 0, fullView->width() - width() +
				vertScrollBar->width() );
		else
			horScrollBar->setRange( 0, fullView->width() - width() );
	} else {
		horScrollBar->setRange( 0, 0 );
	}
	horScrollBar->setSteps( (int)( fullView->width()/50),  width() );
	
	vertScrollBar->setGeometry(width()-SCROLLBAR_SIZE, 0,
		SCROLLBAR_SIZE, clip_height);
	if( fullView->height()- height() > 0 ) {
		if( horScrollBar->isVisible() )
			vertScrollBar->setRange( 0,  fullView->height() - height() +
				horScrollBar->height());
		else
			vertScrollBar->setRange( 0,  fullView->height() - height() );
	} else {
		vertScrollBar->setRange( 0,  0 );
	}
	vertScrollBar->setSteps( (int)( fullView->height()/50),  height() );
	
	if ( vertScrollBar->isVisible() && horScrollBar->isVisible() ) {
		patch->show();
		patch->setGeometry( vertScrollBar->x(),
		vertScrollBar->y()+vertScrollBar->height(),
			vertScrollBar->width(), horScrollBar->height() );
	} else
		patch->hide();
	
	if( clip_width == fullView->width() ) {
		frame_width = width();
		frame_height = height();
	} else {
		frame_width = clip_width;
		frame_height = clip_height;
	}
	
	topFrame->setGeometry(0, 0,
		frame_width, 1);
		
	leftFrame->setGeometry(0, 0,
		1, frame_height);
		
	rightFrame->setGeometry(frame_width-1, 0,
		1, frame_height);
		
	bottomFrame->setGeometry(0, frame_height-1,
		frame_width, 1);
	
	movePage(); 
}

void KPSWidget::KPSLayout( Bool force )
{
	//printf("KPSWidget::KPSLayout\n");

    Bool different_size = computeSize();
    if (different_size || force ) {
    	
    	//printf("Changed  layout %d, %d\n", fullWidth, fullHeight);
    	
    	fullView->setGeometry(0, 0, fullWidth, fullHeight);
    	//fullView->resize(fullWidth, fullHeight);
		resizeEvent(0); // Dutta 16/3/98
    	// resize(width(), height()); // Dutta 16/3/98
    	repaint();
    	setup();
    	
    }
}

/**********************************************************************************
 *
 *	SLOTS
 *
 **********************************************************************************/

bool KPSWidget::readDown()
{
	int new_value;
	
	if( vertScrollBar->value() == vertScrollBar->maxValue() )
		return False;
	
	new_value = vertScrollBar->value()+height()-50;
	if(new_value > vertScrollBar->maxValue())
		new_value = vertScrollBar->maxValue();
	
	vertScrollBar->setValue( new_value );
	return True;
}

void KPSWidget::scrollRight()
{
	int new_value;
	
	new_value = horScrollBar->value()+50;
	if(new_value > horScrollBar->maxValue())
		new_value = horScrollBar->maxValue();
	
	horScrollBar->setValue( new_value );
}

void KPSWidget::scrollLeft()
{
	int new_value;
	
	new_value = horScrollBar->value()-50;
	if(new_value < horScrollBar->minValue())
		new_value = horScrollBar->minValue();
	
	horScrollBar->setValue( new_value );
}

void KPSWidget::scrollDown()
{
	int new_value;
	
	new_value = vertScrollBar->value()+50;
	if(new_value > vertScrollBar->maxValue())
		new_value = vertScrollBar->maxValue();
	
	vertScrollBar->setValue( new_value );
}

void KPSWidget::scrollUp()
{
	int new_value;
	
	new_value = vertScrollBar->value()-50;
	if(new_value < vertScrollBar->minValue())
		new_value = vertScrollBar->minValue();
	
	vertScrollBar->setValue( new_value );
}

void KPSWidget::scrollTop()
{
	vertScrollBar->setValue( vertScrollBar->minValue() );
}

void KPSWidget::movePage()
{
	fullView->setGeometry(-scroll_x_offset+clip_x, 
		-scroll_y_offset+clip_y, fullView->width(),
		fullView->height());
	repaint();
}
	
	
void KPSWidget::slotVertScroll(int value)
{
	scroll_y_offset =  value;
	movePage();
}

void KPSWidget::slotHorScroll(int value)
{
	scroll_x_offset = value;
	movePage();
}

//*********************************************************************************
//
//	PRIVATE METHODS
//
//*********************************************************************************


Bool KPSWidget::computeSize()
{
	//printf("KPSWidget::computeSize\n");

	int newWidth=0, newHeight=0;
	Bool change = FALSE;
	
	switch (orientation)
	  {
	  case KPSPortrait: //PORTRAIT
	    orient_angle=0;
	    newWidth = (int) ((urx - llx) / 72.0 * xdpi + 0.5);
	    newHeight = (int) ((ury - lly) / 72.0 * ydpi + 0.5);
	    break;
	  case KPSUpsideDown: //UPSIDEDOWN
	    orient_angle=180;
	    newWidth = (int) ((urx - llx) / 72.0 * xdpi + 0.5);
	    newHeight = (int) ((ury - lly) / 72.0 * ydpi + 0.5);
	    break;
	  case KPSLandscape: //LANDSCAPE
	    orient_angle=90;
	    newWidth = (int) ((ury - lly) / 72.0 * xdpi + 0.5);
	    newHeight = (int) ((urx - llx) / 72.0 * ydpi + 0.5);
	    break;
	  case KPSSeascape: //SEASCAPE
	    orient_angle=270;
	    newWidth = (int) ((ury - lly) / 72.0 * xdpi + 0.5);
	    newHeight = (int) ((urx - llx) / 72.0 * ydpi + 0.5);
	    break;
	  }
	
	if((newWidth != fullWidth) || (newHeight != fullHeight))
	  change = TRUE;
	
	
	fullWidth = newWidth;
	fullHeight = newHeight;
	return change;
}

static Bool alloc_error;
static XErrorHandler oldhandler;

static int catch_alloc (Display *dpy, XErrorEvent *err)
{
    if (err->error_code == BadAlloc) {
	alloc_error = True;
    }
    if (alloc_error) return 0;
    return oldhandler(dpy, err);
}

void KPSWidget::setup()
{
	//printf("KPSWidget::setup\n");

	Pixmap bpixmap = None;

	// NO stop interpreter ?
	
	stopInterpreter();
	
	// NO test of actual change ?
	
	if (background_pixmap != None) {
	    XFreePixmap(gs_display, background_pixmap);
	    background_pixmap = None;
	    XSetWindowBackgroundPixmap(gs_display, gs_window, None);
	}
	
	if( intConfig->backingOpt == PIX_BACKING ) {
		if (background_pixmap == None) {

	    	XSync(gs_display, False); 
	    	oldhandler = XSetErrorHandler(catch_alloc);
	    	alloc_error = False;
	    	bpixmap = XCreatePixmap(gs_display, gs_window,
				    	fullView->width(), fullView->height(),
				    	DefaultDepth( gs_display, DefaultScreen( gs_display) ) );
	    	XSync(gs_display, False); 
	    	if (alloc_error) {
	    		printf("BadAlloc\n");
			//XtCallCallbackList(w, gvw->ghostview.message_callback,
			//		   "BadAlloc");
			if (bpixmap != None) {
		    	XFreePixmap(gs_display, bpixmap);
		    	XSync(gs_display, False); 
		    	bpixmap = None;
			}
	    	}
	    	oldhandler = XSetErrorHandler(oldhandler);
	    	if (bpixmap != None) {
				background_pixmap = bpixmap;
				XSetWindowBackgroundPixmap(gs_display, gs_window,
						   background_pixmap);
	    	}
		} else {
	    	bpixmap = background_pixmap;
		}
	}


	XSetWindowAttributes xswa;
	
	if (bpixmap != None) {
		xswa.backing_store = NotUseful;
		XChangeWindowAttributes(gs_display, gs_window,
					CWBackingStore, &xswa);
    } else {
		xswa.backing_store = Always;
		XChangeWindowAttributes(gs_display, gs_window,
					CWBackingStore, &xswa);
    }

	ghostview = (Atom) XInternAtom(gs_display, "GHOSTVIEW", False);
	gs_colors = (Atom) XInternAtom(gs_display, "GHOSTVIEW_COLORS", False);
	next = (Atom) XInternAtom(gs_display, "NEXT", False);
	gs_page = (Atom) XInternAtom(gs_display, "PAGE", False);
	done = (Atom) XInternAtom(gs_display, "DONE", False);
	
	sprintf(buf, "%ld %d %d %d %d %d %g %g %d %d %d %d",
	    background_pixmap,
	    orient_angle,
	    llx, lly,
	    urx, ury,
	    xdpi, ydpi,
	    left_margin, bottom_margin,
	    right_margin, top_margin);
	
	//printf("%s\n", buf);
	
	XChangeProperty(gs_display, gs_window,
	    ghostview,
		XA_STRING, 8, PropModeReplace,
		(unsigned char *)buf, strlen(buf));

	sprintf(buf, "%s %d %d",
		intConfig->paletteOpt == MONO_PALETTE ? "Monochrome" :
	    intConfig->paletteOpt == GRAY_PALETTE  ? "Grayscale" :
	    intConfig->paletteOpt == COLOR_PALETTE	   ? "Color" : "?",
		(int) BlackPixel(gs_display, DefaultScreen(gs_display)),
		(int) WhitePixel(gs_display, DefaultScreen(gs_display)) );
	
	//printf("%s\n", buf);
	
	XChangeProperty(gs_display, gs_window,
		gs_colors,
		XA_STRING, 8, PropModeReplace,
		(unsigned char *)buf, strlen(buf));

	XSync(gs_display, False);  // Be sure to update properties 
} 

void KPSWidget::startInterpreter()
{
	//printf("KPSWidget::startInterpreter\n");
	
	GC gc;
	XGCValues values;
	
	values.foreground = WhitePixel(gs_display, DefaultScreen( gs_display) );
	values.background = BlackPixel(gs_display, DefaultScreen( gs_display) );
	
	gc = XCreateGC ( gs_display,
					RootWindow( gs_display, DefaultScreen( gs_display ) ),
					( GCForeground | GCBackground ), &values );

	stopInterpreter();
	
	if ( background_pixmap != None ) {
		XFillRectangle( gs_display, background_pixmap,
		       gc /* DefaultGC( gs_display, DefaultScreen( gs_display ) ) */,
		       0, 0, fullView->width(), fullView->height() );
    }
    
    fullView->erase();
	
	if (disable_start) return;
	
	gs_arg=0;
	gs_call[gs_arg++] = "gs";
	if( intConfig->antialias ) {
		gs_call[gs_arg++] = "-sDEVICE=x11alpha";
	} else {
		gs_call[gs_arg++] = "-sDEVICE=x11";
	}
	
	if( !intConfig->platform_fonts )
		gs_call[gs_arg++] = "-dNOPLATFONTS";
	
	gs_call[gs_arg++] = "-dNOPAUSE";
	gs_call[gs_arg++] = "-dQUIET";
	gs_call[gs_arg++] = "-dSAFER";
	gs_call[gs_arg++] = "-";
	gs_call[gs_arg++] = 0;
	
	if (filename == 0) {
		ret = pipe(std_in);
		if (ret == -1) {
	    	perror("Could not create pipe");
	    	exit(1);
		}
    } else if (strcmp(filename, "-")) {
		std_in[0] = open(filename, O_RDONLY, 0);
		//printf("opened %s\n", filename.data());
    }

 
    ret = pipe(std_out);
    if (ret == -1) {
		perror("Could not create pipe");
		exit(1);
    }

    ret = pipe(std_err);
    if (ret == -1) {
		perror("Could not create pipe");
		exit(1);
    } 
	
	changed = False;
	busy = True;
	//startTimer(200);
	fullView->setCursor( waitCursor );
	
    interpreter_pid = vfork();
    if (interpreter_pid == 0) {
    
  		::close(std_out[0]);
		::close(std_err[0]);
		::dup2(std_out[1], 1);
		::close(std_out[1]);
		::dup2(std_err[1], 2);
		::close(std_err[1]); 
		sprintf(buf, "%d", (int) gs_window);
		setenv("GHOSTVIEW", buf, True);
		setenv("DISPLAY", XDisplayString(gs_display), True);
		if (filename == 0) {
			::close(std_in[1]);
			::dup2(std_in[0], 0);
			::close(std_in[0]);
		} else if (strcmp(filename, "-")) {
			::dup2(std_in[0], 0);
			::close(std_in[0]);
		}
 	
		execvp(gs_call[0], gs_call);
	  	sprintf(buf, "Exec of this shit failed");
		perror(buf);
		_exit(1); 
		
    } else {
    	if (filename == 0) {
  			int result;
			::close(std_in[0]);

			result = fcntl(std_in[1], F_GETFL, 0);
			result = result | O_NONBLOCK;
			result = fcntl(std_in[1], F_SETFL, result);

			interpreter_input = std_in[1];
			interpreter_input_id = None;

			sn_input = new QSocketNotifier( interpreter_input, QSocketNotifier::Write );
			sn_input->setEnabled(False);
			QObject::connect( sn_input, SIGNAL( activated(int) ),
				this, SLOT( gs_input() ) );
				
		} else if (strcmp(filename, "-")) {
			//printf("Should close stdin[0]\n");
	    	::close(std_in[0]);
		}
		
		//printf("Now connecting output and error channels\n");
		
		::close(std_out[1]);
		interpreter_output = std_out[0];
		sn_output = new QSocketNotifier( interpreter_output,
		QSocketNotifier::Read );
		QObject::connect( sn_output, SIGNAL( activated(int) ),
				this, SLOT( gs_output(int) ) );

		::close(std_err[1]);
		interpreter_error = std_err[0];
		sn_error = new QSocketNotifier( interpreter_error,
		QSocketNotifier::Read );
		QObject::connect( sn_error, SIGNAL( activated(int) ),
				this, SLOT( gs_output(int) ) );
				
		//printf("These connected\n");
    }
    XClearArea(gs_display, gs_window,
	       0, 0, fullWidth, fullHeight, False);
}

void KPSWidget::stopInterpreter()
{
	//printf("KPSWidget::stopInterpreter\n");

	if (interpreter_pid >= 0) {
		::kill(interpreter_pid, SIGTERM);
		//printf("Wait for kill\n");
		::wait(0);
		interpreter_pid = -1;
		//printf("Killing gs process\n");
	}
    
	if (interpreter_input >= 0) {
		::close(interpreter_input);
		//printf("closing interpreter_input\n");
		interpreter_input = -1;
		if (interpreter_input_id != None) {
			sn_input->~QSocketNotifier();
			//printf("Destroy input socket notifier\n");
			interpreter_input_id = None;
		}
		while (ps_input) {
			struct record_list *ps_old = ps_input;
			ps_input = ps_old->next;
			if (ps_old->close) fclose(ps_old->fp);
			free((char *)ps_old);
			//printf("Close and ps pipe\n");
		}
	}
    
    busy=False;
    //killTimers();
    fullView->setCursor( arrowCursor );
    
	if (interpreter_output >= 0) {
		::close(interpreter_output);
		interpreter_output = -1;
		sn_output->~QSocketNotifier();
		//printf("Destroy output socket notifier\n");
	}
	if (interpreter_error >= 0) {
		::close(interpreter_error);
		interpreter_error = -1;
		sn_error->~QSocketNotifier();
		//printf("Destroy error socket notifier\n");
	}
    
}

void KPSWidget::interpreterFailed()
{
	//printf("Interpreter Failed\n");
	stopInterpreter();	
} 

SIGVAL catchPipe(int)
{
    broken_pipe = True;
#ifdef SIGNALRETURNSINT
    return 0;
#endif
}

/* Output - receive I/O from ghostscript's stdout and stderr.
 * Pass this to the application via the output_callback. */
 
void KPSWidget::gs_output( int source )
{
    char buf[BUFSIZ+1];
    int bytes = 0;

    if (source == interpreter_output) {
		bytes = read(interpreter_output, buf, BUFSIZ);
		if (bytes == 0) { /* EOF occurred */
	    	::close(interpreter_output);
	    	interpreter_output = -1;
	    	//printf("Try to destroy sn_output EOF\n");
	    	sn_output->~QSocketNotifier();
	    	return;
		} else if (bytes == -1) {
	    	interpreterFailed();		/* Something bad happened */
	    	return;
		}
    } else if (source == interpreter_error) {
		bytes = read(interpreter_error, buf, BUFSIZ);
		if (bytes == 0) { /* EOF occurred */
	    	::close(interpreter_error);
	    	interpreter_error = -1;
	    	sn_error->~QSocketNotifier();
	    	//printf("Try to destroy sn_output EOF error\n");
	    	return;
		} else if (bytes == -1) {
	    	interpreterFailed();		/* Something bad happened */
	    	return;
		}
    }
    
    if (bytes > 0) {
		buf[bytes] = '\0';
		if( intConfig->show_messages ) {
			messages->show();
			messages->cancel->setFocus();
			messages->messageBox->append( buf );
			//messages->setCursorPosition( messages->numRows(), 0 );
		}
    }
}


void KPSWidget::gs_input()
{
	//printf("KPSWidget::gs_input\n");

	int bytes_written;
	SIGVAL (*oldsig)(int);
	oldsig = signal(SIGPIPE, catchPipe);

	do {
		if (buffer_bytes_left == 0) {
			if (ps_input && bytes_left == 0) {
				struct record_list *ps_old = ps_input;
				ps_input = ps_old->next;
				if (ps_old->close) fclose(ps_old->fp);
				free((char *)ps_old);
			}

			if (ps_input && ps_input->seek_needed) {
				if (ps_input->len > 0)
					fseek(ps_input->fp, ps_input->begin, SEEK_SET);
				ps_input->seek_needed = False;
				bytes_left = ps_input->len;
			}

			if (bytes_left > BUFSIZ) {
		
				
				buffer_bytes_left =
				fread(input_buffer, sizeof (char), BUFSIZ, ps_input->fp);
			} else if (bytes_left > 0) {
				buffer_bytes_left =
				fread(input_buffer, sizeof (char), bytes_left, ps_input->fp);
			} else {
				buffer_bytes_left = 0;
			}
			
			if (bytes_left > 0 && buffer_bytes_left == 0) {
				interpreterFailed();	
			}
			input_buffer_ptr = input_buffer;
			bytes_left -= buffer_bytes_left;
			
		}

		if (buffer_bytes_left > 0) {
			bytes_written =
			write(interpreter_input, input_buffer_ptr, buffer_bytes_left);
			
			if (broken_pipe) {
				broken_pipe = False;
				interpreterFailed();		
			} else if (bytes_written == -1) {
				if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) {
					interpreterFailed();	
				}
			} else {
				buffer_bytes_left -= bytes_written;
				input_buffer_ptr += bytes_written;
			}
		}
		
	} while(ps_input && buffer_bytes_left == 0);

	signal(SIGPIPE, oldsig);
	
	if (ps_input == 0 && buffer_bytes_left == 0) {
		if (interpreter_input_id != None) {
			 sn_input->setEnabled(False);
			 fullView->setCursor( arrowCursor );
			 //printf("Disabled INPUT\n");
			 interpreter_input_id = None;
		}
	}
	
}  


