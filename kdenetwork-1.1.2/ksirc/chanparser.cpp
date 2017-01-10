#include "chanparser.h"
#include "estring.h"
#include "alistbox.h"
#include "control_message.h"
#include <qmsgbox.h>
#include <iostream.h>
#include <stdio.h>
#include <ssfeprompt.h>
#include <string.h>

// Static parser table is "initialized"
QDict<parseFunc> ChannelParser::parserTable;


ChannelParser::ChannelParser(KSircTopLevel *_top) /*fold00*/
{
  top = _top;

  /*
   * Initial helper variables
   */
  prompt_active = FALSE;
  current_item = -1;
  top_item = 0;

  if(parserTable.isEmpty() == TRUE){
    parserTable.setAutoDelete(TRUE);
    parserTable.insert("`l`", new("parseFunc") parseFunc(&ChannelParser::parseSSFEClear));
    parserTable.insert("`s`", new("parseFunc") parseFunc(&ChannelParser::parseSSFEStatus));
    parserTable.insert("`i`", new("parseFunc") parseFunc(&ChannelParser::parseSSFEInit));
    parserTable.insert("`t`", new("parseFunc") parseFunc(&ChannelParser::parseSSFEMsg));
    parserTable.insert("`o`", new("parseFunc") parseFunc(&ChannelParser::parseSSFEOut));
    parserTable.insert("`p`", new("parseFunc") parseFunc(&ChannelParser::parseSSFEPrompt));
    parserTable.insert("`P`", new("parseFunc") parseFunc(&ChannelParser::parseSSFEPrompt));
    parserTable.insert("`R`", new("parseFunc") parseFunc(&ChannelParser::parseSSFEReconnect));
    // The rest are *** info message
    parserTable.insert("***", new("parseFunc") parseFunc(&ChannelParser::parseINFOInfo));
    parserTable.insert("*E*", new("parseFunc") parseFunc(&ChannelParser::parseINFOError));
    parserTable.insert("*!*", new("parseFunc") parseFunc(&ChannelParser::parseINFONicks)); // Normal
    parserTable.insert("*C*", new("parseFunc") parseFunc(&ChannelParser::parseINFONicks)); // 1st line
    parserTable.insert("*c*", new("parseFunc") parseFunc(&ChannelParser::parseINFONicks)); // Last line
    parserTable.insert("*#*", new("parseFunc") parseFunc(&ChannelParser::parseINFONicks)); // Non enhanced
    parserTable.insert("*>*", new("parseFunc") parseFunc(&ChannelParser::parseINFOJoin));
    parserTable.insert("*<*", new("parseFunc") parseFunc(&ChannelParser::parseINFOPart));
    parserTable.insert("*N*", new("parseFunc") parseFunc(&ChannelParser::parseINFOChangeNick));
    parserTable.insert("*+*", new("parseFunc") parseFunc(&ChannelParser::parseINFOMode));
    parserTable.insert("*T*", new("parseFunc") parseFunc(&ChannelParser::parseINFOTopic));
    // End of info message
    parserTable.insert("*  ", new("parseFunc") parseFunc(&ChannelParser::parseCTCPAction));
  }

}

