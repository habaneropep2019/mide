// htable.h				emacs, this is written in -*-c++-*-
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

// This file defines the HeadedTable class.
// A HeadedTable is a QTableView with one line of headers over each column.

#ifndef HTABLE_H
#define HTABLE_H

#include <qtablevw.h>
#include <qscrbar.h>
#include <qtooltip.h>
#include "svec.h"

class HeadedTable;

class HeadingTip : public QToolTip
{
public:
    HeadingTip(QWidget *parent);

    friend class TableHead;

protected:
    void maybeTip(const QPoint &pos);
};

class TableHead : public QTableView
{
    Q_OBJECT
public:
    TableHead(HeadedTable *parent = 0);

    void tip(const QPoint &pos);

protected slots:
    void scrollSideways(int);

protected:
    virtual void paintCell(QPainter *p, int row, int col);
    virtual int cellWidth(int col);
    virtual void mousePressEvent(QMouseEvent *e);
    void drawMacPanel(QPainter *p, int x, int y, int w, int h, bool sunken);


    void setBackground();

    HeadedTable *htable;
    QFont titlefont;
    int cellheight;
    HeadingTip *htip;

    friend class HeadedTable;
};

class TableBody : public QTableView
{
    Q_OBJECT
public:
    TableBody(HeadedTable *parent = 0);

protected slots:
    // for keyboard accelerators
    void scrollUp();
    void scrollDown();
    void scrollLeft();
    void scrollRight();
    void pageUp();
    void pageDown();
    void jumpTop();
    void jumpBottom();

protected:
    virtual void paintCell(QPainter *p, int row, int col);
    virtual int cellWidth(int col);
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void timerEvent(QTimerEvent *);

    void updateRow(int row);
    void dragSelectTo(int row);
    void setBackground();
	
    HeadedTable *htable;
    int cellheight;
    int first_drag_row;		// row where drag started
    int prev_drag_row;		// row where drag was at last event
    bool autoscrolling;		// true if we are autoscrolling right now
    enum { UP, DOWN } scrolldir;
    static const int scroll_delay; // time delay when autoscrolling, in ms
    QColor background;

    friend class HeadedTable;
};

// table options (bits to be ORed together)

#define HTBL_ROW_SELECTION 1	// rows are selectable
#define HTBL_ROW_DOUBLE_CLICK 2	// rows can be double-clicked
#define HTBL_ROW_CONTEXT_MENU 4	// right button menu on rows
#define HTBL_HEADING_TOOLTIPS 8	// tooltips on headings
#define HTBL_HEADING_CONTEXT_MENU 16 // right button menu on headings

class HeadedTable : public QWidget
{
    Q_OBJECT
public:
    HeadedTable(QWidget *parent, int opts);

    void setSortedCol(int col);
    int sortedCol() { return sorted_col; };
    void setNumRows(int rows);
    int numRows() { return nrows; };
    void setNumCols(int cols);
    int numCols() { return ncols; };
    void setSelected(int row, bool sel, bool update = TRUE);
    int numSelected() { return nselected; };
    void clearAllSelections();
    void selectOnly(int row);
    bool isSelected(int row) { return selected[row]; };
    void updateTableSize();
    void repaintBody() { body->repaint(); };
    void repaintHead() { head->repaint(); };
    void repaintAll();
    void topAndRepaint();
    int leftCell() { return body->leftCell(); };
    int lastColVisible() { return body->lastColVisible(); };
    int topCell() { return body->topCell(); };
    int lastRowVisible() { return body->lastRowVisible(); };
    void updateCell(int row, int col, bool erase=TRUE)
	{ body->updateCell(row, col, erase); };
    void updateHeading(int col) { head->updateCell(0, col); };
    int textWidth(const char *s);
    void clearRight();
    void setAutoUpdate(bool update)
	{ head->setAutoUpdate(update); body->setAutoUpdate(update); };
    bool widthChanged(int col);
    int actualColWidth(int col);

    enum Appearance {macOS7, macOS8, none};
    void setAppearance(Appearance a, bool update = TRUE);
    Appearance appearance() { return current_appearance; };

signals:
    void titleClicked(int col);
    void rowSelected(int row);
    void doubleClicked(int row);
    void rightClickedRow(QPoint where);
    void rightClickedHeading(QPoint where, int col);

protected:
    // These must be implemented in subclasses
    virtual QString title(int col) = 0;
    virtual QString text(int row, int col) = 0;
    virtual int colWidth(int col) = 0; // negative return means variable width
    virtual int alignment(int col) = 0;
    virtual int leftGap(int col) = 0;

    // must be implemented or tooltips won't work
    virtual QString tipText(int col);

    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

private:
    // to make signals originate from the right object
    void emit_click_signal(int col);
    void emit_double_click_signal(int row);
    void emit_right_click_signal(QPoint where);
    void emit_right_click_heading_signal(QPoint where, int col);

    int computedWidth(int col);
    void resetWidths();

    int sorted_col;
    int options;
    int nrows;
    int ncols;

    Svec<int> selected;
    int nselected;

    Svec<int> widths;

    TableHead *head;
    TableBody *body;

    QFontMetrics *fm;

    int separator_width;	// gap between head and body

    Appearance current_appearance;

    friend class TableHead;
    friend class TableBody;
};

#endif	// HTABLE_H



