/****************************************************************************
**
** A simple widget to mark and select entries in a list.
**
** Copyright (C) 1997 by Markku Hihnala. 
** This class is freely distributable under the GNU Public License.
**
*****************************************************************************/

#ifndef MARKLIST_H
#define MARKLIST_H

#include <qtablevw.h>
#include "kpopmenu.h"
#include <qstrlist.h>
#include <qlabel.h>

class MarkListTableItem
{
public:
    MarkListTableItem( const char *s ) : marked(0), selected(0)  { _text = s; }
	void	setMark( bool flag )	{ marked = flag; }
	bool	mark()			{ return marked; }
	void	setSelect( bool flag )	{ selected = flag; }
	bool	select()		{ return selected; }
	const char *text()		{ return _text.data(); }

    private:
	bool	marked;
	bool	selected;
	QString _text;
}; 

class MarkListTable: public QTableView
{
	Q_OBJECT

public:
	MarkListTable( QWidget * parent = 0, const char * name = 0 );
	~MarkListTable() { }
	QStrList *	markList();
	void	insertItem ( const char *text, int index=-1);
	void	setAutoUpdate ( bool enable );
	void	clear();
	int	rowHeight();
	const char *text( int index );
	const unsigned int count (void) { return items.count(); }

	QColor selectColor;
	QColor selectTextColor;

public slots:
	void	select(int);
	void	markSelected();
	void	markAll();
	void	markEven();
	void	markOdd();
	void	toggleMarks();
	void	removeMarks();

signals:
	void	selected( int index );
	void	selected( const char * text );

protected:
	void	mousePressEvent ( QMouseEvent* );
	void	mouseReleaseEvent ( QMouseEvent* ) {}
	void	mouseMoveEvent ( QMouseEvent* );
	void	paintCell( QPainter *p, int row, int col );
	int	cellWidth( int );
	void	updateItem( int i );
	

private:
	void	changeMarks( int, int = 0 );
	QPoint	mouse;
	int	sel;
	QPopupMenu* pup;
	int	drag;
	QList<MarkListTableItem> items;
};

class MarkList: public QWidget
{
	Q_OBJECT
	
public:
	MarkList( QWidget * parent = 0, const char * name = 0 );
	~MarkList() { }
	QStrList *	markList();
	void	insertItem ( const char *text, int index=-1);
	void	setAutoUpdate ( bool enable );
	void	clear();
	void 	setSelectColors( QColor bg, QColor fg );
	const char *text( int index );
	const unsigned int count(void) { return listTable->count(); }

	QColor selectColor;
	QColor selectTextColor;

public slots:
	void	select(int index);
	void	markSelected();
	void	markAll();
	void	markEven();
	void	markOdd();
	void	toggleMarks();
	void	removeMarks();

signals:
	void	selected( int index );
	
protected:
	void resizeEvent( QResizeEvent * );

private slots:
	void	selectSig(int index);

private:
	QLabel *markLabel;
	QLabel *pageLabel;
	MarkListTable *listTable;
	QPixmap flagPixmap();

};

#endif 