void ChannelParser::parse(QString string) /*fold00*/
{
  string.detach(); // for older Qts

  parseFunc *pf;
  if(string.length() < 3){
    throw(parseError(string, QString("Dumb string, too short")));
  }

  /**
   * Start pre-parsing the strings to make them fit the 3 character
   * parser codes, etc
   */
  
  /*
   * SSFE control messages are too long, so we convert the
   * messges into a 3 character code, `#ssfe#\S becomes `\S`
   */
  if(string[0] == '`'){
    if(strncmp("`#ssfe#", string, 7) == 0){
      char s[] = { string[7], 0 };
      uint space;
      for(space = 6; (string[space] != ' '); space ++){
        if(space >= string.length()){
          space++;
          break;
        }
      }
      string.remove(0, space);
      string.prepend("` ");
      string.prepend(s);
      string.prepend("`");
    }
  }
  else if(string[0] == '*' &&
	  string[1] == ' '){
    string.insert(1, ' ');
  }
  // Pre-parsing is now complete
  
  pf = parserTable[string.mid(0, 3)];
  if(pf != 0x0){
//    debug("new("hanlder") hanlder handling: %s", string.data());
    (this->*(pf->parser))(string);
  }

  // Little bit of past parsing to catch one we've missed
  if(string[0] == '*' &&
     string[2] == '*'){
    string.remove(0, 3);
    throw(parseSucc(string, kSircConfig->colour_info, top->pix_bball));
  }
  // If it's unkown we just fall out of the function
}

void ChannelParser::parseSSFEClear(QString string) /*fold00*/
{
  string.detach();
  top->mainw->clear();
  top->mainw->repaint(TRUE);
  string.truncate(0);
  throw(parseSucc(QString(""))); // Null string, don't display anything
}

void ChannelParser::parseSSFEStatus(QString string) /*fold00*/
{
  string.detach();
  string.remove(0, 4); // strip off the first 4 characters
  char *status;
  if(string.length() < 8)
    throw(parseError("", "Unable to parse status string"));
  status = &string[8];
  char nick[101], modes[101], chan[101], chanmode[101];
  int found = sscanf(status, "%100s (%100[^)]) on %100s (%100[^)])", nick, modes, chan, chanmode); // Channel modes and user modes
  if(found != 4){
    found = sscanf(status, "%100s on %100s (%100[^)])", nick, chan, chanmode); // No user mode set
    if(found != 3){
      found = sscanf(status, "%100s (%100[^)]) (away) on %100s (%100[^)])", nick, modes, chan, chanmode); // Away
      if(found != 4){
	found = sscanf(status, "%100s (away) on %100s (%100[^)])", nick, chan, chanmode); // Away and no user mode set
	if(found != 3)
          throw(parseError("", "Unable to parse status string"));

        modes[0] = 0x0; modes[1] = 0x0; // No mode set for user
      }
      if(strlen(chan) < 98){
        memmove(chan+2, chan, 98);
        chan[0] = 'A';
	chan[1] = '-';
      }
    }
    modes[0] = 0x0; modes[1] = 0x0; // No mode set for user
  }
  
  QString status_line = chan;
  status_line += QString(" (") + chanmode + ") " +  nick +  " (" + modes + ") ";

  /*
   * Go srearching for key and limit messages
   */
  int start_kl = -1, end_kl = -1;
  start_kl = string.find('<');
  if(start_kl != -1){
    end_kl = string.findRev('>');
    if(end_kl != -1){
      status_line += string.mid(start_kl, end_kl - start_kl + 1) + " ";
    }
  }
  
  if(kSircConfig->DisplayTopic){
    if(top->topic.length() > 0)
      status_line += "T: " + top->topic;
    else
      status_line += "T: " + QString("<No Topic Set>");
  }

  if(strcmp(top->caption, status_line) != 0){
    if(nick[0] == '@')                 // If we're an op,,
      // update the nicks popup menu
      top->opami = TRUE;                  // opami = true sets us to an op
    else
      top->opami = FALSE;                 // FALSE, were not an ops
    top->UserUpdateMenu();                // update the menu
    top->setCaption(status_line);
    top->setIconText(status_line);
    if(top->ticker) {
      top->ticker->setCaption(status_line);
      top->ticker->setIconText(status_line);
    }
    top->caption = status_line;           // Make copy so we're not
    top->caption.detach();
    // constantly changing the title bar
  }
  throw(parseSucc(QString(""))); // Null string, don't display anything
}

void ChannelParser::parseSSFEInit(QString string) /*fold00*/
{
  throw(parseSucc(QString(""))); // Null string, don't display anything
}

