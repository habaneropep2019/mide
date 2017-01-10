#include "Data.h"
#include "Data.moc"
#include <kapp.h>
#include <klocale.h>

extern KApplication *mykapp;

char escapeseq[4] = "\x1b\x25\x2d";


Data::Data(){

  readSettings();
}

Data::~Data(){

}

Data Data::operator=(Data data){

  Printer       =   data.Printer.copy();
  Job_name 	=   data.Job_name.copy();
  Density 	=   data.Density.copy();
  Fontnumber 	=   data.Fontnumber.copy();
  Format 	=   data.Format.copy();
  Termination 	=   data.Termination.copy();
  RET 		=   data.RET.copy();
  Copies 	=   data.Copies.copy();
  Powersave 	=   data.Powersave.copy();
  Resolution 	=   data.Resolution.copy();
  Language 	=   data.Language.copy();
  Pageprotect 	=   data.Pageprotect.copy();
  Ptsize 	=   data.Ptsize.copy();
  Pitch 	=   data.Pitch.copy();
  Economode 	=   data.Economode.copy();
  Manualfeed 	=   data.Manualfeed.copy();
  Orientation 	=   data.Orientation.copy();
  Symset 	=   data.Symset.copy();
  Autocont 	=   data.Autocont.copy();
  Formlines 	=   data.Formlines.copy();


}

bool Data::print(){


  FILE* printpipe;
  
  QString cmd;
  cmd = "lpr ";
  cmd += "-P" ;
  cmd += Printer.data();

  //  printf("%s\n",cmd.data());

  printpipe = popen(cmd.data(),"w");
  
  if(printpipe == NULL){
    QString str;
    str.sprintf("Unable to print with:\n %s",cmd.data());
    QMessageBox::message("Sorry",str.data(),"OK");
    return FALSE;
  }
  
  fwrite(escapeseq,3,1,printpipe);
  fprintf(printpipe,"12345X@PJL\n");
  fprintf(printpipe,"@PJL JOB NAME = \"%s\"\n",		Job_name.data());
  fprintf(printpipe,"@PJL DEFAULT Density = %s\n",	Density.data());
  fprintf(printpipe,"@PJL DEFAULT Fontnumber = %s\n",	Fontnumber.data());
  fprintf(printpipe,"@PJL DEFAULT Format = %s\n",	Format.data());
  fprintf(printpipe,"@PJL DEFAULT Termination = %s\n",	Termination.data());
  fprintf(printpipe,"@PJL DEFAULT RET = %s\n",		RET.data());
  fprintf(printpipe,"@PJL DEFAULT Copies = %s\n",	Copies.data());
  fprintf(printpipe,"@PJL DEFAULT Powersave = %s\n",	Powersave.data());
  fprintf(printpipe,"@PJL DEFAULT Resolution = %s\n", 	Resolution.data());
  fprintf(printpipe,"@PJL DEFAULT Language = %s\n",	Language.data());
  fprintf(printpipe,"@PJL DEFAULT Pageprotect = %s\n",	Pageprotect.data());
  fprintf(printpipe,"@PJL DEFAULT Ptsize = %s\n",	Ptsize.data());
  fprintf(printpipe,"@PJL DEFAULT Pitch = %s\n",	Pitch.data());
  fprintf(printpipe,"@PJL DEFAULT Economode = %s\n",	Economode.data());
  fprintf(printpipe,"@PJL DEFAULT Manualfeed = %s\n",	Manualfeed.data());
  fprintf(printpipe,"@PJL DEFAULT Orientation = %s\n",	Orientation.data());
  fprintf(printpipe,"@PJL DEFAULT Symset = %s\n",      	Symset.data());
  fprintf(printpipe,"@PJL DEFAULT Autocont = %s\n",	Autocont.data());
  fprintf(printpipe,"@PJL DEFAULT Formlines = %s\n", 	Formlines.data());
  fprintf(printpipe,"@PJL EOJ NAME= \"end profile\"\n");
  fwrite(escapeseq,3,1,printpipe);
  fprintf(printpipe,"12345X\n");

  pclose(printpipe);


  return TRUE;

}

void Data::setPrinter(char *printer){


  Printer = printer;
  Printer.detach();

}


bool Data::initialize(){

  FILE* printpipe;
  
  QString cmd;
  cmd = "lpr ";
  cmd += "-P" ;
  cmd += Printer.data();

  //  printf("%s\n",cmd.data());

  printpipe = popen(cmd.data(),"w");

  if(printpipe == NULL){
    QString str;
    str.sprintf(  i18n("Unable to print with:\n %s"),cmd.data());
    QMessageBox::message(i18n("Sorry"),str.data(),i18n("OK"));
    return FALSE;
  }
  

  fwrite(escapeseq,3,1,printpipe);
  fprintf(printpipe,"12345X@PJL\n");
  fprintf(printpipe,"@PJL INITIALIZE\n");
  fwrite(escapeseq,3,1,printpipe);
  fprintf(printpipe,"12345X\n");

  pclose(printpipe);


  return TRUE;

}

