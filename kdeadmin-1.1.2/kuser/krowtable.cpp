#include "krowtable.moc"
#include "qkeycode.h"

KRowTable::KRowTable( Flags aflags, QWidget *parent, const char *name )
	: QTableView( parent, name )
{
	init(aflags);
}

KRowTable::KRowTable( int rows, int cols, Flags aflags, QWidget *parent, const char *name )
	: QTableView( parent, name )
{
	init(aflags);
	setNumRows( rows );
	setNumCols( cols );
}

KRowTable::~KRowTable()
{
	clear();
}

void KRowTable::init(Flags aflags)
{
	m_flags = aflags;
	current_row = -1;
	if( aflags == SelectRow )
		current_col = -1;
	else
		current_col = 0;

	setFrameStyle( WinPanel | Sunken );
	setBackgroundColor( colorGroup().base() );

	setCellWidth( 0 );
	setCellHeight( 0 );

	setAutoUpdate( TRUE );
    setFocusPolicy( StrongFocus );
    setTableFlags( Tbl_autoScrollBars |
                   Tbl_clipCellPainting |
                   Tbl_smoothScrolling);

	connect( (QObject*)horizontalScrollBar(), SIGNAL(sliderMoved(int)),
				SLOT(hSliderMovedSlot(int)) );
	connect( (QObject*)horizontalScrollBar(), SIGNAL(valueChanged(int)),
				SLOT(hSliderMovedSlot(int)) );

	connect( (QObject*)verticalScrollBar(), SIGNAL(sliderMoved(int)),
				SLOT(vSliderMovedSlot(int)) );
	connect( (QObject*)verticalScrollBar(), SIGNAL(valueChanged(int)),
				SLOT(vSliderMovedSlot(int)) );

	autoDeleteFlag = FALSE;
}

void KRowTable::setCellWidth( int width )
{
	QTableView::setCellWidth( width );
}

void KRowTable::setCellHeight( int height )
{
	QTableView::setCellHeight( height );
}

void KRowTable::clear()
{
	if( autoDelete() )
		for(int i=0 ; i<numRows() ; i++) {
			delete m_rows[i];
			m_rows[i] = 0;
		}
	if( autoUpdate() )
		repaint();
}

void KRowTable::setAutoDelete( bool autodelete )
{
	autoDeleteFlag = autodelete;
}

bool KRowTable::autoDelete() const
{
	return autoDeleteFlag;
}

void KRowTable::setNumCols( int cols )
{
	int oldsize = m_colWidths.size();
	m_colWidths.resize( cols );
	for( int i=oldsize ; i<cols ; i++)
		m_colWidths[i] = 0;
	QTableView::setNumCols( cols );
}

void KRowTable::setNumRows( int rows )
{
	int oldsize = m_rowHeights.size();
	m_rowHeights.resize( rows );
	m_rows.resize( rows );
	for( int i=oldsize ; i<rows ; i++) {
		m_rowHeights[i] = 0;
		m_rows[i] = 0;
	}
	QTableView::setNumRows( rows );
}

int KRowTable::cellWidth( int col )
{
	int width = QTableView::cellWidth();
	if( width != 0 )
		return width;
	return m_colWidths[col];
}

void KRowTable::setHighlightColumn( int col )
{
	current_col = col;
	repaint();
}

void KRowTable::setColumnWidth( int col, int width )
{
	m_colWidths[col] = width;
	resizeEvent(0);
	repaint();
}

int KRowTable::cellHeight( int row )
{
	int height = QTableView::cellHeight();
	if( height != 0 )
		return height;
	return m_rowHeights[row];
}

void KRowTable::setRowHeight( int row, int height )
{
	m_rowHeights[row] = height;
	resizeEvent(0);
	repaint();
}

void KRowTable::vSliderMovedSlot(int value)
{
	emit(vSliderMoved(value));
}

void KRowTable::hSliderMovedSlot(int value)
{
	emit(hSliderMoved(value));
}

void KRowTable::focusInEvent( QFocusEvent * )
{
	if( current_row != -1 && current_col != -1 )
		updateCell( current_row, current_col, FALSE );
	else if( current_row != -1 ) {
		for( int i=0 ; i<numCols() ; i++ ) {
			updateCell( current_row, i, FALSE );
		}
	}
}

void KRowTable::focusOutEvent( QFocusEvent * )
{
	if( current_row != -1 && current_col != -1 )
		updateCell( current_row, current_col, FALSE );
	else if( current_row != -1 ) {
		for( int i=0 ; i<numCols() ; i++ ) {
			updateCell( current_row, i, FALSE );
		}
	}
}

void KRowTable::paletteChange( const QPalette &oldPalette )
{
	setBackgroundColor( colorGroup().base() );
}

