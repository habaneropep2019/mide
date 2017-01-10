sub rndchr {
  my $string = "";
  for(my $i = 0; $i < 8; $i++){
    $string .= chr(int(rand(26)) + 97); # More or less the alpahbet
  }
  return $string;
}

srand(time());

&timer(1, "&next_one", 1);

$state = 0;
$max_nicks = 100;
$min_nicks = 5;
$num_nicks = 0;
%list_nicks = ();
$repeat = 100000;
$count = 0;

@state = (\&join, \&part);

$line = "~#test~*#* Users on #test:";
for($i = 0; $i < $max_nicks; $i++){
  my($mynick) = rndchr();
  $list_nicks{$mynick} = 1;
  $is_op = rand(100);
  if($is_op > 50){
    $mynick = "@" . $mynick;
  }
  $line .= " " . $mynick;
  $num_nicks ++;
}
print "$line\n";

sub next_one {
  for(my($i) = 0; $i < 200; $i++){
    $goto_state = int(rand($#state+1));
    &{$state[$goto_state]};
  }
  if($count++ < $repeat){
    &timer(1, "&next_one", 1);
  }
}

sub join{
  return if $num_nicks > $max_nicks;
  my($mynick) = rndchr();
  $list_nicks{$mynick} = 1;
  print("~#test~*>* $mynick (blah\@blah) has joined channel #test\n");
  $is_op = rand(100);
  if($is_op > 75){
    print "~#test~*+* Mode change \"+o $mynick\" on channel #test by ChanServ\n";
  }
  $is_voice = rand(100);
  if($is_voice > 40){
    print "~#test~*+* Mode change \"+v $mynick\" on channel #test by ChanServ\n";
  }
  $num_nicks ++;
}

sub part{
  return if $num_nicks < $min_nicks;
  AGAIN: {
    my($times) = int(rand($num_nicks));
    for($i = 0; $i<= $times; $i++){
      ($mynick, $value) = each(%list_nicks);
    }
    return if $mynick eq '';
  }
  next AGAIN if $value != 1;
  $list_nicks{$mynick} = 0;
  print("~#test~*<* $mynick has left channel #test\n");

  delete $list_nicks{$mynick};
  $num_nicks --;
}
