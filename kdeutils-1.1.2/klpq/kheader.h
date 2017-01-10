
#ifndef kheader_included
#define kheader_included

#include "qframe.h"

class KHeaderItem;

class KHeader : public QFrame {
  Q_OBJECT
public:
  enum { Buttons = 1,
	 Vertical = 2,
	 Resizable = 4
  };
  
  KHeader( QWidget *parent = 0, const char *name = 0,
	   int numHeaders = 0, int flags = 0 );
  ~KHeader();
  
  void setNumHeaders( int numHeaders );
  int numHeaders();
  void setHeaderSize( int header, int width );
  int  getHeaderSize( int header );
  void setHeaderLabel( int header, const char *text, int align = AlignCenter );
  void setHeaderFlags( int header, int flags );
  void setGeometry( int x, int y, int w, int h );
  void resize( int w, int h );
  
protected:
  virtual void paintEvent( QPaintEvent *pev );
  virtual void mousePressEvent( QMouseEvent *mev );
  virtual void mouseReleaseEvent( QMouseEvent *mev );
  virtual void enterEvent( QEvent *ev );
  virtual void leaveEvent( QEvent *ev );
  virtual void fontChange( const QFont &oldFont );
  
signals:
  void sizeChanged( int header, int size );
  void sizeChanging( int header, int size );
  void selected( int header );
public slots:
  void setOrigin( int pos );

private:
  bool eventFilter( QObject *, QEvent * );
  void adjustHeaderSize( int start, int delta );

  QArray<KHeaderItem*> labels;
  int divider;
  int divstart;
  int m_offset;
  int m_selected;
  int m_temp_sel;
  int m_flags;
  int m_resizing;
  QCursor m_defCursor;
};


#endif /* kheader_included */
