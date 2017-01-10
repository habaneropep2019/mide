/**********************************************************************

	--- Qt Architect generated file ---

	File: SpoolerConfig.h
	Last generated: Sat Feb 7 11:59:44 1998

 *********************************************************************/

#ifndef SpoolerConfig_included
#define SpoolerConfig_included

#include "SpoolerConfigData.h"

class QString;

class SpoolerConfig : public SpoolerConfigData
{
  Q_OBJECT
public:
  SpoolerConfig ( QWidget* parent = 0L, const char* name = 0L );
  virtual ~SpoolerConfig();

  void    initConfig( QString name);
  QString saveConfig();

protected slots:
  void buttonClicked(int id);

protected:
  //void spoolerChanged( QString name );

  QString lpq_bsd;
  QString lpc_bsd;
  QString lprm_bsd;

  QString lpq_ppr;
  QString lpc_ppr;
  QString lprm_ppr;

  QString lpq_lprng;
  QString lpc_lprng;
  QString lprm_lprng;

  int old_button;
};

#endif // SpoolerConfig_included