void ChannelParser::parseSSFEOut(QString string) /*fold00*/
{
  throw(parseSucc(QString(""))); // Null string, don't display anything
}

void ChannelParser::parseSSFEMsg(QString string) /*fold00*/
{
  string.detach();
  if(string.length() > 100)
    throw(parseError(QString(""), QString("String length for nick is greater than 100 characters, insane, too big")));

  char nick[string.length()];
  int found = sscanf(string.data(), "`t` %s", nick);

  if(found < 1)
    throw(parseError(string, QString("Could not find nick in string")));

  if(!top->nick_ring.contains(nick)){
    top->nick_ring.append(nick);
    cerr << "Appending: " << nick << endl;
    if(top->nick_ring.count() > 10)
      top->nick_ring.removeFirst();
  }
  throw(parseSucc(QString(""))); // Null string, don't display anything
}


void ChannelParser::parseSSFEPrompt(QString string) /*fold00*/
{
  string.detach();
  if(prompt_active == FALSE){
    QString prompt, caption;
    ssfePrompt *sp;

    // Flush the screen.
    // First remove the prompt message from the Buffer.
    // (it's garunteed to be the first one)
    top->LineBuffer->removeFirst();
    top->Buffer = FALSE;
    top->sirc_receive(QString(""));

    caption = top->mainw->text(top->mainw->count() - 1);
    if(caption.length() < 3){
      caption = top->mainw->text(top->mainw->count() - 2);
      if(caption.length() > 2)
	top->mainw->removeItem(top->mainw->count() - 2 );
    }
    else
      top->mainw->removeItem(top->mainw->count() - 1 );
    top->mainw->scrollToBottom();
    // "'[pP]' " gives 4 spaces
    if(string.length() < 5)
      prompt = "No Prompt Given?";
    else
      prompt = string.mid(4, string.length() - 4);
    prompt_active = TRUE;
    // If we use this, then it blows up
    // if we haven't popped up on the remote display yet.
    sp = new("ssfePrompt") ssfePrompt(prompt, 0);
    sp->setCaption(caption);
    if(string[1] == 'P')
      sp->setPassword(TRUE);
    sp->exec();
    //	  cerr << "Entered: " << sp->text() << endl;
    prompt = sp->text();
    prompt += "\n";
    emit top->outputLine(prompt);
    delete sp;
    prompt_active = FALSE;
  }

  throw(parseSucc(QString(""))); // Null string, don't display anything
}

void ChannelParser::parseSSFEReconnect(QString string) /*fold00*/
{
  string.detach();
  if(top->channel_name[0] == '#'){
    QString str = "/join " + QString(top->channel_name) + "\n";
    emit top->outputLine(str);
  }

  throw(parseSucc(QString(""))); // Null string, don't display anything
}

void ChannelParser::parseINFOInfo(QString string) /*fold00*/
{
  string.detach();
  string.remove(0, 3);                // takes off the junk

  throw(parseSucc(string, kSircConfig->colour_info, top->pix_info)); // Null string, don't display anything
}

void ChannelParser::parseINFOError(QString string) /*fold00*/
{
  string.detach();
  string.remove(0, 3);               // strip the junk

  throw(parseSucc(string,kSircConfig->colour_error, top->pix_madsmile)); // Null string, don't display anything
}

