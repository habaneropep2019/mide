// htable.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

// TODO:
// * interface to add/remove columns and to repaint everything in table/head
// * interface to repaint individual cell
// * field width management? (would require vector, length = ncols)
// * multiple styles (macos 7, 8, motif, windows)
// * drag speed proportional to distance from edge
// * use bit vector instead of byte vector for selections
// * context menu on headings
// * interface to add/remove rows (for disclosure triangles)
// * interface to display pixmaps in cells (for disclosure triangles etc)

#include <qaccel.h>
#include <kapp.h>
#include "htable.h"
#include "svec.C"

const int TableBody::scroll_delay = 10; // time delay when autoscrolling, in ms

// HeadingTip: tooltips for headings
HeadingTip::HeadingTip(QWidget *parent)
          : QToolTip(parent)
{}

void HeadingTip::maybeTip(const QPoint &pos)
{
    ((TableHead *)parentWidget())->tip(pos);
}

// TableHead: the horizontally scrollable table head

TableHead::TableHead(HeadedTable *parent)
          : QTableView(parent),
	    htable(parent),
	    titlefont(parent->font())
{
    setBackground();
    setTableFlags(Tbl_smoothHScrolling | Tbl_scrollLastHCell);
    setFont(titlefont);
    cellheight = QFontMetrics(titlefont).lineSpacing();
    setCellHeight(cellheight);
    setNumRows(1);
    if(htable->options & HTBL_HEADING_TOOLTIPS)
	htip = new HeadingTip(this);
}

// set background colour according to appearance
void TableHead::setBackground()
{
    switch(htable->appearance()) {
    case HeadedTable::macOS7:
	setBackgroundColor(white);
	break;
    case HeadedTable::macOS8:
	setBackgroundColor(QColor(0xc0c0c0)); // Qt colour
	break;
    default:
	break;
    }
}

void TableHead::drawMacPanel(QPainter *p, int x, int y, int w, int h,
		  bool sunken)
{
    QColor nw1, se1, c1;
    QColor nw2, se2, c2;

    if(sunken) {
	nw1 = QColor(0x111111);
	se1 = QColor(0x444444);
	c1 = QColor(0x222222);
	nw2 = QColor(0x555555);
	se2 = QColor(0xaaaaaa);
	c2 = QColor(0x777777);
	p->fillRect(x + 2, y + 2, w - 4, h - 4, QColor(0x888888));
    } else {
	nw1 = QColor(0x606060);	// re-use some Qt colours
	se1 = QColor(0x333333);
	c1 = QColor(0x555555);
	nw2 = QColor(0xffffff);
	se2 = QColor(0x888888);
	c2 = QColor(0xc0c0c0);	// Qt colour
    }
    p->setPen(nw1);
    p->drawLine(x, y, x, y + h - 2);
    p->drawLine(x, y, x + w - 2, y);
    p->setPen(c1);
    p->drawPoint(x, y + h - 1);
    p->drawPoint(x + w - 1, y);
    p->setPen(se1);
    p->drawLine(x + 1, y + h - 1, x + w - 1, y + h - 1);
    p->drawLine(x + w - 1, y + 1, x + w - 1, y + h - 1);
    p->setPen(nw2);
    p->drawLine(x + 1, y + 1, x + 1, y + h - 3);
    p->drawLine(x + 1, y + 1, x + w - 3, y + 1);
    p->setPen(c2);
    p->drawPoint(x + 1, y + h - 2);
    p->drawPoint(x + w - 2, y + 1);
    p->setPen(se2);
    p->drawLine(x + 2, y + h - 2, x + w - 2, y + h - 2);
    p->drawLine(x + w - 2, y + 2, x + w - 2, y + h - 2);
}

void TableHead::paintCell(QPainter *p, int, int col)
{
    int gap = htable->leftGap(col);
    int w = cellWidth(col);
    if(htable->appearance() == HeadedTable::macOS8) {
	drawMacPanel(p, 0, 0, cellWidth(col), cellheight,
		     col == htable->sorted_col);
	p->setPen(black);
	w -= 4;			// don't go too close to the border
    }
    if(htable->appearance() == HeadedTable::macOS7) {
	titlefont.setUnderline(col == htable->sorted_col);
	p->setFont(titlefont);
    }
    p->drawText(gap, 0, w - gap, height(),
		AlignVCenter | htable->alignment(col),
		htable->title(col));
}

int TableHead::cellWidth(int col)
{
    return htable->actualColWidth(col);
}

void TableHead::scrollSideways(int val)
{
    setXOffset(val);
}

void TableHead::mousePressEvent(QMouseEvent *e)
{
    int col = findCol(e->pos().x());
    if(col == -1) return;
    if(e->button() == LeftButton)
	htable->emit_click_signal(col);
    else if(htable->options & HTBL_HEADING_CONTEXT_MENU
	    && e->button() == RightButton)
	htable->emit_right_click_heading_signal(mapToGlobal(e->pos()), col);
}