bool Data::eject(){

  FILE* printpipe;
  
  QString cmd;
  cmd = "lpr ";
  cmd += "-P" ;
  cmd += Printer.data();

  //  printf("%s\n",cmd.data());

  printpipe = popen(cmd.data(),"w");
  
  if(printpipe == NULL){
    QString str;
    str.sprintf(i18n("Unable to print with:\n %s"),cmd.data());
    QMessageBox::message(i18n("Sorry"),str.data(),i18n("OK"));
    return FALSE;
  }
  

  fwrite(escapeseq,3,1,printpipe);
  fprintf(printpipe,"12345X@PJL\n");
  fprintf(printpipe,"@PJL DEFAULT @PCL ENTER LANGUAGE = PCL\n");

  char ejectseq[4] = "\x1b\x45\x0a";
  fwrite(ejectseq,3,1,printpipe);

  fwrite(escapeseq,3,1,printpipe);
  fprintf(printpipe,"12345X\n");

  pclose(printpipe);

  return TRUE;
}

bool Data::reset(){

  FILE* printpipe;
  
  QString cmd;
  cmd = "lpr ";
  cmd += "-P" ;
  cmd += Printer.data();

  //  printf("%s\n",cmd.data());

  printpipe = popen(cmd.data(),"w");
  
  if(printpipe == NULL){
    QString str;
    str.sprintf(i18n("Unable to print with:\n %s"),cmd.data());
    QMessageBox::message(i18n("Sorry"),str.data(),i18n("OK"));
    return FALSE;
  }
  
  fwrite(escapeseq,3,1,printpipe);
  fprintf(printpipe,"12345X@PJL\n");
  fprintf(printpipe,"@PJL DEFAULT RESET\n");
  fwrite(escapeseq,3,1,printpipe);
  fprintf(printpipe,"12345X\n");

  pclose(printpipe);

  return TRUE;
}

void Data::readSettings(){

  KConfig *config;
  config = mykapp->getConfig();
  config->setGroup( "Data" );

  /*  
  Printer = "lp";
  Job_name = "default job";
  Density = "3";
  Fontnumber = "0";
  Format = "Letter";
  Termination = "unix";
  RET = "medium";
  Copies = "1";
  Powersave = "15";
  Resolution = "600";
  Language = "pcl";
  Pageprotect = "auto";
  Ptsize = "12.0";
  Pitch = "10.0";
  Economode = "off";
  Manualfeed = "off";
  Orientation = "Portrait";
  Symset = "PC8";
  Autocont = "on";
  Formlines = "60"; // this should be 66 for DIN-A4
  */

  Printer 	= config->readEntry( "Printer","lp" );
  Job_name 	= config->readEntry( "Job_name","default job" );
  Density 	= config->readEntry( "Density","3" );
  Fontnumber	= config->readEntry( "Fontnumber", "0" );
  Format 	= config->readEntry( "Format", "Letter" );
  Termination	= config->readEntry( "Termination","unix" );
  RET 		= config->readEntry( "RET", "medium" );
  Copies	= config->readEntry( "Copies","1" );
  Powersave	= config->readEntry( "Powersave", "15" );
  Resolution	= config->readEntry( "Resolution","600" );
  Language 	= config->readEntry( "Language","pcl" );
  Pageprotect	= config->readEntry( "Pageprotect","auto" );
  Ptsize 	= config->readEntry( "Ptsize","12.0" );
  Pitch 	= config->readEntry( "Pitch","10.0" );
  Economode 	= config->readEntry( "Economode","off" );
  Manualfeed 	= config->readEntry( "Manualfeed","off" );
  Orientation 	= config->readEntry( "Orientation","Portrait" );
  Symset 	= config->readEntry( "Symset","PC8" );
  Autocont	= config->readEntry( "Autocont","on" );
  Formlines 	= config->readEntry( "Formlines","60" );

}

void Data::writeSettings(){

  KConfig *config;
  config = mykapp->getConfig();
  config->setGroup( "Data" );

  config->writeEntry( "Printer",Printer.data() );
  config->writeEntry( "Job_name",Job_name.data() );
  config->writeEntry( "Density",Density.data() );
  config->writeEntry( "Fontnumber", Fontnumber.data() );
  config->writeEntry( "Format", Format.data() );
  config->writeEntry( "Termination",Termination.data() );
  config->writeEntry( "RET", RET.data() );
  config->writeEntry( "Copies",Copies.data() );
  config->writeEntry( "Powersave", Powersave.data() );
  config->writeEntry( "Resolution",Resolution.data() );
  config->writeEntry( "Language",Language.data() );
  config->writeEntry( "Pageprotect",Pageprotect.data() );
  config->writeEntry( "Ptsize",Ptsize.data() );
  config->writeEntry( "Pitch",Pitch.data() );
  config->writeEntry( "Economode",Economode.data() );
  config->writeEntry( "Manualfeed",Manualfeed.data() );
  config->writeEntry( "Orientation",Orientation.data() );
  config->writeEntry( "Symset",Symset.data() );
  config->writeEntry( "Autocont",Autocont.data() );
  config->writeEntry( "Formlines",Formlines.data() );
  
  config->sync();

}
