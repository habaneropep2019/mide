#include <stdio.h>

#include "baserules.h"
#include "config.h"
#include <kconfig.h>

extern global_config *kSircConfig;
extern KConfig *kConfig;

void KSMBaseRules::sirc_receive(QString)
{
}

void KSMBaseRules::control_message(int, QString)
{
}

filterRuleList *KSMBaseRules::defaultRules()
{
  filterRule *fr;
  filterRuleList *frl = new("filterRuleList") filterRuleList();
  frl->setAutoDelete(TRUE);
  if(kSircConfig->filterKColour == TRUE){
    // 3 stage rule to filter:
    // ~blah~<something> stuff with ~ here and ~ there
    // Here's the path follows
    // =>~blah~;;;<something> with ~ here and ~ there
    // =>~blah~;;;<something> with ~~ here and ~~ there
    // =>~blah~<something> with ~~ here and ~ there
    // Basic problem was getting it not to change ~blah~ into ~~blah~~
    fr = new("filterRule") filterRule();
    fr->desc = "Add marker to second ~";
    fr->search = "^~\\S+~";
    fr->from = "^~(\\S+)~";
    fr->to = "~;;;$1~;;;";
    frl->append(fr);
    fr = new("filterRule") filterRule();
    fr->desc = "Escape kSirc Control Codes";
    fr->search = "~";
    fr->from = "(?g)~(?!;;;)";
    fr->to = "$1~~";
    frl->append(fr);
    fr = new("filterRule") filterRule();
    fr->desc = "Remove marker to second";
    fr->search = "^~;;;\\S+~;;;";
    fr->from = "^~;;;(\\S+)~;;;";
    fr->to = "~$1~";
    frl->append(fr);

  }
  else{
    // If we don't escape all ~'s at least esacpe the ones in
    // email/part joins etc.
    fr = new("filterRule") filterRule();
    fr->desc = "Search for dump ~'s";
    fr->search = "\\W~\\S+@\\S+\\W";
    fr->from = "~(\\S+@)";
    fr->to = "~~$1";
    frl->append(fr);
  }
  if(kSircConfig->filterMColour == TRUE){
    fr = new("filterRule") filterRule();
    fr->desc = "Remove mirc Colours";
    fr->search = "\\x03";
    fr->from = "(?g)\\x03(?:\\d{0,2},{0,1}\\d{0,2})";
    fr->to = "";
    frl->append(fr);  
  }
  if( kSircConfig->nickFHighlight >= 0){
    fr = new("filterRule") filterRule();
    fr->desc = "Highlight nicks in colours";
    fr->search = "^(?:~\\S+~)<\\S+>";
    fr->from = "<(\\S+)>";
    if(kSircConfig->nickBHighlight >= 0)
      sprintf(to, "<~%d,%d$1~c>", kSircConfig->nickFHighlight,
	                          kSircConfig->nickBHighlight);
    else
      sprintf(to, "<~%d$1~c>", kSircConfig->nickFHighlight);
    fr->to = to;
    frl->append(fr);  
  }
  if( kSircConfig->usHighlight >= 0){
    kConfig->setGroup("StartUp");
    if(strlen(kConfig->readEntry("Nick", "")) > 0){
      const char *nick = kConfig->readEntry("Nick", "");
      if(strlen(nick) > 99){
        debug("Nick too long");
      }
      sprintf(match_us,
              "(?i)<\\S+>.*%s.*", nick);
      sprintf(to_us,
	       "$1~%d", kSircConfig->usHighlight);
      fr = new("filterRule") filterRule();
      fr->desc = "Highlight our nick";
      fr->search = match_us;
      fr->from = "(<\\S+>)";
      fr->to = to_us;
      frl->append(fr);  
    }
  }

  // Default rules alays in place
  fr = new("filterRule") filterRule();
  fr->desc = "Remove Just bold in parts and joins";
  fr->search = "\\*\\x02\\S+\\x02\\*";
  fr->from = "\\*\\x02(\\S+)\\x02\\*";
  fr->to = "\\*$1\\*";
  frl->append(fr);

  return frl;
}
