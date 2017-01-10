// -*- C++ -*-

#ifndef ConfAutoUpdate_included
#define ConfAutoUpdate_included

#include <qdialog.h>

#include "ConfAutoUpdateData.h"

class ConfAutoUpdate : public QDialog,
		       private ConfAutoUpdateData
{
  Q_OBJECT;
public:
  ConfAutoUpdate ( QWidget* parent = NULL, const char* name = NULL );
  virtual ~ConfAutoUpdate();

  void setFreq( int sec ) { s_freq->setValue( sec ); }
  int getFreq() { return s_freq->value(); }
  
protected slots:

};
#endif // ConfAutoUpdate_included
