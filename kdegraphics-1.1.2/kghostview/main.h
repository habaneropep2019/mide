#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <math.h>
#include <qapp.h>
#include <qwidget.h>
#include <qwindow.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qstring.h>
#include <qscrbar.h>
#include <qframe.h>
#include <qpaintd.h>
#include <qbitmap.h>
#include <qpushbt.h>
#include <qsocknot.h>
#define	 GC GC_QQQ
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/Xmu.h>
#include <X11/Xos.h>


#define TOOLBAR_HEIGHT 26
#define TOOLBAR_X_OFFSET 10
#define TOOLBAR_Y_OFFSET 5
#define TOOLBAR_BUTTON_HEIGHT 28
#define TOOLBAR_BUTTON_WIDTH 28

#define MENUBAR_HEIGHT 26



#define STATUSBAR_HEIGHT 20
#define STATUSBAR_PAGE_WIDTH 140
#define STATUSBAR_ZOOM_WIDTH 45
#define STATUSBAR_ORIENT_WIDTH 90


#define PAGE_X_OFFSET 2

class KPSWidget;


class KGhostview : public QWidget
{
    Q_OBJECT

public:
	KGhostview();
	Boolean setup();
	void new_file(int number);
	void show_page(int number);
	
	QString open_file(QString name);
	void buildPagemediaMenu();
	Boolean same_document_media();
	Boolean set_new_orientation(int number);
	Boolean set_new_pagemedia( int number );
	
	void set_magstep(int i);
	Boolean set_new_magstep();
	void magnify(float *dpi, int magstep);
	
	int shrink_magsteps, expand_magsteps, magstep, current_magstep;
	
Boolean	force_document_media;	
	int	base_papersize;
	String	toc_text;
	int	toc_length;
	int	toc_entry_length;
	int pages_in_part[10];
	int num_parts;
	char page_total_label[20];
	char part_total_label[20];
	QString part_string;
	QString page_string;
	QString page_label_text;
	QString part_label_text;
	QString position_label;
	
	
	float default_xdpi, default_ydpi;
	int current_llx;
	int current_lly;
	int current_urx;
	int current_ury;
	int current_pagemedia, default_pagemedia, document_media;
	
	int current_orientation;
	
	int antialias_id;
	KPSWidget *page;
	
public slots:
   void dummy();
   void next_page();
   void prev_page();
   void open_new_file();
   void zoom_in();
   void zoom_out();
   void options_menu_activated(int item);
   
   
protected:
   void resizeEvent( QResizeEvent * );
	
private:
	QMenuBar *menubar;
	QPopupMenu *m_file;
	QPopupMenu *m_view;
	QPopupMenu *m_go;
	QPopupMenu *m_options;
	QPopupMenu *m_help;
	
	QPushButton *b_forward;
	QPushButton *b_back;
	QPushButton *b_page;
	QPushButton *b_view;
	QPushButton *b_print;
	QPushButton *b_mark;
	
	QFrame *frame;
	
	
	QLabel *statusPageLabel;
	QLabel *statusZoomLabel;
	QLabel *statusOrientLabel;
	QLabel *statusMiscLabel;

	
};

#endif // MAIN_H


