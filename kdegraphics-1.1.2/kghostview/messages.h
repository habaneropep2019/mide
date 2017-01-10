#ifndef MESSAGES_H
#define MESSAGES_H

#include <qdialog.h>
#include <qframe.h>
#include <qlabel.h>
#include <qmlined.h>
#include <qpushbt.h>
#include <qkeycode.h>
#include <qcombo.h>

class MessagesDialog : public QDialog {

	Q_OBJECT

public:
	MessagesDialog( QWidget *parent, const char *name );
	QMultiLineEdit *messageBox;
	QPushButton* cancel;
	QFrame *frame;
	
public slots:
	void clear();
	
protected slots:
	void resizeEvent( QResizeEvent * );
};

#endif
