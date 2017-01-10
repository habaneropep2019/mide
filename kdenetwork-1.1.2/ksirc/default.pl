$n_isaway='';
$n_svefile=$ENV{"HOME"}."/.n0thing-pl.sve";
&docommand("^alias fing ctcp \$0 finger");
&docommand("^alias ver ctcp \$0 version");
&docommand("^alias tim ctcp \$0 time");
&docommand("^alias wallop wallops");
&docommand("^alias printuh set printuh");
&docommand("^alias printchan set printchan");
&docommand("^alias localhost set localhost");
&docommand("^alias logfile set logfile");
&docommand("^alias log set log");
&docommand("^alias ex system");
&docommand("^alias ls system ls -al");
&docommand("^alias cat system cat");
&docommand("^alias rm system rm");
&docommand("^alias ps system ps");
&docommand("^alias mail system mail");
&docommand("^alias setfing set finger");
sub cmd_open {
  &getarg;
  $newarg=$talkchannel unless $newarg;
  &tell("*\cbE\cb* Huh? what channel?"), return unless $newarg;
  $newarg =~ tr/A-Z/a-z/;
  &sl("MODE $newarg -lsmpik ".$chankey{$newarg});
}
&addcmd("open");
sub cmd_l {
  if ($talkchannel) {
    &sl("LIST $talkchannel");
    &sl("NAMES $talkchannel");
  }
}
&addcmd("l");
sub cmd_a {
  if ($n_lastmsg) {
    &msg($n_lastmsg, $args);
  } else {
    &tell("*\cbE\cb* How about sending a first /m to someone?");
  }
}
&addcmd("a");
sub cmd_pj {
  if ($talkchannel) {
    &sl("PART $talkchannel");
    &sl("JOIN $talkchannel");
  }
}
&addcmd("pj");
&docommand("^alias ds dcc send");
&docommand("^alias dl dcc list");
&docommand("^alias cdc dcc close chat");
&docommand("^alias cdg dcc close get");
&docommand("^alias cds dcc close send");
&docommand("^alias dcrename dcc rchat");
&docommand("^alias dcren dcc rchat");
&docommand("^alias rdc dcc rchat");
sub cmd_dc {
  &getarg;
  if ($newarg) {
    &docommand("dcc chat $newarg");
  } elsif ($n_lastdc) {
    &docommand("dcc chat $n_lastdc");
    $n_lastdc='';
  }
}
&addcmd("dc");
sub cmd_dg {
  &getarg;
  if ($newarg) {
    &docommand("dcc get $newarg $args");
  } elsif ($n_lastdcc) {
    &docommand("dcc get $n_lastdcc");
    $n_lastdcc='';
  }
}
&addcmd("dg");
sub hook_disc {
  if ($n_rejoinchannels) {
    $n_old_when=time;
    $n_old_umode=$umode;
    @n_old_channels=grep(!&eq($_, $talkchannel), @channels);
    push(@n_old_channels, $talkchannel) if $talkchannel;
    %n_old_chankey=%chankey;
  }
  &docommand("server 1"), $n_lastreconnect=time if 
      $n_reconnect && time-$n_lastreconnect>15;
}
&addhook("disconnect", "disc");
sub hook_reconnect {
  if ($n_rejoinchannels && ($n_old_umode || @n_old_channels) &&
      time <= $n_old_when+15) {
    $n_old_umode =~ tr/o//d;
    &sl("MODE $nick +$n_old_umode") if $n_old_umode ne '';
    local($chans, $keys)=('', '');
    foreach $c (keys (%n_old_chankey)) {
      $chans .= $c.",";
      $keys .= $n_old_chankey{$c}.",";
    }
    foreach $c (@n_old_channels) {
      $c =~ tr/A-Z/a-z/;
      $chans .= $c."," unless defined($n_old_chankey{$c});
    }
    $chans =~ s/,$//;
    $keys =~ s/,$//;
    &sl("JOIN $chans $keys") if $chans;
    $n_old_umode='';
    @n_old_channels=();
    %n_old_chankey=();
  }
}
&addhook("001", "reconnect");
sub hook_ctcp {
  if ($_[1] eq 'DCC') {
    $n_lastdcc=$who if $_[2] =~ /^SEND /;
    $n_lastdc=$who if $_[2] =~ /^CHAT /;
  }
}
&addhook("ctcp", "ctcp");
sub hook_kicked {
  local($kicked, $where_from, $reason)=@_;                           
  if ($n_autorejoin && &eq($kicked, $nick)) {
    &sl("JOIN $where_from $chankey{$where_from}");
  }
}
&addhook("kick", "kicked");
sub hook_uhnotify {
  unless ($n_slowserver) {
    &userhost($_[0], "&tell(\"*\cb)\cb* Signon by $_[0] (\$user\\\@\$host) detected!\");", "");
    $silent=1;
  }
} 
&addhook("notify_signon", "uhnotify");
sub hook_lastmsg {
  $n_lastmsg=$_[0] unless $_[0] =~ /^[\#\+]/;
}
&addhook("send_text", "lastmsg");
&addhook("send_action", "lastmsg");
sub hook_away {
  if ($n_awayonce && $_[0] =~ /^\S+\s+(\S+)\s/) {
    local($l)=$1;
    $l =~ tr/A-Z/a-z/;
    $silent=1 if $n_aways{$l} eq $_[0];
    $n_aways{$l}=$_[0];
  }
}
&addhook("301", "away");
sub hook_whois {
  if ($_[0] =~ /^\S+\s+(\S+)\s/) {
    local($l)=$1;
    $l =~ tr/A-Z/a-z/;
    delete $n_aways{$l};
  }
}
&addhook("311", "whois");
sub set_autorejoin {
 $set{'AUTOREJOIN'}="on", $n_autorejoin=1 if $_[0] =~ /^on$/i;
 $set{'AUTOREJOIN'}="off", $n_autorejoin=0 if $_[0] =~ /^off$/i;
}
&addset("autorejoin");
$set{"AUTOREJOIN"}="on";
$n_autorejoin=1;
&docommand("^alias rejoin set autorejoin");
sub set_rejoinchannels {
 $set{'REJOINCHANNELS'}="on", $n_rejoinchannels=1 if $_[0] =~ /^on$/i;
 $set{'REJOINCHANNELS'}="off", $n_rejoinchannels=0 if $_[0] =~ /^off$/i;
}
&addset("rejoinchannels");
$set{"REJOINCHANNELS"}="on";
$n_rejoinchannels=1;
&docommand("^alias rejoinchan set rejoinchannels");
sub set_reconnect {
 $set{'RECONNECT'}="on", $n_reconnect=1 if $_[0] =~ /^on$/i;
 $set{'RECONNECT'}="off", $n_reconnect=0 if $_[0] =~ /^off$/i;
}
&addset("reconnect");
$set{"RECONNECT"}="on";
$n_reconnect=1;
&docommand("^alias reconnect set reconnect");
sub set_awayonce {
 $set{'AWAYONCE'}="on", $n_awayonce=1 if $_[0] =~ /^on$/i;
 $set{'AWAYONCE'}="off", $n_awayonce=0 if $_[0] =~ /^off$/i;
 %n_awayonce=();
}
&addset("awayonce");
$set{"AWAYONCE"}="on";
$n_awayonce=1;
&docommand("^alias awayonce set awayonce");
sub set_slowserver {
 $set{'SLOWSERVER'}="on", $n_slowserver=1 if $_[0] =~ /^on$/i;
 $set{'SLOWSERVER'}="off", $n_slowserver=0 if $_[0] =~ /^off$/i;
}
&addset("slowserver");
$set{"SLOWSERVER"}="off";
$n_slowserver=0;
&docommand("^alias ss set slowserver");
sub set_wallv {
  $set{"WALLV"}="on", $n_wallv=1 if $_[0] =~ /^on$/i;
  $set{"WALLV"}="off", $n_wallv=0 if $_[0] =~ /^off$/i;
}
&addset("wallv");
$set{"WALLV"}="off";
$n_wallv=0;
&docommand("^alias wallv set wallv");
sub set_quitmsg {
  $n_quitmsg=$set{"QUITMSG"}=$_[0];
}
&addset("quitmsg");
$set{"QUITMSG"}=$n_quitmsg='';
&docommand("^alias setquit set quitmsg");
sub set_autoop {
  $set{"AUTOOP"}="off", $n_autoops=0 if $_[0] =~ /^(off|none)$/i;
  $set{"AUTOOP"}="on", $n_autoops=1 if $_[0] =~ /^(on|all)$/i;
  $set{"AUTOOP"}="oppass", $n_autoops=2 if $_[0] =~ /^op(pass)?$/i;
}
&addset("autoop");
$set{"AUTOOP"}="on";
$n_autoops=1;
&docommand("^alias autoop set autoop");
sub set_opdelay {
  $set{"OPDELAY"}=$n_opdelay=$_[0] if $_[0] =~ /^\d+$/;
}
&addset("opdelay");
$set{"OPDELAY"}="on";
$n_opdelay=5;
&docommand("^alias delay set opdelay");
sub set_oppass {
  $set{"OPPASS"}=$_[0];
}
&addset("oppass");
$set{"OPPASS"}="";
&docommand("^alias oppass set oppass");
sub cmd_ig {
  &docommand("ignore $args"), return if (!$args || $args =~ /^-/);
  &getarg;
  if ($newarg =~ /\@/) {
    $n_lastignore=$newarg;
    &docommand("ignore $newarg");
  } else {
    &userhost($newarg, "\$host=&n_hostpat(\$host); \$user =~ s/^\\~//; \$n_lastignore=\"*\$user\\\@\$host\"; &docommand(\"ignore *\$user\\\@\$host\");");
  }
}
&addcmd("ig");
sub cmd_sig {
  &getarg;
  &userhost($newarg, "\$host=&n_hostpat(\$host); \$n_lastignore=\"*\\\@\$host\"; &docommand(\"ignore *\\\@\$host\");");
}
&addcmd("sig");
sub cmd_unig {
  &getarg;
  if ($newarg eq '') {
    &docommand("ignore -$n_lastignore") if $n_lastignore ne '';
  } elsif ($newarg =~ /\@/) {
    &docommand("ignore -$newarg");
  } else {
    &userhost($newarg, "\$host=&n_hostpat(\$host); \$user =~ s/^\\~//; &docommand(\"ignore -*\$user\\\@\$host\");");
  }
}
&addcmd("unig");
sub n_hostpat {
  if ($_[0] =~ /^(\d+)\.(\d+)\.(\d+)\.(\d+)$/) {
    return "${1}.${2}.${3}.*";
  } elsif ($_[0] =~ /^([^. \t]+)\.([^. \t]+)\.(.*)$/) {
    return "*.${2}.${3}";
  } else {
    return $_[0];
  }
}
sub cmd_kb {
  &getarg;
  local($why)=($args);
  $why="game over, man.  game over." unless $why;
  &tell("*\cb0\cb* Must specify a nick to kickban"), return unless $newarg;
  &tell("*\cb0\cb* You're not on a channel"), return unless $talkchannel;
  &userhost($newarg, "\$host=&n_hostpat(\$host); \$user =~ s/^\\~//;
&sl(\"MODE $talkchannel -o+b $newarg *!*\$user\\\@\$host\");
&sl(\"KICK $talkchannel $newarg :$why\");");
&addcmd("kb");
sub cmd_ksb {
  &getarg;
  local($why)=($args);
  $why="d0rks suck big m00se ballZ" unless $why;
  &tell("*\cb0\cb* Must specify a nick to kicksiteban"), return unless $newarg;
  &tell("*\cb0\cb* You're not on a channel"), return unless $talkchannel;
  &userhost($newarg, "\$host=&n_hostpat(\$host);
&sl(\"MODE $talkchannel -o+b $newarg *!*\\\@\$host\");
&sl(\"KICK $talkchannel $newarg :$why\");");
}
&addcmd("ksb");
sub cmd_ban {
  &tell("*\cb0\cb* Must specify a nick to ban"), return unless $args;
  &tell("*\cb0\cb* You're not on a channel"), return unless $talkchannel;
  while (&getarg, $newarg ne '') {
    &userhost($newarg, "\$host=&n_hostpat(\$host); \$user =~ s/^\\~//;
      &sl(\"MODE $talkchannel +b *!*\$user\\\@\$host\");");
  }
}
&addcmd("ban");
sub cmd_sban {
  &getarg;
  &tell("*\cb0\cb* Must specify a nick to site ban"), return unless $newarg;
  &tell("*\cb0\cb* You're not on a channel"), return unless $talkchannel;
  &userhost($newarg, "\$host=&n_hostpat(\$host);
&sl(\"MODE $talkchannel +b *!*\\\@\$host\");");
}
&addcmd("sban");
sub n_unbans {
  local($n, $l)=('0', '');
  foreach (@n0thing_banlist) {
    if ($n<4) {
      $l.=" ".$_;
      $n++;
    } else {
      &sl("MODE $n0thing_channel -bbbb $l");
      $l=$_;
      $n=1;
    }
  }
  &sl("MODE $n0thing_channel -".("b" x $n)." $l");
}
sub cmd_sb {
  &dosplat;
  &getarg;
  $newarg=$talkchannel unless $newarg;
  return unless $newarg;
  $n_svbanchannel=$newarg;
  @n_svbanlist=();
  $n_nxbannb=1;
  &sl("MODE $newarg b");
}
&addcmd("sb");
sub hook_nbbans {
  local($c, $theban, $by)=((split(/ +/, $_[0]))[1, 2, 3]);
  if ($n_nxbannb && &eq($c, $n_svbanchannel)) {
    push(@n_svbanlist, $theban);
    &tell("*\cbb\cb* $c $n_nxbannb: $theban $by");
    $silent=1;
    $n_nxbannb++;
  }
}
&addhook("367", "nbbans");
sub hook_endofbans {
  $n_nxbannb=0;
}
&addhook("368", "endofbans");
sub cmd_cb {
  &tell("*\cb0\cb* You're not on a channel"), return unless $talkchannel;
  &addhook("367", "onecbban");
  &addhook("368", "donebans");
  @n0thing_banlist=();
  $n0thing_channel=$talkchannel;
  &sl("MODE $talkchannel b");
}
&addcmd("cb");
sub hook_onecbban {
  local($theban)=((split(/ +/, $_[0]))[2]);
  push(@n0thing_banlist, $theban);
  $silent=1;
}
sub hook_donebans {
  &remhook("367", "onecbban");
  &remhook("367", "oneubban");
  &remhook("368", "donebans");
  &n_unbans;
}
sub cmd_unban {
  &getarg;
  &tell("*\cb0\cb* Must specify a nick or address to unban"), return 
    unless $newarg;
  &tell("*\cb0\cb* You're not on a channel"), return unless $talkchannel;
  if ($newarg =~ /^\d+$/) {
    $n0thing_channel=$talkchannel;
    @n0thing_banlist=();
    while(1) {
      $newarg--;
      &tell("*\cb0\cb* No banlist available for $talkchannel"), return
	unless &eq($talkchannel, $n_svbanchannel);
      &tell("*\cb0\cb* No such ban"), return unless $n_svbanlist[$newarg];
      push(@n0thing_banlist, $n_svbanlist[$newarg]);
      &getarg;
      last unless $newarg;
      &tell("*\cb0\cb* Cannot mix nicks and numbers"), last
	unless $newarg =~ /^\d+$/;
    }
    &n_unbans;
  } else {
    @n0thing_banlist=();
    $n0thing_channel=$talkchannel;
    if ($newarg =~ /\@/) {
      $newarg="*!".$newarg unless $newarg =~ /\!/;
      $n_pat=$newarg;
      &addhook("367", "oneubban");
      &addhook("368", "donebans");
      &sl("MODE $talkchannel b");
    } else {
      &userhost($newarg, "&n_dounban;");
    }
  }
}
&addcmd("unban");
&docommand("^alias ub unban");
sub n_dounban {
  $n_pat=$who."!".$user."\@".$host;
  &addhook("368", "donebans");
  @n0thing_banlist=();
  $n0thing_channel=$talkchannel;
  &sl("MODE $talkchannel b");
}
sub hook_oneubban {
  local($theban)=((split(/ +/, $_[0]))[2]);
  local($tb0)=($theban);
  &slashify($theban);
  push(@n0thing_banlist, $tb0) if ($n_pat =~ /^${theban}$/i);
  $silent=1;
}
sub cmd_fkb {
  &getarg;
  &tell("*\cb0\cb* Must specify a nick to filter kick ban"), return unless $newarg;
  &tell("*\cb0\cb* You're not on a channel"), return unless $talkchannel;
  &userhost($newarg, "\$host=&n_hostpat(\$host);
&sl(\"MODE $talkchannel +b *!*\\\@\$host\"); &docommand(\"fk \$host $args\");");
}
&addcmd("fkb");
sub cmd_fk {
  &getarg;
  &tell("*\cb0\cb* Must specify a pattern to kick"), return unless $newarg;
  &tell("*\cb0\cb* You're not on a channel"), return unless $talkchannel;
  $n0thing_channel=$talkchannel;
  $n0thing_kickpat=$newarg;
  &slashify($n0thing_kickpat);
  $n0thing_kickmsg=$args || "though names may change each face retains the mask it wore";
  &addhook("352", "dofklist");
  &addhook("315", "donefk");
  &sl("WHO $talkchannel");
}
&addcmd("fk");
sub hook_dofklist {
  local($a)=@_;
  local($c, $c, $u, $h, $s, $n)=split(/ +/, $a);
  (($n."!".$u."\@".$h) =~ /${n0thing_kickpat}/i) && $n ne $nick &&
    &sl("KICK $n0thing_channel $n :$n0thing_kickmsg");
  $silent=1;
}
sub hook_donefk {
  &remhook("352", "dofklist");
  &remhook("315", "donefk");
}
sub cmd_setaway {
  &me("is away - $args - messages will be logged") if $talkchannel;
  push(@n_savemsg, "-- set away on " . &date(time) . " -- $args");
  %n_awaysent=();
  $args="\cb\cb" if $args eq '';
  &sl("AWAY :$args");
  $n_isaway=1;
}
&addcmd("setaway");
sub cmd_qsetaway {
  push(@n_savemsg, "-- set away on " . &date(time) . " -- $args");
  %n_awaysent=();
  $args="\cb\cb" if $args eq '';
  &sl("AWAY :$args");
  $n_isaway=1;
}
&addcmd("qsetaway");
  if ($n_isaway) {
    local($sec, $min, $hour)=localtime(time);
    local($w)=$who;
    $min="0".$min if $min =~ /^\d$/;
    push(@n_savemsg, ":\cb${who}\cb!${user}\@${host} (${hour}:${min}): $_[0]\n");
    $w =~ tr/A-Z/a-z/;
    &sl("NOTICE $who :Your messages are being logged - n0thing.pl"),
      $n_awaysent{$w}=1 unless $n_awaysent{$w};
  }
} 
&addhook("msg", "messages");
sub hook_dmessages {
  if ($n_isaway) {
    local($sec, $min, $hour)=localtime(time);
    $min="0".$min if $min =~ /^\d$/;
    push(@n_savemsg, "=\cb".$_[0]."\cb (${hour}:${min})= ".$_[1]."\n");
  }
}
&addhook("dcc_chat", "dmessages");
sub cmd_setback {
  $n_isaway='';
  &sl("AWAY");
  foreach (@n_savemsg) {
    &tell($_);
  }
  if ($#n_savemsg>0) {
    &getuserline("*** Forget saved messages (y/n)? ", "clear? ");
    @n_savemsg=() if /^y/i;
  } else {
    @n_savemsg=();
  }
}
&addcmd("setback");
sub cmd_clearmes {
  $n_isaway='';
  @n_savemsg=();
}
&addcmd("clearmes");
sub cmd_wall {
  &tell("*\cbE\cb* No text to send"), return unless $args;
  &tell("*\cbE\cb* You're not on a channel"), return unless $talkchannel;
  $wallop_text=$args;
  $wallop_channel=$talkchannel;
  &addhook("353", "wallopnames");
  &sl("NAMES $talkchannel");
}
&addcmd("wall");

sub hook_wallopnames {
  local($wallop, $n, $s, @wallop)=("", 0, 0, split(/ +/, $_[0]));
  shift @wallop; shift @wallop; shift @wallop;
  $wallop[0] =~ s/^://;
  foreach (@wallop) {
    $wallop.=$_.",", $n++ if s/^\@// || ($n_wallv && s/^\+//) || &eq($_, $nick);
    if ($n>=9) {
      $wallop =~ s/,$//;
      &sl("NOTICE $wallop :\cb[WallOp/$wallop_channel]\cb $wallop_text");
      $n=0;
      $s=1;
      $wallop='';
    }
  }
  if ($wallop eq '' && !$s) {
    &tell("*\cbE\cb* No ops to wallop");
  } elsif ($wallop ne '') {
    $wallop =~ s/,$//;
    &sl("NOTICE $wallop :\cb[WallOp/$wallop_channel]\cb $wallop_text");
  }
  &remhook("353", "wallopnames");
  $silent=1;
}
sub n_addwaitop {
  local($nck, $channel)=@_;
  if ($n_opdelay) {
    local($n)=($nck);
    $channel =~ tr/A-Z/a-z/;
    $n =~ s/(\W)/\\$1/g;
    if ($n_waitops{$channel} !~ /:${n}:/i) {
      $n_waitops{$channel} || ($n_waitops{$channel}=":");
      $n_waitops{$channel}.=$nck.":";
    }
    unless ($n_timerdue) {
      $n_timerdue=1;
      &timer($n_opdelay, "&n_doautoops");
    }
  } else {
    &sl("MODE $channel +o $nck");
  }
}

sub n_remwaitop {
  local($nck, $channel)=@_;
  $channel =~ tr/A-Z/a-z/;
  $nck =~ s/(\W)/\\$1/g;
  $n_waitops{$channel} =~ s/:${nck}:/:/gi;
  delete $n_waitops{$channel} if $n_waitops{$channel} eq ":";
}

sub n_doautoops {
  local($l);
  foreach $c (keys(%n_waitops)) {
    if ($n_waitops{$c} ne ":" && $n_waitops{$c}) {
      $l=$n_waitops{$c};
      $l =~ s/^://;
      $l =~ s/:$//;
      $l =~ tr/:/ /;
      &docommand("o ".$c." ".$l);
    }
    delete $n_waitops{$c};
  }
  $n_timerdue='';
}

sub hook_n_nickchange {
  local($oldnick)=$who;
  $oldnick =~ s/(\W)/\\$1/g;
  foreach $c (keys(%n_waitops)) {
    $n_waitops{$c} =~ s/:${oldnick}:/:$_[0]:/gi;
  }
}
&addhook("nick", "n_nickchange");

sub hook_n_parted {
  &n_remwaitop($who, $_[0]);
}
&addhook("leave", "n_parted");

sub hook_n_quitted {
  local($w)=$who;
  $w =~ s/(\W)/\\$1/g;
  foreach $c (keys(%n_waitops)) {
    $n_waitops{$c} =~ s/:${w}:/:/gi;
    delete $n_waitops{$c} if $n_waitops{$c} eq ":";
  }
}
&addhook("signoff", "n_quitted");

sub hook_n_modechange {
# we only deal with +o, +oo and the like; the worst that can happen
# is that we op someone again
  local ($l)=$_[1];
  return unless $_[0] =~ /^[\#\&\+]/;
  return unless $l =~ s/^\+o+\s+//;
  foreach $n (split(/ /, $l)) {
    &n_remwaitop($n, $_[0]);
  }
}
&addhook("mode", "n_modechange");

sub slashify {
  $_[0] =~ s/([^\\])\./$1\\./g;
  $_[0] =~ s/\*/\.\*/g;
  $_[0] =~ s/([^\.\*\?\\\w])/\\$1/g;
  $_[0] =~ tr/?/./;
}
sub autoopped {
  local($nuh, $chan, $asked)=@_;
# chan = what chan for, $asked = whether it was asked
  local($p);
  foreach $p (@n_autooplist) {
    return 1 if ($asked.":".$chan.":".$nuh) =~ /^${p}$/i;
  }
  return '';
}

sub addtoops {
  local($chan, $auto)=@_;
  if (&autoopped($who."!".$user."\@".$host, $chan, 1)) {
    &tell("*\cb0\cb* $who is already on your list");
  } else {
    $host=&n_hostpat($host);
    $user =~ s/^\~//;
    local($pat)=("*!*".$user."\@".$host);
    &tell("*\cb0\cb* Adding $who as $pat to your autoop list for $chan");
    &sl("NOTICE $who :You have been added to ${nick}'s autoop list for ".
      ($chan eq '*' ? "all channels" : "$chan"));
    &sl("NOTICE $who :You should feel *very* honored");
    &slashify($pat);
    &slashify($chan);
    push(@n_autooplist, $auto.":".$chan.":".$pat);
  }
}

sub cmd_addop {
  if ($args =~ /^([01])\s+(\S+)\s+(\S+)\s*$/) {
    local($auto, $chan, $uh)=($1, $2, $3);
    if ($chan ne '*' && $chan !~ /^[#&]/) {
      &tell("*\cbE\cb* Invalid channel - use '*' or a #channel name");
      return;
    }
    $chan =~ s/\'//g;
    $uh="*!".$uh if $uh =~ /\@/ && $uh !~ /\!/;
    $auto=($auto==0 ? "1" : ".");
    if ($uh =~ /\!.*\@/) {
      if (&autoopped($uh, $chan, 1)) {
	&tell("*\cb0\cb* $uh is already on your list");
	return;
      }
      &tell("*\cb0\cb* Adding $uh to your auto-op list for $chan");
      &slashify($uh);
      &slashify($chan);
      push(@n_autooplist, $auto.":".$chan.":".$uh);
    } else {
      &userhost($uh, "&addtoops('$chan', '$auto');");
    }
  } else {
    &tell("*\cb0\cb* Format: /addop (0 or 1) (#chan or *) (nick or user\@host)");
  }
}
&addcmd("addop");

sub hook_joined {
  local($c)=($_[0]);
  $c =~ tr/A-Z/a-z/;
  &n_addwaitop($who, $c) if ($n_autoops==1 && $haveops{$c} && 
			     &autoopped($who."!".$user."\@".$host, $c, 0));
}
&addhook("join", "joined");

sub hook_opbeg {
  if ($_[1] =~ /^op$/i && $n_autoops && &eq($nick, $_[0])) {
    local($chn, $pass)=split(/ +/, $_[2]);
    $chn =~ tr/A-Z/a-z/;
    &sl("MODE $chn +o $who")
      if $haveops{$chn} && $pass eq $set{"OPPASS"} &&
			&autoopped($who."!".$user."\@".$host, $chn, 1);
  }
}
&addhook("ctcp", "opbeg");

sub cmd_listop {
  local($n, $p, $q, $c, $a, $ch)=(0);
  &getarg;
  $ch=$newarg || '*';
  &tell("*\cb0\cb* That's the people in your autoop list:");
  foreach $p (@n_autooplist) {
    ($a, $c, $q)=split(/:/, $p);
    $c =~ s/\\//g;
    $c =~ s/\.\*/*/g;
    next if (!&eq($c, $ch) && $c ne '*' && $ch ne '*');
    $n++;
    $q =~ s/\\//g;
    $q =~ s/\.\*/*/g;
    &tell("*\cb0\cb* ".($a eq '.' ? "1 " : "0 ").$c." ".$q);
  }
  &tell("*\cb0\cb* which makes.. uhm... $n people, I think");
}
&addcmd("listop");

sub rem_matches {
  local($nuh, $ent)=@_;
  $ent=$' if $ent =~ /:.*:/;
  return 1 if $nuh =~ /${ent}$/i;
  # not exactly the same as slashify
  $nuh =~ s/\*/\.\*/g;
  $nuh =~ s/([^\\])\.([^\*])/$1\\\\*\\.$2/g;
  $nuh =~ s/([^\.\*\?\\\w])/\\\\*\\$1/g;
  $nuh =~ tr/?/./;
  return ($ent =~ /${nuh}$/i);
}

sub n_remop {
  local($nuh)=($who."!".$user."\@".$host);
  @n_autooplist=grep(!&rem_matches($nuh, $_), @n_autooplist);
}

sub cmd_remop {
  &getarg;
  if (!$newarg) {
    &tell("*\cb0\cb* Must specify a nick or address");
  } elsif ($newarg =~ /[\@\*]/) {
    # $newarg='*!'.$newarg if ($newarg !~ /\!/);
    &tell("*\cb0\cb* Time to remove $newarg from the autoop list");
    @n_autooplist=grep(!&rem_matches($newarg, $_), @n_autooplist);
  } else {
    &tell("*\cb0\cb* Time to remove $newarg from the autoop list");
    &userhost($newarg, "&n_remop;");
  }
}
&addcmd("remop");

sub cmd_clearop {
  @n_autooplist=();
  &tell("*\cb0\cb* Auto-op list cleared!");
}
&addcmd("clearop");
sub cmd_sve {
  local($p);
  if (!open(SVE, "> ".$n_svefile)) {
    &tell("*\cbE\cb* can't write to save file");
    return;
  }
  print SVE $n_autoops+0, "\n";
  print SVE ($n_autorejoin ? "1" : "0"), "\n";
  print SVE "\n";	# deprecated, pong reply
  print SVE $set{"FINGER"}, "\n";
  print SVE join(" ", keys(%notify)), "\n";
  print SVE join(" ", @n_autooplist), "\n";
  print SVE ($n_slowserver ? "1" : "0"), "\n";
  print SVE $n_opdelay, "\n";
  print SVE $n_quitmsg, "\n";
  print SVE "^\n";	# deprecated, command char
  print SVE ($n_reconnect ? "1" : "0"), "\n";
  print SVE $set{"AWAYONCE"}, "\n";
  print SVE $set{"REJOINCHANNELS"}, "\n";
  print SVE $set{"CTCP"}, "\n";
  print SVE $set{"PRINTCHAN"}, "\n";
  print SVE $set{"PRINTUH"}, "\n";
  print SVE $set{"SENDAHEAD"}, "\n";
  print SVE $set{"USERINFO"}, "\n";
  print SVE $set{"WALLV"}, "\n";
  print SVE $set{"OPPASS"}, "\n";
  print SVE $set{"IRCNAME"}, "\n";
  close SVE;
  &tell("*\cb0\cb* save completed!");
}
&addcmd("sve");

sub cmd_quit {
  if ($args eq '') {
    &docommand("/QUIT $n_quitmsg");
  } else {
    &docommand("/QUIT $args");
  }
}
&addcmd("quit");
&docommand("^alias bye quit");
&docommand("^alias exit quit");
&docommand("^alias signoff quit");
if (open(NOTHING, "< ".$n_svefile)) {
  local($n, $l);

  chop($l=<NOTHING>);
  &doset("AUTOOP", ($l ? "on" : "off"));

  chop($l=<NOTHING>);
  &doset("AUTOREJOIN", ($l ? "on" : "off"));

  <NOTHING>;

  chop($l=<NOTHING>);
  &doset("FINGER", $l) if $l ne '';

  chop($l=<NOTHING>);
  %notify=();
  foreach $n (split(' ', $l)) {
    &docommand("^NOTIFY ".$n);
  }

  chop($l=<NOTHING>);
  @n_autooplist=split(' ', $l);
  foreach (@n_autooplist) {
    $_=".:.*:".$_ unless /:.*:/;
  }

  chop($l=<NOTHING>);
  &doset("SLOWSERVER", ($l ? "on" : "off"));

  chop($l=<NOTHING>);
  $l=5 if $l !~ /^\d+$/;
  &doset("OPDELAY", $l);

  chop($l=<NOTHING>);
  &doset("QUITMSG", $l);

  <NOTHING>;

  chop($l=<NOTHING>);
  $l=1 unless defined $l;
  &doset("RECONNECT", ($l ? "on" : "off"));

  chop($l=<NOTHING>);
  &doset("AWAYONCE", $l) if $l;

  chop($l=<NOTHING>);
  &doset("REJOINCHANNELS", $l) if $l;

  chop($l=<NOTHING>);
  &doset("CTCP", $l) if $l;

  chop($l=<NOTHING>);
  &doset("PRINTCHAN", $l) if $l;

  chop($l=<NOTHING>);
  &doset("PRINTUH", $l) if $l;

  chop($l=<NOTHING>);
  &doset("SENDAHEAD", $l) if $l;

  chop($l=<NOTHING>);
  &doset("USERINFO", $l) if $l;

  chop($l=<NOTHING>);
  &doset("WALLV", $l) if $l;

  chop($l=<NOTHING>);
  &doset("OPPASS", $l) if $l;

  chop($l=<NOTHING>);
  &doset("IRCNAME", $l) if $l;

  close(NOTHING);
}

