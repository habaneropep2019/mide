#ifndef _KHEADER_H
#define _KHEADER_H

#include <qframe.h>

/**
* A widget that provides column or row headers for a table view
* widget. It can be configured to allow each header to be resizable
* and/or selectable. The widget sends a signal during resize,
* after resize, and when a header has been pressed (selected).
*
* @short A widget that provides column or row headers.
* @author Paul Kendall <paul@orion.co.nz>
* @version 0.1
*/
class KHeader : public QFrame {
	Q_OBJECT

public:
	enum { Buttons = 1, Vertical = 2, Resizable = 4 };

	/**
	* The constructor for a KHeader widget. All parameters have a default of 0.
	* @Doc:
	* \begin{itemize}
	* \item { \tt The parent widget. }
	* \item { \tt The name of the widget. }
	* \item { \tt The number of headers (columns or rows). }
	* \item { \tt Flags: this parameter specifies whether the header widget
	* displays row (Vertical) or column headers, if the headers are resizable
	* (Resizable) and if they are selectable (Buttons). These flags are the
	* defaults used for creating headers when using setNumHeaders().
	* \end{itemize}
	*/
	KHeader( QWidget *parent = 0, const char *name = 0,
			 int numHeaders = 0, int aflags = 0 );

	virtual ~KHeader();

	/**
	* Set the number of headers.
	* To change the number of headers displayed in the widget,
	* you must call this method. It can be used to set the initial number
	* or dynamically change the number of headers displayed. The headers
	* are created using the default flags set by the construction of the
	* widget.
	*/
	void setNumHeaders( int numHeaders );

	/**
	* Return the number of headers set in the widget.
	*/
	int numHeaders();

	/**
	* Change/Set the width of a header.
	*/
	void setHeaderSize( int header, int width );

	/**
	* Set the text to display in the header, and the text alignment.
	* The default alignment is AlignCenter.
	*/
	void setHeaderLabel( int header, const char *text, int align = AlignCenter );

	/**
	* Set the flags for an individual header.  This can be used to override
	* the defaults supplied in the construction of the widget.
	*/
	void setHeaderFlags( int header, int flags );

	/**
	* Set the size and position (viewable area) of the widget.
	* The height of the widget is automatically calculated as the height
	* of the widget's font, plus a 4 pixel suround, for horizontal
	* instantiations (i.e. column headers).
	*/
	void setGeometry( int x, int y, int w, int h );

	/**
	* Set the size (viewable area) of the widget.
	* The height of the widget is automatically calculated as the height
	* of the widget's font, plus a 4 pixel suround, for horizontal
	* instantiations (i.e. column headers).
	*/
	virtual void resize( int w, int h );

signals:
	/**
	* The size of the given header has changed to size.
	*/
	void sizeChanged( int header, int size );

	/**
	* The size of the given header is changing, and is currently size.
	*/
	void sizeChanging( int header, int size );

	/**
	* The given header has been selected (clicked).
	*/
	void selected( int header );

public slots:
	/**
	* This slot changes the offset of the headers within the widget.
	* It is usually connected to a slider/scrollbar so if a table view
	* is scrolled, the scrolling action will also scroll the attached
	* headers.
	*/ 
	void setOrigin( int pos );

protected:
	virtual void paintEvent( QPaintEvent *pev );
	virtual void mousePressEvent( QMouseEvent *mev );
	virtual void mouseReleaseEvent( QMouseEvent *mev );
	virtual void enterEvent( QEvent *ev );
	virtual void leaveEvent( QEvent *ev );
	virtual void fontChange( const QFont &oldFont );

private:
	bool eventFilter( QObject *, QEvent * );
	void adjustHeaderSize( int start, int delta );

	QArray<class KHeaderItem*> labels;
	int divider;
	int divstart;
	int m_offset;
	int m_selected;
	int m_temp_sel;
	int m_flags;
	int m_resizing;
	QCursor m_defCursor;
};

#endif