void TableHead::tip(const QPoint &pos)
{
    int col = findCol(pos.x());
    if(col != -1) {
	QString s = htable->tipText(col);
	if(!s.isEmpty()) {
	    int x;
	    colXPos(col, &x);
	    htip->tip(QRect(x, 0, cellWidth(col), height()), s);
	}
    }
}

// TableBody: the table body, scrollable in all ways

TableBody::TableBody(HeadedTable *parent)
          : QTableView(parent),
	    htable(parent)
{
    setBackground();
    setTableFlags(Tbl_autoScrollBars | Tbl_smoothScrolling
		  | Tbl_clipCellPainting);
    setFont(parent->font());
    cellheight = htable->fm->lineSpacing();
    setCellHeight(cellheight);
    ((QScrollBar *)horizontalScrollBar())->setTracking(TRUE);

    QAccel *acc = new QAccel(this);
    acc->connectItem(acc->insertItem(Key_Up), this, SLOT(scrollUp()));
    acc->connectItem(acc->insertItem(Key_Down), this, SLOT(scrollDown()));
    acc->connectItem(acc->insertItem(Key_Prior), this, SLOT(pageUp()));
    acc->connectItem(acc->insertItem(Key_Next), this, SLOT(pageDown()));
    acc->connectItem(acc->insertItem(Key_Left), this, SLOT(scrollLeft()));
    acc->connectItem(acc->insertItem(Key_Right), this, SLOT(scrollRight()));
    acc->connectItem(acc->insertItem(Key_Home), this, SLOT(jumpTop()));
    acc->connectItem(acc->insertItem(Key_End), this, SLOT(jumpBottom()));

    first_drag_row = prev_drag_row = -1;
    autoscrolling = FALSE;
}

// set background colour according to appearance
void TableBody::setBackground()
{
    switch(htable->appearance()) {
    case HeadedTable::macOS7:
	background = white;
	break;
    case HeadedTable::macOS8:
	background = QColor(0xeeeeee);
	break;
    default:
	background = white;
	break;
    }
    setBackgroundColor(background);
}

void TableBody::paintCell(QPainter *p, int row, int col)
{
    int gap = htable->leftGap(col);
    
    int w = cellWidth(col);
    if(htable->appearance() == HeadedTable::macOS8) {
	p->setPen(white);
	p->drawLine(0, cellheight - 1, w, cellheight - 1);
    }
    if(htable->isSelected(row)) {
	p->setPen(colorGroup().base());

	//p->setBackgroundColor(yellow);
	if(style() == WindowsStyle)
	  p->setBackgroundColor(kapp->winStyleHighlightColor());
	else
	  p->setBackgroundColor(colorGroup().text());

	int h = cellheight;
	if(htable->appearance() == HeadedTable::macOS8) h--;
	p->eraseRect(0, 0, w, h);
    } else {
	p->setPen(colorGroup().text());
	if(htable->appearance() == HeadedTable::macOS8
	   && col == htable->sortedCol()) {
	    p->setBackgroundColor(0xdddddd);
	    p->eraseRect(0, 0, w, cellheight - 1);
	} else
	    p->setBackgroundColor(background);
    }
    p->drawText(gap, 0, w - gap, cellheight,
		AlignVCenter | htable->alignment(col),
		htable->text(row, col));
    p->setBackgroundColor(background);
}

void TableBody::scrollUp()
{
    setYOffset(yOffset() - cellheight);
}

void TableBody::scrollDown()
{
    setYOffset(yOffset() + cellheight);
}

void TableBody::pageUp()
{
    setYOffset(yOffset() - viewHeight());
}

void TableBody::pageDown()
{
    setYOffset(yOffset() + viewHeight());
}

void TableBody::scrollLeft()
{	
    setXOffset(xOffset() - cellheight);
}

void TableBody::scrollRight()
{
    int x = xOffset() + cellheight;
    int maxx = totalWidth() - viewWidth();
    setXOffset(x > maxx ? maxx : x);
}

void TableBody::jumpTop()
{
    setYOffset(0);
}

void TableBody::jumpBottom()
{
    setYOffset(maxYOffset());
}

int TableBody::cellWidth(int col)
{
    return htable->actualColWidth(col);
}

void TableBody::updateRow(int row)
{
    for(int col = 0; col < htable->ncols; col++)
	updateCell(row, col);
}

