/**********************************************************************

	--- Dlgedit generated file ---

	File: Misc.cpp
	Last generated: Tue Jun 24 12:56:12 1997

 *********************************************************************/

#include "Misc.h"
#include "Misc.moc"
#include "MiscData.moc"


#define Inherited MiscData

Misc::Misc
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


Misc::~Misc()
{
}


void Misc::setWidgets(){

    if (!data)
    return;
  
  int count = PageProtectCombo->count();

  if (count == 0){
    fprintf(stderr,"PageProtectCombo empty\n");
    return;
  }

  for ( int i = 0; i < count; i++){

    if (strcmp(PageProtectCombo->text(i), data->Pageprotect.data()) == 0){
      PageProtectCombo->setCurrentItem(i);
      break;
    }
  }


  count = PowerSaveCombo->count();

  if (count == 0){
    fprintf(stderr,"PwerSaveCombo empty\n");
    return;
  }

  for ( int i = 0; i < count; i++){

    if (strcmp(PowerSaveCombo->text(i), data->Powersave.data()) == 0){
      PowerSaveCombo->setCurrentItem(i);
      break;
    }
  }
  
  if ( strcmp(data->Termination.data(),"unix") == 0){
    UnixButton->setChecked(TRUE);
    DosButton->setChecked(FALSE);
  }
  else{
    UnixButton->setChecked(FALSE);
    DosButton->setChecked(TRUE);
  }


}

void Misc::setData(Data* _data){

  data = _data;


}