void KRowTable::paintCell( QPainter *p, int row, int col )
{
	QPen oldPen = p->pen();
	QColor oldBackground = p->backgroundColor();

	QColorGroup g = colorGroup();
	p->setBackgroundColor( g.base() );
	p->setPen( g.text() );

	if( current_row == row && 
		( current_col == col || current_col == -1 ) )
	{
		QColor fc;
		if( style() == WindowsStyle )
			fc = darkBlue;
		else
			fc = g.text();
		p->fillRect( 0, 0, cellWidth( col ), cellHeight( row ), fc );
		p->setPen( g.base() );
		p->setBackgroundColor( g.text() );
	}

	KRow *cell = getRow( row );
	if( cell != 0 )
		cell->paint( p, col, cellWidth(col) );

	if( current_row == row &&
	    ( current_col == col || current_col == -1 ) &&
		hasFocus() )
	{
		if( style() == WindowsStyle )
			p->drawWinFocusRect( 1, 1, cellWidth( col )-2, cellHeight( row )-2 );
		else
		{
			p->setPen( g.base() );
			p->setBrush( NoBrush );
			p->drawRect( 1, 1, cellWidth( col )-2, cellHeight( row )-2 );
		}
	}

	p->setPen( oldPen );
	p->setBackgroundColor( oldBackground );
}

KRow *KRowTable::selectedRow()
{
	if( current_row == -1 )
		return 0;
	return getRow( current_row );
}

void KRowTable::appendRow( KRow *cell )
{
	setNumRows( numRows() + 1 );
	m_rows[numRows()-1] = cell;
	if( autoUpdate() )
		repaint();
}

void KRowTable::insertRow( KRow *cell, int row )
{
	if( row < 0 )
		row=0;

	if( row >= numRows() )
		setNumRows( row+1 );
        else {
	        setNumRows( numRows() + 1 );

		for( int i=numRows()-1 ; i>row ; i-- ) {
		        m_rowHeights[i] = m_rowHeights[i-1];
		        m_rows[i] = m_rows[i-1];
		}
	}
	m_rowHeights[row] = 0;
	m_rows[row] = cell;

	if( autoUpdate() )
		repaint();
}

bool KRowTable::replaceRow( KRow *cell, int row )
{
	if( row < 0 || row >= numRows() )
		return FALSE;

	if( autoDelete() && m_rows[row] != 0 )
		delete m_rows[row];
	m_rows[row] = cell;

	if( autoUpdate() )
		repaint();

	return TRUE;
}

KRow *KRowTable::getRow( int row )
{
	if( row >= numRows() )
		return 0;
	return m_rows[row];
}

void KRowTable::mousePressEvent( QMouseEvent *e )
{
	if( m_flags & SelectCell )
		setCurrentRow( findRow( e->pos().y() ), findCol( e->pos().x() ) );
	else if( m_flags & SelectFixed )
		setCurrentRow( findRow( e->pos().y() ), current_col );
	else
		setCurrentRow( findRow( e->pos().y() ), -1 );
}

void KRowTable::mouseDoubleClickEvent( QMouseEvent *e )
{
	if( m_flags == SelectRow ) {
		if( current_row != -1 )
			emit selected( current_row, current_col );
	} else {
		if( current_col != -1 && current_row != -1 )
			emit selected( current_row, current_col );
	}
	repaint();
}

void KRowTable::setCurrentRow( int row, int col )
{
	int old_row = current_row;
	current_row = row;
	int old_col = current_col;
	current_col = col;
	updateCell( old_row, old_col );
	updateCell( current_row, current_col, FALSE );
	emit highlighted( current_row, current_col );
	repaint();
}

void KRowTable::keyPressEvent( QKeyEvent *e )
{
	if( m_colWidths.size() == 0 )
		return;
	if( current_row == -1 )
		setCurrentRow( topCell(), current_col );

	int pageSize;

	switch( e->key() )
	{
	case Key_Up:
		if( current_row > 0 )
		{
			setCurrentRow( current_row-1, current_col );
			if( current_row < topCell() )
				setTopCell( current_row );
		}
		break;

	case Key_Down:
		if( current_row < numRows()-1 )
		{
			setCurrentRow( current_row + 1, current_col );
			if( current_row > lastRowVisible() )
				setTopCell( topCell() + current_row - lastRowVisible() );
		}
		break;

	case Key_Next:
		pageSize = lastRowVisible() - topCell();
		if( rowIsVisible( current_row ) )
			setTopCell( QMIN( topCell() + pageSize, numRows()-pageSize ) );
		else
			setTopCell( QMIN( current_row + pageSize, numRows()-pageSize ) );
		setCurrentRow( QMIN( current_row + pageSize, numRows()-1 ), current_col );
		break;

	case Key_Prior:
		pageSize = lastRowVisible() - topCell();
		if( rowIsVisible( current_row ) )
			setTopCell( QMAX( topCell() - pageSize, 0 ) );
		else
			setTopCell( QMAX( current_row - pageSize, 0 ) );
		setCurrentRow( QMAX( current_row - pageSize, 0 ), current_col );
		break;

	case Key_Return:
	case Key_Enter:
		if( current_row != -1 && current_col != -1 )
			emit selected( current_row, current_col );
		break;

	default:
		e->ignore();
		break;
	}
}

void KRowTable::setSelectColumn( int col )
{
	if( m_flags == SelectRow )
		col = -1;
	current_col = col;
	repaint();
}

int KRowTable::selectColumn()
{
	return current_col;
}
