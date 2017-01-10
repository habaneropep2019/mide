// pstable.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#include "pstable.h"
#include "proc.h"

Pstable::Pstable(QWidget *parent, Procview *pv)
       : HeadedTable(parent,
		     HTBL_ROW_SELECTION
		     | HTBL_ROW_DOUBLE_CLICK
		     | HTBL_ROW_CONTEXT_MENU
		     | HTBL_HEADING_TOOLTIPS
		     | HTBL_HEADING_CONTEXT_MENU),
	 procview(pv),
         leftmostchanged(-1)
{
    connect(this, SIGNAL(rowSelected(int)), SLOT(selection_update(int)));
    connect(this, SIGNAL(titleClicked(int)), SLOT(sortcol_change(int)));
    set_sortcol();
}

QString Pstable::title(int col)
{
    return procview->cats[col]->name;
}

QString Pstable::text(int row, int col)
{
    return procview->cats[col]->string(procview->procs[row]);
}

int Pstable::colWidth(int col)
{
    // this is -1 for variable width fields, htable keeps track of it
    return procview->cats[col]->width();
}

int Pstable::alignment(int col)
{
    Category *cat = procview->cats[col];
    return cat->alignment();
}

int Pstable::leftGap(int col)
{
    Category *cat = procview->cats[col];
    return cat->gap();
}

QString Pstable::tipText(int col)
{
    Category *cat = procview->cats[col];
    QString s(cat->help);
    if(cat == procview->sortcat)
	s.append(procview->reversed ? "\n(sorted backwards)" : "\n(sorted)");
    return s;
}

// recompute the widths of displayed columns
void Pstable::recompute_table_widths()
{
    int rows = procview->procs.size();
    int cols = procview->cats.size();
    setNumRows(rows);		// this will clear all selections in pstable
    setAutoUpdate(FALSE);	// avoid redundant repaints
    setNumCols(cols);		// this will clear all widths
    setAutoUpdate(TRUE);
    leftmostchanged = -1;

    for(int col = 0; col < cols; col++) {
	if(widthChanged(col) && leftmostchanged == -1)
	    leftmostchanged = col;
    }

    if(leftmostchanged != -1)
	updateTableSize();
}

// repaint cells that have been changed from previous generation
void Pstable::repaint_changed()
{
    int rows = procview->procs.size();
    int cols = procview->cats.size();
    int left = leftCell(), right = lastColVisible();
    int top = topCell(), bottom = lastRowVisible();
    if(right >= cols) right = cols - 1;
    if(bottom >= rows) bottom = rows - 1;
    int far_right = right;
    if(leftmostchanged != -1 && right >= leftmostchanged)
	right = leftmostchanged - 1;
    for(int c = right + 1; c <= far_right; c++)
	updateHeading(c);
    for(int r = top; r <= bottom; r++) {
	for(int c = left; c <= right; c++) {
	    Category *cat = procview->cats[c];
	    Procinfo *p = procview->procs[r], *op = 0;
	    if(r >= procview->old_procs.size()
	       || (op = procview->old_procs[r], cat->compare(p, op) != 0)
	       || p->selected != op->selected) {
		updateCell(r, c);
	    }
	}
	// update all cells that have moved or changed width
	for(int c = right + 1; c <= far_right; c++)
	    updateCell(r, c);
    }
    if(leftmostchanged != -1)
	clearRight();
}

// repaint cells that have been changed due to configuration change
void Pstable::repaint_statically_changed(int update_col)
{
    int rows = procview->procs.size();
    int cols = procview->cats.size();
    int right = lastColVisible(), left = right + 1;
    int top = topCell(), bottom = lastRowVisible();
    if(leftmostchanged == -1 || leftmostchanged > update_col)
	leftmostchanged = update_col;
    if(leftmostchanged != -1)
	left = leftmostchanged;
    if(right >= cols) right = cols - 1;
    if(bottom >= rows) bottom = rows - 1;
    for(int c = left; c <= right; c++)
	updateHeading(c);
    for(int r = top; r <= bottom; r++) {
	for(int c = left; c <= right; c++)
	    updateCell(r, c);
    }
    if(leftmostchanged != -1)
	clearRight();
}

// transfer selection from procview to pstable
// (no visible update is done)
void Pstable::transfer_selection()
{
    int rows = procview->procs.size();
    for(int i = 0; i < rows; i++)
	setSelected(i, procview->procs[i]->selected, FALSE);
}

// slot: called when selection changes
void Pstable::selection_update(int row)
{
    procview->procs[row]->selected = isSelected(row);
    emit selection_changed();
}

// slot: called when a title is clicked
void Pstable::sortcol_change(int col)
{
    if(col == sortedCol())
	procview->reversed = !procview->reversed;
    else {
	procview->reversed = FALSE;
	procview->sortcat = procview->cats[col];
	setSortedCol(col);
    }
    procview->rebuild();
    recompute_table_widths();
    transfer_selection();
    topAndRepaint();
}

// set sorted column of table to procview->sortcol
void Pstable::set_sortcol()
{
    for(int i = 0; i < procview->cats.size(); i++)
	if(procview->cats[i] == procview->sortcat) {
	    setSortedCol(i); return;
	}
    setSortedCol(-1);
}
