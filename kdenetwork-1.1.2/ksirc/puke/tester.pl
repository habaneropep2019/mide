#!/usr/bin/perl

$sock = $ENV{'HOME'} . "/.ksirc.socket";

use Socket;

$proto = getprotobyname('tcp');
socket(fd, PF_UNIX, SOCK_STREAM, 0) || die "Sock failed: $!\n";
$sun = sockaddr_un($sock);
print "Connecting to $sock\n";
connect(fd,$sun) || die "Connect failed: $!\n";

select(fd); $| = 1; select(STDOUT);

while(1){
  print "Command: ";
  chomp($cmd = <STDIN>);
  print "WinId: ";
  chomp($winid = <STDIN>);
  print "iArg: ";
  chomp($iarg = <STDIN>);
  print "cArg: ";
  chomp($carg = <STDIN>);
  $m = pack("iiia50xx", $cmd, $winid, $iarg, $carg);
  print fd $m;
  
}

