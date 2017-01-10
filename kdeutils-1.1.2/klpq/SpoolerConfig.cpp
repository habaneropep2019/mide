/*
  This file is part of klpq (C) 1998 Christoph Neerfeld
*/
/**********************************************************************

	--- Qt Architect generated file ---

	File: SpoolerConfig.cpp
	Last generated: Sat Feb 7 11:59:44 1998

 *********************************************************************/

#include <kapp.h>

#include "SpoolerConfig.h"
#include "SpoolerConfig.moc"

SpoolerConfig::SpoolerConfig( QWidget* parent, const char* name )
  : SpoolerConfigData( parent, name )
{
  setCaption( klocale->translate("Select your spooling system") );
  connect( bg_spooler, SIGNAL(clicked(int)), this, SLOT(buttonClicked(int)) );
  connect( b_ok, SIGNAL(clicked()), this, SLOT(accept()) );
  connect( b_cancel, SIGNAL(clicked()), this, SLOT(reject()) );
}

SpoolerConfig::~SpoolerConfig()
{
}

void SpoolerConfig::initConfig( QString name )
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup("klpqPpr");
  lpq_ppr = config->readEntry("lpqCommand", "ppop");
  lpc_ppr = config->readEntry("lpcCommand", "ppop");
  lprm_ppr = config->readEntry("lprmCommand", "ppop");
  config->setGroup("klpqLprng");
  lpq_lprng = config->readEntry("lpqCommand", "lpq");
  lpc_lprng = config->readEntry("lpcCommand", "/usr/sbin/lpc");
  lprm_lprng = config->readEntry("lprmCommand", "lprm");
  config->setGroup("klpqBsd");
  lpq_bsd = config->readEntry("lpqCommand", "lpq");
  lpc_bsd = config->readEntry("lpcCommand", "/usr/sbin/lpc");
  lprm_bsd = config->readEntry("lprmCommand", "lprm");
  if( name == "PPR" )
    {
      i_lpq->setText(lpq_ppr);
      i_lpc->setText(lpc_ppr);
      i_lprm->setText(lprm_ppr);
      old_button = 1;
      rb_ppr->setChecked( TRUE );
    }
  else if( name == "LPRNG" )
    {
      i_lpq->setText(lpq_lprng);
      i_lpc->setText(lpc_lprng);
      i_lprm->setText(lprm_lprng);
      old_button = 2;
      rb_lprng->setChecked( TRUE );
    }
  else
    {
      i_lpq->setText(lpq_bsd);
      i_lpc->setText(lpc_bsd);
      i_lprm->setText(lprm_bsd);
      old_button = 0;
      rb_bsd->setChecked( TRUE );
    }
}

QString SpoolerConfig::saveConfig()
{
  KConfig *config = KApplication::getKApplication()->getConfig();
  QString temp;  
  if( rb_ppr->isChecked() )
    {
      lpq_ppr = i_lpq->text();
      lpc_ppr = i_lpc->text();
      lprm_ppr = i_lprm->text();
      temp = "PPR";
    }
  else if( rb_lprng->isChecked() )
    {
      lpq_lprng = i_lpq->text();
      lpc_lprng = i_lpc->text();
      lprm_lprng = i_lprm->text();
      temp = "LPRNG";
    }
  else
    {
      lpq_bsd = i_lpq->text();
      lpc_bsd = i_lpc->text();
      lprm_bsd = i_lprm->text();
      temp = "BSD";
    }
  config->setGroup("klpq");
  config->writeEntry("Spooler", temp );

  config->setGroup("klpqPpr");
  config->writeEntry("lpqCommand", lpq_ppr);
  config->writeEntry("lpcCommand", lpc_ppr);
  config->writeEntry("lprmCommand", lprm_ppr);
  config->setGroup("klpqLprng");
  config->writeEntry("lpqCommand", lpq_lprng);
  config->writeEntry("lpcCommand", lpc_lprng);
  config->writeEntry("lprmCommand", lprm_lprng);
  config->setGroup("klpqBsd"); 
  config->writeEntry("lpqCommand", lpq_bsd);
  config->writeEntry("lpcCommand", lpc_bsd);
  config->writeEntry("lprmCommand", lprm_bsd);
  return temp;
}

void SpoolerConfig::buttonClicked(int id)
{
  if( old_button == id )
    return;
  switch( old_button ) {
  case 1:
    lpq_ppr = i_lpq->text();
    lpc_ppr = i_lpc->text();
    lprm_ppr = i_lprm->text();
    break;
  case 2:
    lpq_lprng = i_lpq->text();
    lpc_lprng = i_lpc->text();
    lprm_lprng = i_lprm->text();
    break;
  default:
    lpq_bsd = i_lpq->text();
    lpc_bsd = i_lpc->text();
    lprm_bsd = i_lprm->text();
  };

  switch( id ) {
  case 1:
    i_lpq->setText(lpq_ppr);
    i_lpc->setText(lpc_ppr);
    i_lprm->setText(lprm_ppr);
    break;
  case 2:
    i_lpq->setText(lpq_lprng);
    i_lpc->setText(lpc_lprng);
    i_lprm->setText(lprm_lprng);
    break;
  default:
    i_lpq->setText(lpq_bsd);
    i_lpc->setText(lpc_bsd);
    i_lprm->setText(lprm_bsd);
  };
  old_button = id;
}