void ChannelParser::parseINFONicks(QString in_string) /*fold00*/
{

  EString string = in_string;
  char *nick, *place_holder;

  string.detach();

  int start, count;
  char channel_name[101];

  bool clear_box = FALSE;

  // Check to see if it's a continued line
  if(string[1] == 'C'){
    string[1] = '!';
    clear_box = TRUE;
  }
  if(string[1] == '#'){
    string[1] = '!';
    clear_box = FALSE;
  }
  else if(string[1] == 'c'){
    top->nicks->setAutoUpdate(TRUE);         // clear and repaint the listbox
    if(current_item > 0)
      top->nicks->setCurrentItem(current_item);
    top->nicks->setTopItem(top_item);
    top->nicks->repaint(TRUE);
    throw(parseSucc(QString("")));           // Parsing ok, don't print anything though
  }
  
  
  // Get the channel name portion of the string
  // Search for the first space, since : can be embeded into channel names.
  count = sscanf(string, "*!* Users on %100[^ ]", channel_name);
  if(count < 1){
    throw(parseError(string, QString("Could not find channel name")));
  }

  // Remove the : from the end.
  channel_name[strlen(channel_name)-1] = 0x0;
  
  if (strcasecmp(channel_name,top->channel_name) != 0){
    string.remove(0,3);
    throw(parseSucc(string,kSircConfig->colour_info,top->pix_info));
  }
  
  top->nicks->setAutoUpdate(FALSE);        // clear and update nicks
  if(clear_box == TRUE){
    current_item = top->nicks->currentItem();
    top_item = top->nicks->topItem();
    top->nicks->clear();
  }
  
  try {
      start = string.find(": ", 0, FALSE); // Find start of nicks
  }
  catch(estringOutOfBounds &err){
      throw(parseError(string, QString("Could not find start of nicks")));
  }
  
  place_holder = &string[start]+2;
  nick = strtok(place_holder, " ");
  
  while(nick != 0x0){                     // While there's nick to go...
    nickListItem *irc = new("nickListItem") nickListItem();

    bool done = FALSE;

    for(;;){
      switch(*nick){
      case '@':
	irc->setOp(TRUE);
	break;
      case '+':
	irc->setVoice(TRUE);
	break;
      case '#':
	irc->setAway(TRUE);
	break;
      case '*':
	irc->setIrcOp(TRUE);
	break;
      default:
	done = TRUE;
      }
      if(done == TRUE)
	break;
      nick++; // Move ahead to next character
    }

    irc->setText(nick);
    top->nicks->inSort(irc);
    nick = strtok(NULL, " ");
  }
  
  throw(parseSucc(QString("")));           // Parsing ok, don't print anything though
}

void ChannelParser::parseINFOJoin(QString string) /*fold00*/
{
  char nick[101], channel[101];
  string.detach();
  string.remove(0, 4);                   // strip *>* to save a few compares
  if(sscanf(string, "You have joined channel %100s", channel) > 0){
    QString chan = QString(channel).lower();
    if(strcasecmp(top->channel_name, chan) == 0)
      top->show();
    emit top->open_toplevel(chan);
    throw(parseSucc(" " + string, kSircConfig->colour_chan, top->pix_greenp));
  }
  else if(sscanf(string, "%100s %*s has joined channel %100s", nick, channel) > 0){
    if(strcasecmp(top->channel_name, channel) != 0){
      throw(parseWrongChannel(" " + string, kSircConfig->colour_error, top->pix_greenp));
    }
    //	nicks->insertItem(s3, 0);      // add the sucker
    top->nicks->inSort(nick);
    throw(parseSucc(" " + string, kSircConfig->colour_chan, top->pix_greenp));
  }


}

