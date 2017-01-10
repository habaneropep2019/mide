/***********************************************************************

 IO Controller Object

 $$Id: iocontroller.cpp,v 1.28.2.2 1999/02/11 15:11:55 kulow Exp $$

 Main io controller.  Reads and writes strings to sirc.  Input
 received in the following 2 formats:

 1. ~window name~<message>
 2. <message>

 each is handled diffrently. The window name is extracted from #1 and
 if the window exists the message portion is sent to it.  If the
 window doesn't exist, or case 2 is found the window is sent to the
 control window "!default".  !default is NOT a constant window but
 rather follows focus arround.  This is the easiest way to solve the
 problem of output to commands that don't have a fixed destination
 window.  (/whois, /help, etc )

 Implementation:

   Friends with KSircProcess allows easy access to TopList.  Makes sence
   and means that IOController has access to TopList, etc.  The two work
   closely together.

   Variables:
      holder: used to hold partital lines between writes.
      *proc: the acutally sirc client process.
      *ksircproc: the companion ksircprocess GUI controller
      stdout_notif: access to SocketNotifier, why is this global to the
                    class?
      counter: existance counter.

   Functions:
   public:
     KSircIOController(KProcess*, KSircProcess*):
       - Object constructor takes two arguements the KProcess
         that holds a running copy of sirc.
       - KSircProcess is saved for access latter to TopList.
       - The receivedStdout signal from KProcess is connected to
         stdout_read and the processExited is connected to the sircDied
	 slot.

     ~KSircIOController: does nothing at this time.

     public slots:
       stdout_read(KProcess  *, _buffer, buflen): 
         - Called by kprocess when data arrives.
	 - This function does all the parsing and sending of messages
           to each window.

       stderr_read(KProcess*, _buffer, buflen):
         - Should be called for stderr data, not connected, does
	   nothing.

       stdin_write(QString):
         - Slot that get's connected to by KSircProcess to each
	   window.  QString shold be written un touched! Let the
	   writter figure out what ever he wants to do.

       sircDied: 
         - Should restart sirc or something....
	 - Becarefull not to get it die->start->die->... etc

***********************************************************************/

#include "iocontroller.h"
#include "toplevel.h"
#include <iostream.h>
#include "ksircprocess.h"
#include "control_message.h"
#include "config.h"

extern KApplication *kApp;
extern global_config *kSircConfig;

int KSircIOController::counter = 0;

KSircIOController::KSircIOController(KProcess *_proc, KSircProcess *_ksircproc)
  : QObject()
{

  counter++;

  proc = _proc;              // save proc
  ksircproc = _ksircproc;    // save ksircproce

  send_buf = 0x0;

  // Connect the data arrived
  // to sirc receive for adding
  // the main text window
  connect(proc, SIGNAL(receivedStdout(KProcess *, char *, int)),
          this, SLOT(stdout_read(KProcess*, char*, int))); 

  // Connect the stderr data
  // to sirc receive for adding
  // the main text window
  connect(proc, SIGNAL(receivedStderr(KProcess *, char *, int)),
          this, SLOT(stderr_read(KProcess*, char*, int)));
  
  connect(proc, SIGNAL(processExited(KProcess *)),
	  this, SLOT(sircDied(KProcess *)));
                                              // Notify on sirc dying
  connect(proc, SIGNAL(wroteStdin(KProcess*)),
	  this, SLOT(procCTS(KProcess*)));
  proc_CTS = TRUE;
}

