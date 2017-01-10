#ifndef _KROWTABLE_H
#define _KROWTABLE_H

#include "qtablevw.h"
#include "qpainter.h"
#include "qlist.h"
#include "qarray.h"

class KRow
{
public:
	KRow() {};
	virtual ~KRow() {};

protected:
	friend class KRowTable;
	virtual void paint( QPainter *painter, int col, int width ) = 0;

private:    // Disabled copy constructor and operator=
	KRow( const KRow & ) {}
	KRow& operator=( const KRow & ) { return *this; }
};

class KRowTable : public QTableView
{
	Q_OBJECT
protected:
	enum Flags {SelectRow, SelectCell, SelectFixed};

public:
	KRowTable( Flags aflags=SelectRow, QWidget *parent = 0, const char *name = 0 );
	KRowTable( int rows, int cols, Flags aflags=SelectRow, QWidget *parent = 0, const char *name = 0 );
	~KRowTable();

	KRow *selectedRow();
	void setNumCols( int cols );
	void setNumRows( int rows );
	void appendRow( KRow *cell );
	void insertRow( KRow *cell, int row );
	bool replaceRow( KRow *cell, int row );
	void setCurrentRow( int row, int col );
	void setHighlightColumn( int col );
	KRow *getRow( int row );
	void clear();
	void setAutoDelete( bool autodelete );
	bool autoDelete() const;
	void setCellWidth( int width );
	void setCellHeight( int height );
	void setSelectColumn( int col );
	int selectColumn();

protected:
	int cellWidth( int col );
	int cellHeight( int row );

	virtual void paintCell( QPainter *p, int row, int col );
	virtual void mousePressEvent( QMouseEvent *e );
	virtual void mouseDoubleClickEvent( QMouseEvent *e );
	virtual void keyPressEvent( QKeyEvent *e );
	virtual void focusInEvent( QFocusEvent *e );
	virtual void focusOutEvent( QFocusEvent *e );
	virtual void paletteChange( const QPalette &oldPalette );

signals:
	void selected(int row, int col);
	void highlighted(int row, int col);
	void hSliderMoved(int value);
	void vSliderMoved(int value);

public slots:
	void setColumnWidth(int col, int width);
	void setRowHeight(int row, int height);

private slots:
	void hSliderMovedSlot(int value);
	void vSliderMovedSlot(int value);

private:
	void init(Flags aflags);
	QArray<KRow *> m_rows;
	QArray<int> m_colWidths;
	QArray<int> m_rowHeights;
	bool autoDeleteFlag;
	int	current_col;
	int	current_row;
	int m_flags;
};

#endif // _KROWTABLE_H
