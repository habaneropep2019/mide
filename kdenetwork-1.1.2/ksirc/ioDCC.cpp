/**********************************************************************
 
 The IO DCC Window

 $$Id: ioDCC.cpp,v 1.13.2.1 1999/02/09 14:47:00 kulow Exp $$

 Display DCC progress, etc.  This in the future should be expanded.

**********************************************************************/
 

#include "ioDCC.h"
#include <qregexp.h> 
#include <iostream.h>

KSircIODCC::~KSircIODCC()
{
}

void KSircIODCC::sirc_receive(QString str)
{
  // Parse the string to find out what type it is.
  // Note the order here.  
  // Most people tend to receive files, so let's
  // parse the byte xfered messages first since there's lot's of them.
  // The we get lots of send bytexfer messages so parse them second.
  // Then we look at the one time start/stop messages.  They only arrive 
  // once in a long long time (compared to the byte messages) so if it takes
  // a few extra clock cycles to find them, who cares?
  if(str.find("DCC GET read:", 0) != -1){
    //
    // Find the filename first
    //
    int pos1 = 18; // String starts at 20, fixed length.
    int pos2 = str.find(" ", pos1);
    if(pos2 < 0)
      return;
    QString filename = str.mid(pos1, pos2 - pos1);
    //
    // Does the DCCInfo exist? If not, exit now.
    //
    DCCInfo *stat = DCCStatus[filename];
    if(stat == 0){
      proc->getWindowList()["!default"]->sirc_receive(
					"*E* DCC Premature Close");
      //      proc->getWindowList()["!default"]->sirc_receive(str);
      return;
    }
    //
    // Find the number of bytes transfered.
    //
    pos1 = pos2 + 8; // Next string is 8 characters after the next the filename
    pos2 = str.length();
    int bytesXfer = str.mid(pos1, pos2-pos1).toInt();
    //
    // Only update the display for 1% intervals.
    // LastSize + 1%Size must be less than the total xfered bytes.
    //
    if(bytesXfer > (stat->LastSize + stat->PercentSize)){
      ASSERT(stat->PercentSize > 0); // We devide by this!
      ASSERT(bytesXfer > 0); // Setting progress back to 0 screws it up
      DlgList[filename]->setValue(bytesXfer/(stat->PercentSize));
      stat->LastSize = bytesXfer;
    }
  }
  else if(str.find("DCC SEND write:", 0) != -1){
    //
    // Find the filename, it's first byte is fixed at 22.
    //
    int pos1 = 20; // Starting bye is fixed.
    int pos2 = str.find(" ", pos1);
    if(pos2 < 0)
      return;
    QString filename = str.mid(pos1, pos2 - pos1);
    // 
    // Let's get and make sure DCCInfo holds the right status info.
    //
    DCCInfo *stat = DCCStatus[filename];
    if(stat == 0){
      proc->getWindowList()["!default"]->sirc_receive(
					 "*E* DCC Premature Close - flushing buffers"
					 );
      //      proc->getWindowList()["!default"]->sirc_receive(str);
      return;
    }
    //
    // Fine the bytesXfered.  Start is 8 bytes after end of filename. 
    // No error checking since pos2 is > 0 therefor pos1 is > 0.
    // and string is not 0 length, therefor pos2 is > 0.
    //
    pos1 = pos2 + 8;
    pos2 = str.length();
    int bytesXfer = str.mid(pos1, pos2-pos1).toInt();
    //
    // We're in progress now, check to see if that's out status and set it so
    //
    if(stat->inProgress == FALSE){
      stat->inProgress = TRUE;
      DlgList[filename]->setTopText("DCC Sending: " + filename);      
    }
    //
    // Don't update except for 1 percent intervals.
    //
    if(bytesXfer > (stat->LastSize + stat->PercentSize)){
      ASSERT(stat->PercentSize > 0); // We devide by this!
      ASSERT(bytesXfer >= 0); // Setting progress back to 0 screws it up
      DlgList[filename]->setValue(bytesXfer/(stat->PercentSize));
      stat->LastSize = bytesXfer;
    }
  }
  else if(str.find("DCC SEND (", 0) != -1){
    int pos1 = str.find("SEND") + 6;
    int pos2 = str.find(")", pos1);
    if((pos1 < 0) || (pos2 < 0)){
      cerr << "FILENAME Pos1/Pos2 " << pos1 << '/' << pos2 << endl;
      return;
    }
    QString filename = str.mid(pos1, pos2 - pos1);
    pos1 = str.find("(size: ") + 7;
    pos2 = str.find(")", pos1);
    if((pos1 < 0) || (pos2 < 0)){
      cerr << "SIZE Pos1/Pos2 " << pos1 << '/' << pos2 << endl;
      return;
    }
    QString size = str.mid(pos1, pos2-pos1);
    int fileSize = size.toInt(); // Bytes per step
    if(fileSize >= 100){
      fileSize /= 100;
    }
    else{
      fileSize = 1;
    }
    pos1 = str.find(" from ") + 6;
    pos2 = str.find(" ", pos1);
    if((pos1 < 0) || (pos2 < 0)){
      cerr << "FROM Pos1/Pos2 " << pos1 << '/' << pos2 << endl;
      return;
    }
    QString nick = str.mid(pos1, pos2 - pos1);
    // Setup info structure before xfer list in case they press cancel.
    DCCInfo *stat = new("DCCInfo") DCCInfo;
    stat->LastSize = 0;
    stat->PercentSize = fileSize;
    stat->nick = nick;
    stat->cancelMessage = "/dcc close get " + nick + " " + filename + "\n";
    stat->inProgress = FALSE;
    DCCStatus.insert(filename, stat);
    // Setup and make xfer dialog.
    DlgList.insert(filename, new("KSProgress") KSProgress());
    DlgList[filename]->setID(filename);
    DlgList[filename]->setRange(0,100);
    DlgList[filename]->setTopText("DCC Receving: " + filename);
    DlgList[filename]->setBotText("Size: " + size);
    connect(DlgList[filename], SIGNAL(cancel(QString)),
	    this, SLOT(cancelTransfer(QString)));
    // Add file to pending list, and show it if needed
    pending->fileListing()->insertItem(nick + " offered " + filename);
    pending->fileListing()->setCurrentItem(pending->fileListing()->count()-1);
    if(pending->isVisible() == FALSE)
      pending->show();
  }
  else if(str.find("Sent DCC SEND", 0) != -1){
    // Message for form: *D* Sent DCC SEND request to nick (file,size)
    //    cerr << "Got: " << str << endl;
    // Find the users nick
    int pos1 = str.find("request to ") + 11;
    int pos2 = str.find(" ", pos1);
    if((pos1 < 0) || (pos2 < 0)){
      cerr << "NICK Pos1/Pos2 " << pos1 << '/' << pos2 << endl;
      return;
    }
    QString nick = str.mid(pos1, pos2 - pos1);
    // Find the filename
    pos1 = str.find("(",pos2) + 1; // Start from last offset
    pos2 = str.find(",", pos1);    // until the comma
    if((pos1 < 0) || (pos2 < 0)){
      cerr << "FILENAME Pos1/Pos2 " << pos1 << '/' << pos2 << endl;
      return;
    }
    QString filename = str.mid(pos1, pos2-pos1);
    // Find the size
    pos1 = pos2 + 1; // size start is right after comma
    pos2 = str.find(")", pos1); // look for last )
    if((pos1 < 0) || (pos2 < 0)){
      cerr << "SIZE Pos1/Pos2 " << pos1 << '/' << pos2 << endl;
      return;
    }
    QString size = str.mid(pos1, pos2-pos1);
    int fileSize = size.toInt(); // Bytes per step
    if(fileSize >= 100){
      fileSize /= 100;
    }
    else{
      fileSize = 1;
    }
    // Create status structure
    DCCInfo *stat = new("DCCInfo") DCCInfo;
    stat->LastSize = 0;
    stat->PercentSize = fileSize;
    stat->nick = nick;
    stat->cancelMessage = "/dcc close send " + nick + " " + filename + "\n";
    stat->inProgress = FALSE;
    DCCStatus.insert(filename, stat);
    // Create transfer status window
    DlgList.insert(filename, new("KSProgress") KSProgress());
    DlgList[filename]->setID(filename);
    DlgList[filename]->setRange(0,100);
    DlgList[filename]->setTopText("DCC Pending Send: " + filename);
    DlgList[filename]->setBotText("Size: " + size);
    DlgList[filename]->show();
    connect(DlgList[filename], SIGNAL(cancel(QString)),
	    this, SLOT(cancelTransfer(QString)));
  }
  else if(str.find("DCC transfer with") != -1){
    int pos1 = str.find(" (", 0)+2;
    int pos2 = str.find(")", pos1);
    QString filename = str.mid(pos1, pos2-pos1);
    delete DlgList[filename];
    DlgList.remove(filename);
    delete DCCStatus[filename];
    DCCStatus.remove(filename);
  }
  else{
    proc->getWindowList()["!default"]->sirc_receive(str);
  }
  
}