void TableBody::mousePressEvent(QMouseEvent *e)
{
    int row = findRow(e->pos().y());
    if(row==-1)
      return;
    if((htable->options & HTBL_ROW_SELECTION) && e->button() == LeftButton) {
	if(row != -1) {
	    if(!(e->state() & ShiftButton)
	       && (htable->numSelected() > 1 || !htable->isSelected(row)))
	       htable->selectOnly(row);
	    else
		htable->setSelected(row, !htable->isSelected(row));
	    first_drag_row = prev_drag_row = row;
	}
    } else if((htable->options & HTBL_ROW_CONTEXT_MENU)
	      && e->button() == RightButton) {
	if((htable->options & HTBL_ROW_SELECTION) && !htable->isSelected(row))
	    htable->selectOnly(row);
	htable->emit_right_click_signal(mapToGlobal(e->pos()));
    }
}

void TableBody::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(htable->options & HTBL_ROW_DOUBLE_CLICK && e->button() == LeftButton) {
	int row = findRow(e->pos().y());
	if(row != -1) {
	    if(htable->options & HTBL_ROW_SELECTION
	       && !htable->isSelected(row))
		htable->selectOnly(row);
	    htable->emit_double_click_signal(row);
	}
    }
}

void TableBody::mouseMoveEvent(QMouseEvent *e)
{
    if(!(htable->options & HTBL_ROW_SELECTION))
	return;
    int row = findRow(e->pos().y());
    if(row != prev_drag_row) {
	if(row == -1) {
	    if(!autoscrolling) {
		// dragging outside table, cause scrolling
		scrolldir = (e->pos().y() < 0) ? UP : DOWN;
		killTimers();
		startTimer(scroll_delay);
		autoscrolling = TRUE;
	    }
	} else {
	    killTimers();
	    autoscrolling = FALSE;
	    dragSelectTo(row);
	}
    }
}

void TableBody::mouseReleaseEvent(QMouseEvent *)
{
    if(autoscrolling) {
	killTimers();		// no more autoscrolling
	first_drag_row = prev_drag_row = -1;
	autoscrolling = FALSE;
    }
}

void TableBody::timerEvent(QTimerEvent *)
{
    if(!autoscrolling) return;
    killTimers();
    if(scrolldir == UP) {
	int top = topCell();
	setTopCell((top > 1) ? top - 1 : 0);
	dragSelectTo(topCell());
    } else {
	setTopCell(topCell() + 1);
	dragSelectTo(lastRowVisible());
    }
    startTimer(scroll_delay);
}

// change drag selection point from previous drag position to row
void TableBody::dragSelectTo(int row)
{
    int dir = (row > prev_drag_row) ? 1 : -1;
    if((prev_drag_row - first_drag_row) * dir >= 0) {
	// moving away from start point
	for(int i = prev_drag_row + dir; i - dir != row; i += dir)
	    htable->setSelected(i, TRUE);
    } else {
	// moving towards start point
	for(int i = prev_drag_row; i != row; i += dir)
	    htable->setSelected(i, FALSE);
    }
    prev_drag_row = row;
}

// HeadedTable: the actually useable class

HeadedTable::HeadedTable(QWidget *parent, int opts)
            : QWidget(parent),
	      options(opts)
{
    QFont f = font();
    f.setBold(FALSE);
    setFont(f);
    fm = new QFontMetrics(font());
    current_appearance = none;
    head = new TableHead(this);
    body = new TableBody(this);
    setAppearance(macOS8, FALSE);
    sorted_col = -1;
    nrows = ncols = -1;
    nselected = 0;

    // connect keyboard shortcuts
    QAccel *acc = new QAccel(this);
    acc->connectItem(acc->insertItem(Key_Up),
		     body, SLOT(scrollUp()));
    acc->connectItem(acc->insertItem(Key_Down),
		     body, SLOT(scrollDown()));
    acc->connectItem(acc->insertItem(Key_Left),
		     body, SLOT(scrollLeft()));
    acc->connectItem(acc->insertItem(Key_Right),
		     body, SLOT(scrollRight()));
    acc->connectItem(acc->insertItem(Key_Prior),
		     body, SLOT(pageUp()));
    acc->connectItem(acc->insertItem(Key_Next),
		     body, SLOT(pageDown()));
    acc->connectItem(acc->insertItem(Key_Home),
		     body, SLOT(jumpTop()));
    acc->connectItem(acc->insertItem(Key_End),
		     body, SLOT(jumpBottom()));

    // synchronize horizontal scrolling of head and body
    connect(body->horizontalScrollBar(), SIGNAL(valueChanged(int)),
	    head, SLOT(scrollSideways(int)));
}

void HeadedTable::setNumRows(int rows)
{
    for(int i = 0; i < rows; i++)
	selected[i] = FALSE;
    nselected = 0;
    body->setNumRows(rows);
    nrows = rows;
}

void HeadedTable::setNumCols(int cols)
{
    if(cols != ncols) {
	head->setNumCols(cols);
	body->setNumCols(cols);
	ncols = cols;
	resetWidths();
    }
}