void ChannelParser::parseINFOPart(QString string) /*fold00*/
{
  char nick[101], channel[101];
 
  string.detach();
  string.remove(0, 4);                // clear junk

  // Multiple type of parts, a signoff or a /part
  // Each get's get nick in a diffrent localtion
  // Set we search and find the nick and the remove it from the nick list
  // 1. /quit, signoff, nick after "^Singoff: "
  // 2. /part, leave the channek, nick after "has left \w+$"
  // 3. /kick, kicked off the channel, nick after "kicked off \w+$"
  //


  try {
    if(sscanf(string, "Signoff: %100s", nick) >= 1){
      infoFoundNick ifn(nick);
      throw(ifn);
    }
    /*
     * Check for "You" before everyone else or else the next
     * case will match it
     */
    else if(sscanf(string, "You have left channel %100s", channel)){

      if(strcasecmp(top->channel_name, channel) == 0){
	QApplication::postEvent(top, new("QCloseEvent") QCloseEvent()); // WE'RE DEAD
	throw(parseSucc(QString("")));
      }
    }
    /*
     * Same as above, check your own state first
     */
    else if(sscanf(string, "You have been kicked off channel %100s", channel) >= 1){
      if(strcasecmp(top->channel_name, channel) != 0)
        throw(parseWrongChannel(string, kSircConfig->colour_error, top->pix_madsmile));
      if (kSircConfig->AutoRejoin == TRUE) {
        QString str = "/join " + QString(top->channel_name) + "\n";
        emit top->outputLine(str);
        if(top->ticker)
          top->ticker->show();
        else
          top->show();
      }
      else {
        if(top->KickWinOpen != false)
          throw(parseError(" " + string, QString("Kick window Open")));
        top->KickWinOpen = true;
        switch(QMessageBox::information(top, "You have Been Kicked",
                                        string.data(),
                                        "Rejoin", "Leave", 0, 0, 1)){
                                        case 0:
                                          {
                                            QString str = "/join " + QString(top->channel_name) + "\n";
                                            emit top->outputLine(str);
                                            if(top->ticker)
                                              top->ticker->show();
                                            else
  					      top->show();
					    throw(parseSucc(" " + string, kSircConfig->colour_chan, top->pix_greenp));
                                          }
                                        break;
                                        case 1:
                                          QApplication::postEvent(top, new("QCloseEvent") QCloseEvent()); // WE'RE DEAD
                                          break;
        }
        top->KickWinOpen = false;
      }
    }
    else if(sscanf(string, "%100s has left channel %100s", nick, channel) >= 2){
      if(strcasecmp(top->channel_name, channel) == 0){
	infoFoundNick ifn(nick);
	throw(ifn);
      }
      else{
	throw(parseWrongChannel(QString("")));
      }
    }
    else if(sscanf(string, "%100s has been kicked off channel %100s", nick, channel) >= 2){
      if(strcasecmp(top->channel_name, channel) == 0){
	infoFoundNick ifn(nick);
	throw(ifn);
      }
      else
	throw(parseWrongChannel(QString("")));
    }
    else{                                // uhoh, something we missed?
      throw(parseError(" " + string, QString("Failed to parse part/kick/leave/quit message")));
    }
  }
  catch(infoFoundNick &ifn){
    int index = top->nicks->findNick(ifn.nick);
    if(index >= 0){
      top->nicks->removeItem(index);
      throw(parseSucc(" " + string, kSircConfig->colour_chan, top->pix_greenp));
    }
    else{
      throw(parseSucc(QString("")));
    }
  }
}

void ChannelParser::parseINFOChangeNick(QString string) /*fold00*/
{
  char old_nick[101], new_nick[101];
  string.detach();
  string.remove(0, 4); // Remove the leading *N* and space
  int found = sscanf(string, "%100s is now known as %100s", old_nick, new_nick);
  if(found < 0){
    throw(parseError(" Unable to parse: " + string, QString("Unable to parse change nick code")));
  }
  
    // If we have a window open talking to the nick
  // Change the nick to the new one.
  if((top->channel_name[0] != '#') &&
     (strcasecmp(top->channel_name, old_nick) == 0)){
    QString snew_nick = new_nick;
    snew_nick.detach();
    top->control_message(CHANGE_CHANNEL, snew_nick.lower());
  }
  
  // search the list for the nick and remove it
  // since the list is source we should do a binary search...
  found = top->nicks->findNick(old_nick);
  if(found >= 0){ // If the nick's in the nick list, change it and display the change /*FOLD01*/
    int selection = top->nicks->currentItem();
    bool isOp = top->nicks->isTop(found); // Are they an op?
    top->nicks->setAutoUpdate(FALSE);
    top->nicks->removeItem(found);        // remove old nick
    if(isOp == TRUE){
      nickListItem *irc  = new("nickListItem") nickListItem();
      irc->setText(new_nick);
      irc->setOp(TRUE);
      top->nicks->inSort(irc);
    }
    else{
      top->nicks->inSort(new_nick);     // add new("nick") nick in sorted poss
      // can't use changeItem since it
      // doesn't maintain sort order
    }
    top->nicks->setCurrentItem(selection);
    top->nicks->setAutoUpdate(TRUE);
    top->nicks->repaint(TRUE);
    // We're done, so let's finish up
    throw(parseSucc(" " + string, kSircConfig->colour_chan, top->pix_greenp));
  }
  else {
    throw(parseSucc(QString("")));
    //	  warning("Toplevel-N: nick change search failed on %s", s3.data());
  } /*fold01*/

}

