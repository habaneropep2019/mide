#include "ConfAutoUpdate.h"
#include "ConfAutoUpdate.moc"

#include <kapp.h>

ConfAutoUpdate::ConfAutoUpdate(	QWidget* parent, const char* name )
  : QDialog( parent, name, TRUE ), ConfAutoUpdateData( this )
{
  initMetaObject();
  setCaption( klocale->translate("Configure Auto Update") );
  s_freq->setSteps(1,20);
  s_freq->setRange(0, 180 );
  connect( (QObject*) s_freq, SIGNAL(valueChanged(int)), (QObject*) lcd_freq, SLOT(display(int)) );
  connect( b_ok, SIGNAL(clicked()), this, SLOT(accept()) );
  connect( b_cancel, SIGNAL(clicked()), this, SLOT(reject()) );
}


ConfAutoUpdate::~ConfAutoUpdate()
{
}
