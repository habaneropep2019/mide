/**********************************************************************

	--- Dlgedit generated file ---

	File: Fonts.cpp
	Last generated: Tue Jun 24 11:41:55 1997

 *********************************************************************/

#include "Fonts.h"
#include "FontsData.h"
#include "Fonts.moc"
#include "FontsData.moc"

#define Inherited FontsData

Fonts::Fonts
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


Fonts::~Fonts()
{
}

void Fonts::setWidgets(){

  if (!data)
    return;
  
  int count = LanguageComboBox->count();

  if (count == 0){
    fprintf(stderr,"LanguageCombo empty\n");
    return;
  }

  for ( int i = 0; i < count; i++){

    if (strcmp(LanguageComboBox->text(i), data->Language.data()) == 0){
      LanguageComboBox->setCurrentItem(i);
      break;
    }
  }

  count = SymbolSetCombo->count();

  if (count == 0){
    fprintf(stderr,"SymboSetCombo empty\n");
    return;
  }

  for ( int i = 0; i < count; i++){

    if (strcmp(SymbolSetCombo->text(i), data->Symset.data()) == 0){
      SymbolSetCombo->setCurrentItem(i);
      break;
    }
  }
  
  Font->setText(data->Fontnumber.data());
  Pitch->setText(data->Pitch.data());
  PointSize->setText(data->Ptsize.data());

}

void Fonts::setData(Data *_data){

  data = _data;
  

}
