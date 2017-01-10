#
# Structure format is:
# $var[filter_number]{field}
#
# Defined fields: (* is required)
#    SEARCH(*): search filter
#    FROM(*): intput to subtitution regex
#    TO(*): resultant to substitution regex
#    DESC: description of filter rule
#

$#KSIRC_FILTER = 0;

print "*** Loading filter parser...\n";

$ksirc_dump = 0;

sub hook_ksircfilter {
  my($i) = 0;
  for(; $i <= $#KSIRC_FILTER; $i++){
    if($_[0] =~ m/$KSIRC_FILTER[$i]{'SEARCH'}/){
       $_[0] =~ s/$KSIRC_FILTER[$i]{'FROM'}/$KSIRC_FILTER[$i]{'TO'}/ee;
    }
  }
}

print "*** Filter Parser Loaded\n";

addhook("print", "ksircfilter");

sub cmd_ksircprintrule {
  my($i) = 0;
  for(; $i <= $#KSIRC_FILTER; $i++){
    print STDOUT "*** Rule $i: if ". $KSIRC_FILTER[$i]{SEARCH} . " then ". $KSIRC_FILTER[$i]{FROM} . " -> " . $KSIRC_FILTER[$i]{TO} . "\n";
  }
}

addcmd("ksircprintrule");
&docommand("^alias prule ksircprintrule");

# 
# Addrule command takes 4 or arguments seperated by " key==value !!! key2==value2 ||| etc" 
# 1. Name of rule
# 2. Pattern to search for
# 3. Substitution to take
# 4. Sub to make 

sub cmd_ksircappendrule {
  my($rule, %PARSED);
  foreach $rule (split(/ !!! /, $args)){
    my($key,$value) = split(/==/, $rule);
    $PARSED{$key} = $value;
  }
  if($PARSED{'DESC'} && $PARSED{'SEARCH'} && $PARSED{'FROM'} && $PARSED{'TO'}){
    my($i) = $#KSIRC_FILTER + 1;
    my($key, $value);
    while(($key, $value) = each %PARSED){
      $KSIRC_FILTER[$i]{$key} = $value;
    }
#    &tell("*** Added rule: " . $KSIRC_FILTER[$i]{'DESC'} . "\n");
  }
  else{
    print "*E* Parse Error in Rule, format is: name !!! search !!! from !!! to\n";
  }
}

addcmd("ksircappendrule");
&docommand("^alias arule ksircappendrule");

sub cmd_ksircclearrule {
  @KSIRC_FILTER = ();
#  &tell("*** ALL FILTER RULES ERASED - DEFAULTS ADDED\n");
}

&addcmd("ksircclearrule");
&docommand("^alias crule ksircclearrule");
&docommand("^crule");

sub cmd_ksircdelrule {
  if($args =~ /^\d+$/){
    splice(@KSIRC_FILTER, $args, 1);
    print STDOUT "*** Deleted rule: $args\n";
  }
  else {
    print STDOUT "*E* Syntax is ksircdelrule <rule number>\n";
  }
}

&addcmd("ksircdelrule");
&docommand("^alias drule ksircdelrule");

sub hook_ctcp_lag {
   if (($_[0] eq $nick) && ($_[1] eq 'LAG')) {
     my($_t) = kgettimeofday();
     $_t -= $_[2];
#     print "*** Diff: $_t\n";
     print "~!lag~*L* " . $_t . "\n";
     $skip = 1;
   } 
}

&addhook("ctcp", "ctcp_lag");

sub cmd_lag {
  if($away){
    print "~!lag~*L* AUAY\n";
    return;
  }
  my($_t) = kgettimeofday();
  &docommand("^ctcp $nick LAG $_t");
  my $c;
  foreach $c (@channels){
    &docommand("^extnames $c");
  }
}


eval {
require 'sys/syscall.ph';
};

if(! defined(&SYS_gettimeofday)){
  if(open(SYSCALL, "/usr/include/sys/syscall.h")){
    my(@line) = grep(/define\s+SYS_gettimeofday/, <SYSCALL>);
    close SYSCALL;
    print "*E* Strange syscall.h, SYS_gettimeofday defined more than once!\n" if $#line > 1;
    my($line) = "@line";
    if($line =~ /SYS_gettimeofday\s+(\d+)/){
      eval "sub SYS_gettimeofday () {$1;}";
      print "*** Set time of day to: $1\n";
    }
  }

  if(! defined(&SYS_gettimeofday)){
    eval 'sub SYS_gettimeofday () {78;}';
    print "*E* Unable to find SYS_gettimeofday, using LINUX default!\n";
  }
}


sub kgettimeofday {
  local($failed) = 0;
  local($TIMEVAL_T) = "LL";
  local($start) = pack($TIMEVAL_T, ());
  eval{
    (syscall( &SYS_gettimeofday, $start, 0) != -1) or $failed = 1;
  };
  if(!$failed){
    my(@start) = unpack($TIMEVAL_T, $start);
    $start[1] /= 1000000;
    return $start[0] + $start[1];
  }
  else {
    return time();
  }
}

&addcmd("lag");

sub hook_ksirc_notify_connected {
  &print('~!all~`#ssfe#R reconnected');
}

&addhook("376", "ksirc_notify_connected");   # join on the "end of MOTD"
