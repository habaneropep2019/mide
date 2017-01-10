#ifndef INFO_H
#define INFO_H

#include <qdialog.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qkeycode.h>
#include <qcombo.h>

class InfoDialog : public QDialog {
	Q_OBJECT
public:

	InfoDialog( QWidget *parent, const char *name );
	QLabel *fileLabel;
	QLabel *titleLabel;
	QLabel *dateLabel;
	QPushButton* ok;
	
public slots:

};

#endif
