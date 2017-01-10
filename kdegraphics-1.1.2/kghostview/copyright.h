#ifndef COPYRIGHT_H
#define COPYRIGHT_H

#include <qdialog.h>
#include <qframe.h>
#include <qlabel.h>
#include <qmlined.h>
#include <qpushbt.h>
#include <qkeycode.h>
#include <qcombo.h>

class CopyrightDialog : public QDialog {
	Q_OBJECT
public:

	CopyrightDialog( QWidget *parent, const char *name );
	QMultiLineEdit *copyrightBox;
	QPushButton* cancel;
};

#endif
