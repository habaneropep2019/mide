#ifndef FIELD_H
#define FIELD_H

#include <qwidget.h>
#include <qlcdnum.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <kprogress.h>
#include <kaccel.h>
#include <kkeydialog.h>
#include <qpopmenu.h>
#include "spiece.h"
#include "net.h"

class Board;

class Field : public QWidget
{
    Q_OBJECT
		
 public:
    Field( QWidget *parent=0, const char *name=0 );
    virtual ~Field();

    void fillPopup(QPopupMenu *pop);
    QSize sizeHint() const;
    void configKeys() { KKeyDialog::configureKeys( kacc ); }
	
 private slots:
    void multiGame();
    void showOpponents();
    void updateOpponents();
	
 protected:
    bool eventFilter(QObject *, QEvent *e);
	
 private:
    QPopupMenu *popup;
	
    Board  *board;
    ShowNextPiece *showNext;

    QLCDNumber *showScore, *showLevel, *showLines;
    QPushButton *quitButton , *startButton, *pauseButton;
    QLabel *lScore, *lLines, *lLevel, *lNext;
    QLabel *labPrevName, *labNextName;
    KProgress *prevHeight, *nextHeight;
	
    NetObject *net_obj;
    KAccel *kacc;
};

#endif // FIELD_H
