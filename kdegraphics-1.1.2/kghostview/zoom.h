#ifndef ZOOM_H
#define ZOOM_H

#include <qdialog.h>
#include <qcombobox.h>
#include <kbuttonbox.h>

class Zoom : public QDialog {
      Q_OBJECT
      
public:
      Zoom( QWidget *parent = 0, const char *name = 0 );
      QComboBox *sbMag;
      
      unsigned int zoomFactor (void) { return (unsigned int) mag; }
      
      void updateZoom (int mag);
public slots:
	//      void slotOk();

signals:
      void applyChange();

protected:
      const char *withPercent (int n);
      unsigned int reverseArray (int magnification);

protected slots:
	//      void slotValueDecreased ();
	// void slotValueIncreased ();
	void slotZoom (const char *);

private:
      int mag;
      int *mags;
      QButton *apply, *okButton;
      QString wp;
      
};

#endif

