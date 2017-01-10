/**********************************************************************

	--- Dlgedit generated file ---

	File: Paper.cpp
	Last generated: Tue Jun 24 10:35:56 1997

 *********************************************************************/

#include "Paper.h"
#include "PaperData.h"
#include "Paper.moc"
#include "PaperData.moc"

#define Inherited PaperData

Paper::Paper
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{

  

}


Paper::~Paper()
{
}

void Paper::setWidgets(){

  if (!data)
    return;
  
  int count = FormatComboBox->count();

  if (count == 0)
    return;

  for ( int i = 0; i < count; i++){

    if (strcmp(FormatComboBox->text(i), data->Format.data()) == 0){
      FormatComboBox->setCurrentItem(i);
      break;
    }
  }
  
  numberofcopies->setText(data->Copies.data());
  LengthEdit->setText(data->Formlines.data());
  
  if( strcmp(data->Orientation.data(),"Portrait") == 0){
    PortraitCheckBox->setChecked(TRUE);
    LandscapeCheckBox->setChecked(FALSE);
  }
  else{
    PortraitCheckBox->setChecked(FALSE);
    LandscapeCheckBox->setChecked(TRUE);
  }

  if ( strcmp(data->Manualfeed.data(),"off") == 0)
    ManualFeedCheckBox->setChecked(FALSE);
  else
    ManualFeedCheckBox->setChecked(TRUE);

  if ( strcmp(data->Autocont.data(),"off") == 0)
    AutoContinueCheckBox->setChecked(FALSE);
  else
    AutoContinueCheckBox->setChecked(TRUE);


}

void Paper::setData(Data* _data){

  data = _data;

}
