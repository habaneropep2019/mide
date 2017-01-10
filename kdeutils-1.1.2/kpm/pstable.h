// pstable.h				emacs, this is written in -*-c++-*-
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef PSTABLE_H
#define PSTABLE_H

#include "htable.h"
#include "proc.h"

class Pstable : public HeadedTable
{
    Q_OBJECT
public:
    Pstable(QWidget *parent, Procview *pv);

    void recompute_table_widths();
    void repaint_changed();
    void repaint_statically_changed(int update_col);
    void transfer_selection();
    void set_sortcol();

public slots:
    void selection_update(int row);
    void sortcol_change(int col);

signals:
    void selection_changed();

protected:
    // implementation of the interface to HeadedTable
    virtual QString title(int col);
    virtual QString text(int row, int col);
    virtual int colWidth(int col);
    virtual int alignment(int col);
    virtual int leftGap(int col);
    virtual QString tipText(int col);

private:
    Procview *procview;
    int leftmostchanged;	// leftmost col whose width changed, or -1
};

#endif // PSTABLE_H