void ChannelParser::parseINFOMode(QString string) /*fold00*/
{
  // Basic idea here is simple, go through the mode change and
  // assign each mode a + or a - and an argument or "" if there is
  // none.  After that each mode change it looked at to see if
  // we should handle it in any special way.

  // Strip off leading sirc info
  string.detach();
  string.remove(0, 4);


  /*
   * 1k is pretty safe since KProcess returns 1 k blocks, and lines don't get split between reads. This is emprical
   */
  char modes[1024], args[1024], channel[101]; // Modes holds list of modes
  char *next_arg; //, *next_token;
  int found = 0;
  
  if(string.find("for user") >= 0)
    throw(parseSucc(" " + string, kSircConfig->colour_info, top->pix_bluep));

  /*
   * We need to 2 scanf's, one for the case of arguments, and one for no args.
   */
  found = sscanf(string, "Mode change \"%1023s %1023[^\"]\" on channel %100s", modes, args, channel);
  if(found < 3){
    found = sscanf(string, "Mode change \"%1023[^\" ]\" on channel %100s", modes, channel);
    if(found < 2)
      throw(parseError(" Failed to parse mode change: " + string, QString("")));
    /*
     * But in a null so incase we try and read an arg we don't barf
     */
    args[0] = 0;
  }
  
  QStrList mode, arg; // Deep copes is true
  /*
   * fmode provides the complete current mode.
   * fmode[0] is the + or -, and is set when we get a +,-
   * fmode[1] is the acutal single letter mode change.  Updated at the top of each parsing.
   *          this means unless modes[pos] is a valid mode don't rely on fmode.  (it might be ++ or -- for example)
   */
  char fmode[] = "+X";// The full mode with +X where X is the mode
  next_arg = strtok(args, " ");
  for(int pos = 0; modes[pos] != 0; pos++){
    fmode[1] = modes[pos];
    switch(modes[pos]){
    case '+':
    case '-':
      fmode[0] = modes[pos];
      break;
    case 'l': // Chan limits
      /*
       * -l doesn't take any arguments, so just add the mode and break
       * +l otoh does, so read the argument
       */
      if(fmode[0] == '-'){
        mode.append(fmode);
        break;
      }
    case 'o': // Op, arg is the nick
    case 'v': // Voice, arg is the nick
    case 'b': // Ban, arg is mask banned
    case 'k': // kcik, arg is nick
      mode.append(fmode);
      if(next_arg == NULL)
        throw(parseError(" Unable to parse mode change: " + string, QString("")));
      arg.append(next_arg);
      next_arg = strtok(NULL, " ");
      break;
    case 'i': // Invite only
    case 'n': // No message to chan 
    case 'p': // Private
    case 'm': // Moderated
    case 's': // Secret
    case 't': // Topic setable by ops
    case 'R': // (Dalnet) only registered may join
    case 'r': // (Dalnet) only registered may join or something
      /*
       * Mode changes which don't take args
       */
      mode.append(fmode);
      arg.append("");
      break;
    default:
      warning("Unkown mode change: %s. Assuming no args", fmode);
      mode.append(fmode);
      arg.append("");
    }
  }
  // We have the modes set in mode and arg, now we go though
  // looking at each mode seeing if we should handle it.
  for(uint i = 0; i < mode.count(); i++){
    /*
     * Look at the second character, it's uniq, check for +,- latter
     */
    if(mode.at(i)[1] == 'o'){
      bool op;
      if(mode.at(i)[0] == '+')
        op = TRUE;
      else
        op = FALSE;

      if(strlen(arg.at(i)) == 0){
        warning("Invalid nick in +-v mode change");
        continue;
      }
      
      int offset = top->nicks->findNick(arg.at(i));
      if(offset >= 0){
        top->nicks->setAutoUpdate(FALSE);
        nickListItem *irc = new("nickListItem") nickListItem();
        *irc = *top->nicks->item(offset);
        top->nicks->removeItem(offset);           // remove old nick
        irc->setOp(op);
        // add new("nick") nick in sorted pass,with colour
        top->nicks->inSort(irc);
        top->nicks->setAutoUpdate(TRUE);
        top->nicks->repaint(TRUE);
      }
      else{
        warning("TopLevel+o: nick search failed on %s", mode.at(i));
      }
    }
    else if(mode.at(i)[1] == 'v'){
      bool voice;
      if(mode.at(i)[0] == '+')
        voice = TRUE;
      else
        voice = FALSE;

      if(strlen(arg.at(i)) == 0){
        warning("Invalid nick in +-v mode change");
        continue;
      }
      
      int offset = top->nicks->findNick(arg.at(i));
      if(offset >= 0){
        top->nicks->setAutoUpdate(FALSE);
        nickListItem *irc = new("nickListItem") nickListItem();
        *irc = *top->nicks->item(offset);
        top->nicks->removeItem(offset);           // remove old nick
        irc->setVoice(voice) ;
        // add new("nick") nick in sorted pass,with colour
        top->nicks->inSort(irc);
        top->nicks->setAutoUpdate(TRUE);
        top->nicks->repaint();
      }
    }
    else{
      //	      cerr << "Did not handle: " << mode.at(i) << " arg: " << arg.at(i)<<endl;
    }
  }
  /*
   * We're all done, so output the message and be done with it
   */
  throw(parseSucc(" " + string, kSircConfig->colour_info, top->pix_info));

}

