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
$max_win = 5;
$win_open = 0;
%wins = {};
$repeat = 1000;
$count = 0;


sub next_one {
  #
  # State 0 is open window
  #
  if($state == 0){
    $winnum = int(rand($max_win));
    $winname = "#" . rndchr();
    $wins{$winname} = 1;
    print("~$winname~*>* You have joined channel $winname\n");
    &docommand("/join $winname");
    $state = 1;
    $win_open ++;
  }
  #
  # State 1 is print stuff to channel
  #
  elsif($state == 1){
    while(($winname, $value) = each(%wins)){
      &msg("$winname", rndchr());
      &notice("$winname", rndchr());
    }
    $state = 2;
  }
  #
  # State 2 is close window
  #
  elsif($state == 2){
    if($win_open > $max_win){
      $times = int(rand($win_open-1));
      for($i = 0; $i<= $times; $i++){
	($winname, $value) = each(%wins);
      }
      print("~#test~ <boren> leaving $winname\n");
      delete($wins{$winname});
      print("~!all~*<* You have left channel $winname\n");
      &docommand("/part $winname");
      $win_open --;
    }
    $state = 0;
  }

  if($count++ < $repeat){
    &timer(5, "&next_one", 1);
  }
}