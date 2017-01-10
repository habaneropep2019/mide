/**********************************************************************

	--- Dlgedit generated file ---

	File: Printer.cpp
	Last generated: Tue Jun 24 12:34:51 1997

 *********************************************************************/

#include "Printer.h"
#include "Printer.moc"
#include "PrinterData.moc"

#define Inherited PrinterData

Printer::Printer
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


Printer::~Printer()
{
}


void Printer::setWidgets(){

    if (!data)
    return;
  
  int count = DensityComboBox->count();

  if (count == 0){
    fprintf(stderr,"DensityCombo empty\n");
    return;
  }
  
  int item = atoi(data->Density.data()) - 1;
  
  if(item < 0 || item > DensityComboBox->count() -1)
    fprintf(stderr, "Density out of range %d\n", item +1);

  DensityComboBox->setCurrentItem(item);
  

  count = ResolutionComboBox->count();

  if (count == 0){
    fprintf(stderr,"ResolutionCombo empty\n");
    return;
  }

  for ( int i = 0; i < count; i++){

    if (strcmp(ResolutionComboBox->text(i), data->RET.data()) == 0){
      ResolutionComboBox->setCurrentItem(i);
      break;
    }
  }
  
  if ( strcmp(data->Economode.data(),"off") == 0){
    EconomyButton->setChecked(FALSE);
    PresentationButton->setChecked(TRUE);
  }
  else{
    EconomyButton->setChecked(TRUE);
    PresentationButton->setChecked(FALSE);
  }

  if ( strcmp(data->Resolution.data(),"600") == 0){
    Dpi600Button->setChecked(TRUE);
    Dpi300Button->setChecked(FALSE);
  }
  else{
    Dpi600Button->setChecked(FALSE);
    Dpi300Button->setChecked(TRUE);
  }

}


void Printer::addPrinter(char* name){

  PrinterComboBox->insertItem(name);

}
void Printer::setData(Data* _data){

  data = _data;


}