void ChannelParser::parseCTCPAction(QString string) /*fold00*/
{
  string.detach();
  string.remove(0, 2);      // * <something> use fancy * pixmap. Remove 2, leave one for space after te *
                            // why? looks cool for dorks
  throw(parseSucc(string, kSircConfig->colour_text, top->pix_star));
}

void ChannelParser::parseINFOTopic(QString string) /*fold00*/
{
  char channel[101];
  string.detach();
  string.remove(0, 4); // Remove the leading *T* and space
  // *T* Topic for #kde: Don't use koffice! You will get into deep emotional problems!
  int found = sscanf(string, "Topic for %100[^:]: ", channel);
  if(found == 1){
    // If this is the correct channel, set the status line for the topic
    if(strcasecmp(top->channel_name, channel) == 0){
      int start = string.find(": ")+1;
      int end = string.length() - start;
      top->topic = string.mid(start, end);
    }
  }
  else {
    found = sscanf(string, "%*s has changed the topic on channel %100s to", channel);
    if(found == 1){
      if(strcasecmp(top->channel_name, channel) == 0){
        int start = string.find(" \"")+2;
        int end = string.length() - start - 1; // 1 to remove trailing "
        top->topic = string.mid(start, end);
        QString cmd = "/eval &dostatus();\n";
        top->sirc_write(cmd);
      }
    }
  }
  throw(parseSucc(" " + string, kSircConfig->colour_info, top->pix_bball));
}