void KSircIOController::stdout_read(KProcess *, char *_buffer, int buflen)
{

  /*

    Main reader reads from sirc and ships it out to the right
    (hopefully) window.

    Problem trying to solve:

       _buffer holds upto 1024 (it seems) block of data.  We need to
       take it, split into lines figure out where each line should go,
       and ship it there.  

       We watch for broken end of lines, ie partial lines and reattach
       them to the front on the next read.

       We also stop all processing in the windows while writting the
       lines.

    Implementation:

    Variables: 
       _buffer original buffer, holds just, icky thing, NOT NULL terminated!
       buf: new("clean") clean just the right size buf that is null terminated.
       pos, pos2, pos3 used to cut the string up into peices, etc.
       name: destination window.
       line: line to ship out.

    Steps:
       1. read and copy buffer, make sure it's valid.
       2. If we're holding a broken line, append it.
       3. Check for a broken line, and save the end if it is.
       4. Stop updates in all windows.
       5. Step through the data and send the lines out to the right
       window.
       6. Cleanup and continue.

   */

  int pos,pos2,pos3;
  QString name, line;
  
  QString buffer(_buffer, buflen+1);

  name = "!default";

  if(holder.length() > 0){ 
    buffer.prepend(holder);
    holder.truncate(0);
  }

  if(buffer[buffer.length()-1] != '\n'){
    pos2 = buffer.length();
    pos = buffer.findRev('\n', pos2);
    if(pos != -1){
      holder = buffer.mid(pos+1, pos2-pos-1);
      buffer.truncate(pos);
    }
  }

  pos = pos2 = 0;
  ksircproc->TopList["!all"]->control_message(STOP_UPDATES, "");
  do{
    pos2 = buffer.find('\n', pos);

    if(pos2 == -1)
      pos2 = buffer.length();
    
    line = buffer.mid(pos, pos2 - pos);
    if((line.length() > 0) && (line[0] == '~')){
      pos3 = line.find('~', 1);
      if(pos3 > 0){
	name = line.mid(1,pos3-1);
	name = name.lower(); 
	line.remove(0, pos3+1);
      }
    }
    if(!(ksircproc->TopList)[name]){
      // Ignore ssfe control messages with `
      // we left channel, don't open a window for a control message
      if(kSircConfig->AutoCreateWin == TRUE && line[0] != '`' && line[1] != '#'){
//        debug("Creating window for: %s because of: %s", name.data(), line.data());
	ksircproc->new_toplevel(name);
      }
      else{
	name = "!default";
	if(line[0] == '`')
	  name = "!discard";
      }
    }
    
//    debug("Before: %s", line.data());
    ksircproc->TopList[name]->sirc_receive(line);
//    debug("After: %s", line.data());
    
    pos = pos2+1;
  } while((uint) pos < buffer.length());

  ksircproc->TopList["!all"]->control_message(RESUME_UPDATES, "");

}

KSircIOController::~KSircIOController()
{
}

void KSircIOController::stderr_read(KProcess *p, char *b, int l)
{
  stdout_read(p, b, l);
}

void KSircIOController::stdin_write(QString s)
{

  buffer += s;
  if(proc_CTS == TRUE){
    int len = buffer.length();
    if(send_buf != 0x0){
      warning("KProcess barfed in all clear signal again");
      delete[] send_buf;
    }
    send_buf = new("char[send_buf]") char[len];
    strncpy(send_buf, buffer, len);
    if(proc->writeStdin(send_buf, len) == FALSE){
      //      cerr << "Failed to write but CTS HIGH! Setting low!: " << s << endl;
      proc_CTS = FALSE;
      killTimers();
      startTimer(500);
    }
    else{
      buffer.truncate(0);
      proc_CTS = FALSE;
    }
  }

  if(buffer.length() > 5000){
    cerr << "IOController: KProcess barfing again!\n";
  }
  //  write(sirc_stdin, s, s.length());

}

void KSircIOController::sircDied(KProcess *)
{

  ksircproc->TopList["!all"]->sirc_receive("*E* DSIRC IS DEAD");
  ksircproc->TopList["!all"]->sirc_receive("*E* KSIRC WINDOW HALTED");
  ksircproc->TopList["!all"]->sirc_receive("*E* Tried to run: " +  kapp->kde_bindir() + "/dsirc\n");
  ksircproc->TopList["!all"]->sirc_receive("*E* DID YOU READ THE INSTALL INTRUCTIONS?");
}

void KSircIOController::timerEvent ( QTimerEvent * )
{
  if(buffer.isEmpty() == TRUE){
    killTimers();
  }
  else{
    proc_CTS = TRUE;  // CTS was ok, but write failed, so force it high again
    QString str = ""; // and try again.  This should simply work arround
    stdin_write(str); // ksirc getting stuck wating for a procCTS().
  }
}

void KSircIOController::procCTS ( KProcess *)
{
  proc_CTS = TRUE;
  delete[] send_buf;
  send_buf = 0x0;
  if(buffer.isEmpty() == FALSE){
    QString str = "";
    stdin_write(str);
  }
}
#include "iocontroller.moc"
