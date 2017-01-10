/* $Id: kotodoview.h,v 1.3.2.2 1999/03/16 01:14:55 pbrown Exp $ */

#ifndef _KOTODOVIEW_H
#define _KOTODOVIEW_H

#include <qtableview.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <ktablistbox.h>
#include <qlined.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qlistbox.h>
#include <qpopmenu.h>
#include <qlabel.h>

#include "calobject.h"
#include "kdpevent.h"

class todoViewIn : public QTableView
{
  Q_OBJECT
    public:
        todoViewIn( CalObject *, QWidget* parent=0, const char* name=0 );
        ~todoViewIn();

    // old ToDo functions
    public slots:
        void updateView() ;
        void updateConfig();
        KDPEvent *getSelected() { return aTodo; }
        void changeEventDisplay(KDPEvent *which, int action) { updateView(); }

    protected slots:
        void updateItem(int r, int c) { updateCell(r,c); }
        void changeSummary(const char *newsum);
        void updateSummary();
        void changePriority(int pri);
        void lowPriority() { changePriority(3); }
        void mediumPriority() { changePriority(2); }
        void highPriority() { changePriority(1); }
	void sortPriority();
        void doPopup(int, int) {}
        void newTodo();
        void editTodo();
        void cleanWindow(QWidget *) {}
        void deleteTodo();
        void purgeCompleted();
        void headerAction(int) {}
        void hiliteAction(int, int) {}
        void showDates(bool) {}
        inline void showDates() {}
        inline void hideDates() {}

    protected:
        void paintCell( QPainter*, int row, int col );
        void mousePressEvent( QMouseEvent* );
        void keyPressEvent( QKeyEvent* );
        void focusInEvent( QFocusEvent* );
        void focusOutEvent( QFocusEvent* );
        void resizeEvent(QResizeEvent *);

    private:
        int indexOf( int row, int col ) const;
        QString* contents;
        int curRow;
        int curCol;
        QFont todoFont;

    signals:
        void editEventSignal(KDPEvent *);

    protected:
        enum { PRIORITY, DESCRIPTION, COMPLETED, DUEDATE };
        void adjustColumns() {}

	int curSortMode;
        bool editingFlag;
        int prevRow, updatingRow;
        QString tmpSummary;
        QLineEdit *editor;
        QListBox *priList;
        QPopupMenu *rmbMenu1;
        QPopupMenu *rmbMenu2;
        KDPEvent *aTodo;
        CalObject *calendar;
        QList<int> todoIdList;
};


class TodoView : public QWidget
{
  Q_OBJECT

    public:
        TodoView(CalObject *, QWidget* parent=0, const char* name=0 );
        ~TodoView() {}

    public slots:
        void updateView() { todoBox->updateView(); }
        void updateConfig() { todoBox->updateConfig(); }
        KDPEvent *getSelected() { return todoBox->getSelected(); }
        void changeEventDisplay(KDPEvent *which, int action) 
	  { todoBox->changeEventDisplay(which, action); }
        
    signals:
        void editEventSignal(KDPEvent *);

    private:
        void resizeEvent(QResizeEvent *);
	//        void paintEvent(QPaintEvent *) {};

        todoViewIn *todoBox;
        int topH;
        QLabel *label;
};

#endif