void HeadedTable::setSortedCol(int col)
{
    if(col != sorted_col) {
	int old_sorted = sorted_col;
	sorted_col = col;
	if(old_sorted != -1)
	    updateHeading(old_sorted);
	if(col != -1)
	    updateHeading(col);
    }
}

void HeadedTable::setSelected(int row, bool sel, bool update)
{
    if(isSelected(row) != sel) {
	selected[row] = sel;
	if(sel)
	    nselected++;
	else
	    nselected--;
	if(update) {
	    body->updateRow(row);
	    emit rowSelected(row);
	}
    }
}

void HeadedTable::clearAllSelections()
{
    for(int row = 0; row < nrows; row++)
	setSelected(row, FALSE);
}

void HeadedTable::selectOnly(int row)
{
    for(int r = 0; r < nrows; r++)
	setSelected(r, r == row);
}

void HeadedTable::paintEvent(QPaintEvent *)
{
    if(appearance() == macOS7) {
	QPainter p;
	p.begin(this);
	int ly = body->y() - separator_width;
	p.drawLine(0, ly, width() - 1, ly);
	p.drawLine(0, ly + 3, width() - 1, ly + 3);
	p.end();
    }
}

void HeadedTable::resizeEvent(QResizeEvent *)
{
    head->setGeometry(0, 0, width(), head->cellheight);
    int ybody = head->height() + separator_width;
    body->setGeometry(0, ybody, width(), height() - ybody);
}

void HeadedTable::emit_click_signal(int col)
{
    emit titleClicked(col);
}

void HeadedTable::emit_double_click_signal(int row)
{
    emit doubleClicked(row);
}

void HeadedTable::emit_right_click_signal(QPoint where)
{
    emit rightClickedRow(where);
}

void HeadedTable::emit_right_click_heading_signal(QPoint where, int col)
{
    emit rightClickedHeading(where, col);
}

// default implementation returns a null string (no tip displayed)
QString HeadedTable::tipText(int)
{
    return "";
}

// updates the table size of the table body (size and presence of scrollbars)
void HeadedTable::updateTableSize()
{
    body->updateTableSize();
}

// move to top and repaint
void HeadedTable::topAndRepaint()
{
    body->setAutoUpdate(FALSE);
    body->setYOffset(0);
    repaintHead();
    repaintBody();
    body->setAutoUpdate(TRUE);
}

void HeadedTable::repaintAll()
{
    repaint();
    repaintHead();
    repaintBody();
}

// compute width of text in cell with current font
int HeadedTable::textWidth(const char *s)
{
    return fm->width(s);

}

// clear (i.e. repaint) table to the right of all columns, if visible
void HeadedTable::clearRight()
{
    QRect vrh = head->viewRect();
    int lastx = head->totalWidth() - head->xOffset();
    if(lastx < vrh.width()) {
	head->repaint(lastx + vrh.x(), vrh.y(),
		      vrh.width() - lastx, vrh.height());
	QRect vrb = body->viewRect();
	body->repaint(lastx + vrb.x(), vrb.y(),
		      vrb.width() - lastx, vrb.height());
    }
}

// changes current appearance and repaints
void HeadedTable::setAppearance(Appearance a, bool update)
{
    if(a == appearance())
	return;
    current_appearance = a;
    head->setBackground();
    body->setBackground();
    switch(a) {
    case macOS7:
	{
	    separator_width = 4;
	    setBackgroundColor(white);
	}
	break;
    case macOS8:
	{
	    separator_width = 0;
	    setBackgroundColor(QColor(0xeeeeee));
	}
	break;
    default:
	break;
    }
    if(update) {
	resize(width(), height()); // send resize event
	repaint();
	repaintHead();
	repaintBody();
    }
}

// return actual (computed if necessary) column width
int HeadedTable::actualColWidth(int col)
{
    int w = widths[col];	// use cached first
    if(w < 0) {
	w = colWidth(col);
	if(w < 0)
	    w = widths[col] = computedWidth(col);
    }
    return w;
}

// return true if column width actually changed
// (always true if no column width has been calculated)
bool HeadedTable::widthChanged(int col)
{
    if(colWidth(col) >= 0) return FALSE; // fixed columns never change
    int w = computedWidth(col), oldw = widths[col];
    widths[col] = w;
    return w != oldw;
}

int HeadedTable::computedWidth(int col)
{
    int w = 0;
    for(int i = 0; i < numRows(); i++) {
	int sw = textWidth(text(i, col));
	if(sw > w) w = sw;
    }
    // don't forget the width of the heading
    int hw = textWidth(title(col)) + 4;
    if(hw > w) w = hw;
    w += leftGap(col);
    return w;
}

void HeadedTable::resetWidths()
{
    for(int i = 0; i < numCols(); i++)
	widths[i] = -1;
}
