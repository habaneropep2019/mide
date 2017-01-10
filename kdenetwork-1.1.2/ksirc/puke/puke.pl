use Socket;
use Fcntl;

#
# Clean up if this is the second load.
#
# Don't close anything so we can be loaded twice.
#
#if($PUKEFd != undef){
#  &remsel($PUKEFd);
#  close($PUKEFd);
#  sleep(1);
#  $PUKEFd = undef;
#}

#
# Puke timeout waiting for messages
$PUKE_TIMEOUT = 10;

#
# Setup flag fo syncronous operation
# 1 for sync
# 0 for async/fly by the seat of your pants
#
$SYNC = 0;

#
# Setup debugging logger, comment out for production use
#
$DEBUG = 0;
if($DEBUG){
  open(LOG, ">msg-log") || warn "Failed to open log file: $!\n";
  select(LOG); $| = 1; select(STDOUT);
  print LOG "Start time: ". `date`;
}



# 
# Multi operation level handler, winId Based.
# 
# PUKE_HANDLER{Cmd}{winId} = sub();

%PUKE_HANDLER = ();

# 
# Default handler is called if no handler defined
# Default handlers defined in commands-handler.pl
# Single level PUKE_DEF_HANDLER{$cmd};
#

#%PUKE_DEF_HANDLER = ();

#require 'commands-perl.pl';
&docommand("/load commands-perl.pl");
#require 'commands-handler.pl';
&docommand("/load commands-handler.pl");

$PukeHeader = 42; # Alternating 1010 for 32 bits
$PukePacking = "Iiiiia*";  # 4 ints, followed by any number of of characters
$PukeMSize = length(pack($PukePacking, $PukeHeader, 0, 0, 0, 0, ""));

if(!$ENV{'PUKE_SOCKET'}) {
    $sock = $ENV{'HOME'} . "/.ksirc.socket";
}
else {
    $sock = $ENV{'PUKE_SOCKET'};
}

if($PUKEFd == undef){
    $PUKEFd = &newfh;
    $proto = getprotobyname('tcp');
    socket($PUKEFd, PF_UNIX, SOCK_STREAM, 0) || print "PUKE: Sock failed: $!\n";
    $sun = sockaddr_un($sock);
    print "*P* PUKE: Connecting to $sock\n";
    connect($PUKEFd,$sun) || (die "Puke: Connect failed: $!\n",$PUKEFailed=1);
    select($PUKEFd); $| = 1; select(STDOUT);
    #fcntl($PUKEFd, F_SETFL, O_NONBLOCK);
}

# Arg1: Command
# Arg2: WinId
# Arg3: iArg
# Arg4: cArg
sub PukeSendMessage {
  my($cmd, $winid, $iarg, $carg, $handler, $waitfor) = @_;
  #  print("PUKE: cArg message too long $cArg\n") if(length($carg) > 50);
  $PUKE_HANDLER{$cmd}{$winid} = $handler if $handler != undef;
  my $msg = pack($PukePacking, $PukeHeader, $cmd, $winid, $iarg, length($carg), $carg);
  syswrite($PUKEFd, $msg, length($msg));
  #  print STDERR "*** " . $msg . "\n";
  print LOG kgettimeofday() . " SEND message: CMD: $PUKE_NUM2NAME{$cmd} WIN: $winid IARG: $iarg LEN: " . length($carg) . " CARG: $carg\n" if $DEBUG;
  if($SYNC == 1 || $waitfor == 1){
    return &sel_PukeRecvMessage(1, $winid, -$cmd, $carg);
  }
  return ();
}

