#ifndef VIEWCONTROL_H
#define VIEWCONTROL_H

#include <qdialog.h>
#include <qcombo.h>
#include <qpushbt.h>


enum VCOrientation {VCPortrait=0, VCLandscape=1, VCUpsideDown=2, VCSeascape=3};

class ViewControl : public QDialog
{
  
  Q_OBJECT
public:

  ViewControl( QWidget *parent, const char *name );
  QComboBox* mediaComboBox;
  QComboBox* magComboBox;
  QComboBox* orientComboBox;
  QPushButton *applyButton, *okButton;

  void updateMedia (int media);
  void updateOrientation (int orientation);

public slots:
  void slotApplyClicked();
  void slotOKClicked();
  void disableApply (int); 
signals:
  void applyChanges();
};

#endif