void KSircIODCC::control_message(int, QString)
{
}

void KSircIODCC::cancelTransfer(QString filename)
{
  if(DlgList[filename]){
    emit outputLine(DCCStatus[filename]->cancelMessage);
    delete DlgList[filename];
    DlgList.remove(filename);
    delete DCCStatus[filename];
    DCCStatus.remove(filename);
  }
}

void KSircIODCC::getFile()
{
  QString text = pending->fileListing()->text(pending->fileListing()->currentItem());
  int pos = text.find(" ", 0);
  QString nick = text.mid(0, pos);
  pos = text.find(" ", pos+1) + 1;
  QString filename = text.mid(pos, text.length() - pos);
  if(DlgList[filename]->isVisible() == FALSE)
    DlgList[filename]->show();
  QString command = "/dcc get " + nick + " " + filename + "\n";
  emit outputLine(command);
  for(uint i = 0; i < pending->fileListing()->count(); i++)
    if(QString(pending->fileListing()->text(i)) == (nick + " offered " + filename))
      pending->fileListing()->removeItem(i);

  if(pending->fileListing()->count() == 0)
     pending->hide();
}

void KSircIODCC::forgetFile()
{
  QString text = pending->fileListing()->text(pending->fileListing()->currentItem());
  int pos = text.find(" ", 0);
  QString nick = text.mid(0, pos);
  pos = text.find(" ", pos+1) + 1;
  QString filename = text.mid(pos, text.length() - pos);
  QString command = "/dcc close get " + nick + " " + filename + "\n";
  emit outputLine(command);
  for(uint i = 0; i < pending->fileListing()->count(); i++)
    if(QString(pending->fileListing()->text(i)) == (nick + " offered " + filename))
      pending->fileListing()->removeItem(i);

  if(pending->fileListing()->count() == 0)
     pending->hide();

}

filterRuleList *KSircIODCC::defaultRules()
{

//  filterRule *fr;
  filterRuleList *frl = new("filterRuleList") filterRuleList();
  frl->setAutoDelete(TRUE);
  /*
  fr = new("filterRule") filterRule();
  fr->desc = "Capture DCC IO Messages";
  fr->search = "^\\*D\\*";
  fr->from = "^";
  fr->to = "~!dcc~";
  frl->append(fr);
  */
  return frl;

}
#include "ioDCC.moc"