sub sel_PukeRecvMessage {
  ($wait, $wait_winid, $wait_cmd, $wait_carg) = @_;
  my($m);
  my($cmd, $winid, $iarg, $carg, $junk);

  while(1){
    my $old_a = $SIG{'alarm'};
    $SIG{'alarm'} = sub { die "alarm\n"; };
    my $old_time = alarm($PUKE_TIMEOUT);
    eval {
      $len = sysread($PUKEFd, $m, $PukeMSize);
    };
    if($@){
       print "*E* Timeout waiting for data for first sysread\n";
       $SIG{ALRM} = $old_a;
       alarm($old_time);
       return;
    }
    $SIG{ALRM} = $old_a;
    alarm($old_time);

    if($len== 0){
      &remsel($PUKEFd);
      close($PUKEFd);
      return;
    }
    #  print "Length: $len " . length($m) . "\n";
    ($header, $cmd, $winid, $iarg, $length, $carg) = unpack($PukePacking, $m);
    if($header != $PukeHeader){
      print("*E* Invalid message received! Discarding! Got: $header wanted: $PukeHeader\n");
      #      return;
    }
    if($length > 0){
      my $old_a = $SIG{'alarm'};
      $SIG{'alarm'} = sub { die "alarm\n"; };
      my $old_time = alarm($PUKE_TIMEOUT);
      eval {
	$clen = sysread($PUKEFd, $m2, $length);
      };
      if($@){
         print "*E* Timeout waiting for cArg data\n";
      }
      $SIG{ALRM} = $old_a;
      alarm($old_time);
      
      if($length != $clen){
        print "\n*E* Warning: wanted to read: $length got $clen\n";
      }
      $m .= $m2;
      ($header, $cmd, $winid, $iarg, $length, $carg) = unpack($PukePacking, $m);
    }
               #    print("PUKE: Got => $PUKE_NUM2NAME{$cmd}/$cmd\n");
               #    print("PUKE: Got: $cmd, $winid, $iarg, $length, $carg\n");
               #    print("\n");
    if($winid == undef){ $winid = 0; }
    $blah = $carg;
    $blah =~ s/\000//g;
    print LOG kgettimeofday() . " GOT  message: CMD: $PUKE_NUM2NAME{$cmd} WIN: $winid IARG: $iarg LEN: $length CARG: $blah\n" if $DEBUG;
    #
    # Check both $cmd and the correct reply -$cmd
    #
    my(%ARG) = ('iCommand' => $cmd,
                'iWinId' => $winid,
                'iArg' => $iarg,
                'cArg' => $carg);

    #  print "*I* Def handler: $PUKE_DEF_HANDLER{$cmd}\n";

    if($wait == 1 && $winid == $wait_winid && $wait_cmd == $cmd){
      print LOG kgettimeofday() . " WAIT message: CMD: $PUKE_NUM2NAME{$cmd} WIN: $winid IARG: $iarg LEN: $length CARG: $blah\n" if $DEBUG;
      ($wait, $wait_winid, $wait_cmd, $wait_carg) = ();
      return %ARG;
    }

    if($PUKE_HANDLER{-$cmd}{$winid}){ # one shot/command handler
      &{$PUKE_HANDLER{-$cmd}{$winid}}(\%ARG);
    } elsif ($PUKE_HANDLER{$cmd}{$winid}){
      &{$PUKE_HANDLER{$cmd}{$winid}}(\%ARG);
    } elsif ($PUKE_W_HANDLER{$cmd}{$winid}) { # widget specific handler
      &{$PUKE_W_HANDLER{$cmd}{$winid}}(\%ARG);
    } elsif ($PUKE_DEF_HANDLER{"$cmd"}) {# catch all
      &{$PUKE_DEF_HANDLER{"$cmd"}}(\%ARG);
    }
    else {
      #
      # If there was no handler this is a widget creation falling throuhg
      #

      if($wait == 1 && (substr($wait_carg,0,7) eq substr($carg,0,7))){
	print LOG kgettimeofday() . " WAI2 message: CMD: $PUKE_NUM2NAME{$cmd} WIN: $winid IARG: $iarg LEN: $length CARG: $blah\n" if $DEBUG;
	($wait, $wait_winid, $wait_cmd, $wait_carg) = ();
        return %ARG;
      }
      # No handler at all, unkown reply
      print("*E* PUKE: Got unkown command: $cmd/$PUKE_NUM2NAME{$cmd}\n");
      #    print("PUKE: Got: $cmd, $winid, $iarg, $carg\n");
    }

    #
    # If we're not waiting for a message, return
    #
    if(!$wait){
      ($wait, $wait_winid, $wait_cmd, $wait_carg) = ();
      return ();
    }
    
    my($rin, $rout) =('', '');
    vec($rin,fileno($PUKEFd),1) = 1;
    $nfound = select($rout=$rin, undef, undef, 1);
    if($nfound < 1){
      print "*E* PUKE: Timed out waiting for reply, returning null\n";
      print LOG kgettimeofday() . " FAIL message: CMD: $PUKE_NUM2NAME{$wait_cmd} WIN: $wait_winid IARG: ### LEN: $length CARG: $wait_carg\n" if $DEBUG;
      return ();
    }
  }
}

&addsel($PUKEFd, "PukeRecvMessage", 0);

# Basics are up and running, now init Puke/Ksirc Interface.

my(%ARG) = &PukeSendMessage($PUKE_SETUP, $::PUKE_CONTROLLER, 0, $server, undef, 1);

$PukeMSize = $ARG{'iArg'};
print "*P* Puke: Initial Setup complete\n";
print "*P* Puke: Communications operational\n";

