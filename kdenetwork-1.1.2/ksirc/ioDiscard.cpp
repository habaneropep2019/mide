/**********************************************************************
 
 The IO Discarder

 $$Id: ioDiscard.cpp,v 1.4 1998/10/22 03:49:50 asj Exp $$

 Simple rule, junk EVERYTHING!!!

**********************************************************************/
 

#include "ioDiscard.h"

KSircIODiscard::~KSircIODiscard()
{
}

void KSircIODiscard::sirc_receive(QString)
{
}

void KSircIODiscard::control_message(int, QString)
{
}
