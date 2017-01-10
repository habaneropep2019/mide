#ifndef GOTO_H
#define GOTO_H

#include <qdialog.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qkeycode.h>

class GoTo : public QDialog {
	Q_OBJECT
public:

	GoTo( QWidget *parent, const char *name );
	int page, part, pages, i, cumulative_pages;
	
	int 	 current_page;
	int 	 pages_in_part[10];
	int 	 num_parts;  
	
	QLineEdit* partLine;
	QLineEdit* pageLine;
	QLabel *pageLabel;
	QLabel *partLabel;
	QPushButton* ok;
	QPushButton* cancel;
	
	void init();
	
public slots:

	void partChange();
	void pageChange();
	void setCurrentPage();
};

#endif
