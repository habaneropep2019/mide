#ifndef PRINT_H
#define PRINT_H

#include <qdialog.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qkeycode.h>
#include <qcombo.h>
#include <qchkbox.h>
#include <qbttngrp.h>

class PrintSetup : public QDialog {
	Q_OBJECT
public:

	PrintSetup( QWidget *parent, const char *name, QString pname,
				QString spooler, QString variable );
	
	QLineEdit* leName;
	QLineEdit* leSpool;
	QLineEdit* leVar;
	
};

class PrintDialog : public QDialog {
	Q_OBJECT
public:

	int pgMode;
	int pgMax, pgStart, pgEnd;
	enum { All, Current, Marked, Range };
	PrintDialog( QWidget *parent, const char *name, int maxPages, bool marked );
	QLineEdit* nameLineEdit;
	QComboBox* modeComboBox;
	QLineEdit* leStart;
	QLineEdit* leEnd;
	QCheckBox* cbOrder;
	QCheckBox* cbFile;
	QButtonGroup *pgGroup;
	QLabel* lTo;
	
	QString printerName;
	QString printerVariable;
	QString spoolerCommand;
	
public slots:
	void slotPageMode( int m );
	void setup( );
	void checkRange( );
};

#endif
