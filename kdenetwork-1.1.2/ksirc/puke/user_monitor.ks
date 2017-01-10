&docommand("/load pbase.pm");
&docommand("/load pwidget.pm");
&docommand("/load pframe.pm");
&docommand("/load ptablevw.pm");
&docommand("/load plistbox.pm");
&docommand("/load pboxlayout.pm");
&docommand("/load plabel.pm");
&docommand("/load pmenudta.pm");
&docommand("/load ppopmenu.pm");
&docommand("/load pbutton.pm");
&docommand("/load ppushbt.pm");
&docommand("/load palistbox.pm");
&docommand("/load plined.pm");

use Net::SMTP;

my $WHO = "$ENV{HOME}/who_online.pl";
my $INFO = "$ENV{HOME}/mysql_fetch";

my %ALLOW_MULT = ();
$ALLOW_MULT{'asj'} = 1;
$ALLOW_MULT{'administrator'} = 1;

@PAGE_PPL = ('andrew', 'lee', 'william', 'seamus', 'gerry', 'jason', 'derik');

$PUKE_DEF_HANDLER{-999} = sub {};

package UserList;

@ISA = qw(PFrame);

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->create();

  my $gm = new PBoxLayout($self, $PBoxLayout::TopToBottom, 5);

  my $list_box = new PListBox($self);
  $gm->addWidget($list_box, 5);

  my $event_list = new PListBox($self);
  $event_list->setMaximumSize(110, 2000);
  $event_list->setMinimumSize(110, 1);
  #  $event_list->setFrameStyle($PFrame::Box|$PFrame::Raised);
  $event_list->setFrameStyle(0);
  $event_list->setBackgroundColor($self->backgroundColor());
  # Turn off the widget so people can't click in it
  $event_list->setEnabled(0);
  $event_list->setAutoScrollBar(0);
  $event_list->setScrollBar(0);
  $gm->addWidget($event_list, 5);

  my $gm_but = new PBoxLayout($PBoxLayout::LeftToRight, 5);
  $gm->addLayout($gm_but, 5);

  my $refresh_but = new PPushButton($self);
  $refresh_but->setMaximumSize(25, 2000);
  $refresh_but->setMinimumSize(25, 25);
  $refresh_but->installHandler($::PUKE_BUTTON_PRESSED_ACK, sub { } );
  $refresh_but->installHandler($::PUKE_BUTTON_RELEASED_ACK, sub { } );
  $refresh_but->installHandler($::PUKE_BUTTON_CLICKED_ACK, sub { &::docommand("refresh_users"); } );
  $refresh_but->setText("&Refresh Users");
  $gm_but->addWidget($refresh_but, 5);

  my $page_dialog = new pageDialog();
  $page_dialog->resize(265, 250);
  
  my $page_but = new PPushButton($self);
  $page_but->setMaximumSize(25, 2000);
  $page_but->setMinimumSize(25, 25);
  $page_but->installHandler($::PUKE_BUTTON_PRESSED_ACK, sub { } );
  $page_but->installHandler($::PUKE_BUTTON_RELEASED_ACK, sub { } );
  $page_but->installHandler($::PUKE_BUTTON_CLICKED_ACK, sub { $self->{'page_dialog'}->show(); } );
  $page_but->setText("&Page");
  $gm_but->addWidget($page_but, 5);


  my $user_count = new PLabel($self);
  $user_count->setMaximumSize(25, 2000);
  $user_count->setMinimumSize(25, 25);
  $user_count->setFrameStyle($PFrame::Box|$PFrame::Raised);
  $gm->addWidget($user_count, 5);

  $user_count->setText($list_box->{count});

  my $menu = new PPopupMenu();
  my $menu_online = $menu->insertText("Online Time");
  $menu->installMenu($menu_online, sub { &::say("online " . $self->{'list_box'}->currentText() . "\n"); });
  my $menu_info = $menu->insertText("User Information");
  $menu->installMenu($menu_info, sub { my $user =  $self->{'list_box'}->currentText(); print `$INFO $user`; });
  my $menu_info2 = $menu->insertText("Connection Information");
  $menu->installMenu($menu_info2, sub { my $exec = "$WHO info " . $self->{'list_box'}->currentText(); print `$exec`; });

  my $menu_rem = $menu->insertText("Remove User From List");
  $menu->installMenu($menu_rem, sub {
     my $user = $self->{'list_box'}->currentText();
     my $count = $self->{'list_box'}->current();
     $self->{'list_box'}->removeItem($count);
     if($users_online->{$user} > 0){
         $users_online->{$user}--;
     }
     if($users_online->{$user} == 0){
         delete $users_online->{$user};
     }
  });
  my $menu_kill = $menu->insertText("Disconnect User");
  $menu->installMenu($menu_kill, sub { my $exec = "$WHO reset " . $self->{'list_box'}->currentText(); print "Running: $exec\n"; system($exec); });

  # Install listner for mouse events
  
  $list_box->installHandler($::PUKE_WIDGET_EVENT_MOUSEBUTTONPRESS,
			      sub {$self->popupMenu(@_)});
  $list_box->installHandler($::PUKE_WIDGET_EVENT_MOUSEBUTTONRELEASE,
			      sub {$self->popdownMenu(@_)});


  @$self{'gm', 'list_box', 'user_count', 'max', 'event_list', 'menu', 'page_dialog', 'page_but', 'gm_but'} = ($gm, $list_box, $user_count, 0, $event_list, $menu, $page_dialog, $page_but, $gm_but);

  return $self;

}

sub DESTROY {
    #  $self->hide();
  $self->{'gm'}->DESTROY;
  delete $self->{'gm'};
  delete $self->{'gm2'};
  $self->{'list_box'}->DESTROY;
  delete $self->{'list_box'};
  $self->{'use_count'}->DESTROY;
  delete $self->{'use_count'};

  $self->SUPER::DESTROY();
}

sub popupMenu {
  my $self = shift;
  my $arg = shift;

  my($x, $y, $b, $s) = unpack("i4", $arg->{'cArg'});
  #  print "$x $y $b $s\n";

  # Only popup for button 2
  $self->{'menu'}->popupAtCurrent() if $b == 2;
}

sub popdownMenu {
  my $self = shift;

  $self->{'menu'}->hide();
}

sub addEvent {
    my $self = shift;

    my $event_list = $self->{'event_list'};
    
    while($event_list->{count} >= 10){
      $event_list->removeItem($event_list->{count});
    }

    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
    
    my $text = sprintf("%02d:%02d:%02d " . shift(), $hour, $min, $sec);
    
    $event_list->insertText($text, 0);
}

sub insertText {
  my $self = shift;
  
  $self->{'list_box'}->insertText(@_);

  if($self->{'list_box'}->{count} > $self->{'max'}){
      $self->{'max'} = $self->{'list_box'}->{count};
  }

  $self->{'user_count'}->setText("C: " . $self->{'list_box'}->{count} . " M: " . $self->{'max'});
}

sub removeItem {
  my $self = shift;

  $self->{'list_box'}->removeItem(@_);

  $self->{'user_count'}->setText("C: " . $self->{'list_box'}->{count} . " M: " . $self->{'max'});
}

sub text {
  my $self = shift;

  return $self->{'list_box'}->text(@_);
}

sub count {
  my $self = shift;

  return $self->{'list_box'}->{count};
}

sub AUTOLOAD {
  my $self = shift;

  return if $AUTOLOAD =~ /::DESTROY$/;
  
  $AUTOLOAD =~ /.+::(.+)/;
  return $self->{'list_box'}->$1(@_);
}

package pageDialog;

@ISA = qw(PFrame);

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);
  $self->create();

  my $gm = new PBoxLayout($self, $PBoxLayout::TopToBottom, 5);

  my $ppl_label = new PLabel($self);
  $ppl_label->setText("Person to page:");
  $ppl_label->setMaximumSize(25, 2000);
  $ppl_label->setMinimumSize(25, 25);
  $gm->addWidget($ppl_label, 5);
  
  my $page_ppl = new PListBox($self);
  $gm->addWidget($page_ppl, 5);

  my $msg_label = new PLabel($self);
  $msg_label->setText("Message to page:");
  $msg_label->setMaximumSize(25, 2000);
  $msg_label->setMinimumSize(25, 25);
  $gm->addWidget($msg_label, 5);

  my $page_msg = new PLineEdit($self);
  $page_msg->setMaximumSize(25, 2000);
  $page_msg->setMinimumSize(25, 25);
  $page_msg->setMaxLength(49);
  $page_msg->installHandler($::PUKE_EVENT_UNKOWN, sub {});
  $gm->addWidget($page_msg, 5);
  
  my $send_but = new PPushButton($self);
  $send_but->setMaximumSize(25, 2000);
  $send_but->setMinimumSize(25, 25);
  $send_but->installHandler($::PUKE_BUTTON_PRESSED_ACK, sub { } );
  $send_but->installHandler($::PUKE_BUTTON_RELEASED_ACK, sub { } );
  $send_but->installHandler($::PUKE_BUTTON_CLICKED_ACK, sub { &::docommand("msg #polarcom page " . $self->{'page_ppl'}->currentText() . " " . $self->{'page_msg'}->text()); $self->hide() } );
  $send_but->setText("&Send");
  $gm->addWidget($send_but, 5);

  $self->setCaption("Page User");

  @$self{'gm', 'page_ppl', 'page_msg', 'send_but', 'msg_label', 'ppl_label'} =
        ($gm,  $page_ppl,  $page_msg,  $send_but,  $msg_label, $ppl_label);

  return $self;

}

sub show {
  my $self = shift;
  my $page_ppl = $self->{'page_ppl'};

  $self->hide();

  my $c = $page_ppl->current();

  $page_ppl->clear();
  my $person;
  foreach $person (@main::PAGE_PPL) {
    $page_ppl->insertText($person, -1);
  }
  $page_ppl->setCurrentItem($c);

  $self->resize(265, 250);
  $self->recreate(0, 0, 265, 250, 1);

#  $self->SUPER::show();
#
#  $self->resize(265,250);
#  $self->move(400,270);
  
}


package main;


if($online == undef){
  $online = new UserList();
  $online->setCaption("Users Online");
  $online->show();
  $online->resize(196,740);
  $online->move(823,0);

  eval { 
  $main::polar = new PWidget();
  $main::polar->fetchWidget("199.247.156.200_toplevel");
  $main::polar->move(0,0);
  $main::polar->resize(816, 740);
  };
  if($@) {
     eval {
         $main::polar = new PWidget();
         $main::polar->fetchWidget("199.247.156.200_#polarcom_toplevel");
         $main::polar->move(0,0);
         $main::polar->resize(816, 740);
     };
  }
  $users_online = {};
}

sub hook_online_mon {
  my $channel = shift;
  my $msg = shift;
  return unless $channel eq '#polarcom';
  return unless $msg =~ /Login|Logoff/;
  return if $msg =~ /administrator/;

  if($msg =~ /ice: Login (\S+)/){
    my $nick = $1;
    #return if $nick =~ /administrator/;
    $nick =~ s/(\S+)\@\S+/$1/g;
    $online->addEvent("On: $nick");
    if($users_online->{$nick} > 0){
      my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
      my $date;
      chomp($date = `date`);
      my $times = $users_online->{$nick} + 1;
      &say("$date Duplicate login for $nick, logged on $times times") if (($main::nick eq 'action') || ($nick eq 'administrator'));
      $online->addEvent("Duplicate: $nick");
      if($ALLOW_MULT{$nick} != 1){
        my $exec = "$WHO reset " . $nick;
        print "*I* Running: $exec\n";
        system($exec);
        my $smtp = new Net::SMTP->new("mail.polarcom.com");
        $smtp->mail("sysadmin\@polarcom.com");
        $smtp->to("$nick\@polarcom.com", "sysadmin\@polarcom.com");
        $smtp->data();
        $smtp->datasend("To: $nick\@polarcom.com\n");
        $smtp->datasend("From: Polarcom System Admin <sysadmin\@polarcom.com>\n");
        $smtp->datasend("Subject: Security Alert\n\n\n");
        $smtp->datasend("To: $nick\n");
        $smtp->datasend("Your account was simultaneously access by 2 or more users.\n");
        $smtp->datasend("The second user was automatically terminated.\n\n");
        $smtp->datasend("If you have any question, please email sysadmin\@polarcom.com\n");
        $smtp->datasend("-- Security Monitor\n");
        $smtp->datasend();
        $smtp->quit();
      }
      else {
        print "*I* $nick not being removed, in exclude list\n";
      }
    }
    # Make the list sorted
    my $i = 0;
    SEARCH: while($online->text($i) ne undef){

      if(($online->text($i) cmp $nick) >= 0){
	last SEARCH;
      }
      $i++;
    }
    $online->insertText($nick, $i);
    $users_online->{$nick}++;
  }
  elsif($msg =~ /ice: Logoff (\S+)/){
    my $i = 0;
    my $nick = $1;
    $nick =~ s/(\S+)\@\S+/$1/g;
    #    print "Trying to logoff: $nick\n";
    $online->addEvent("Off: $nick");
    if($users_online->{$nick} > 0){
      # print "$nick in list\n";
      $users_online->{$nick}--;
      if($users_online->{$nick} == 0){
         delete $users_online->{$nick};
      }
      while($online->text($i) ne undef){
	if($online->text($i) eq $nick){
          #   print "Removing $i for $nick which is really: " . $online->text($i) . "\n";
            $online->removeItem($i);
	  return;
	}
	$i++;
      }
    }
  }
}

&addhook("public", "online_mon");

sub hook_get_users {
  my $mesg = shift;

  if($mesg =~ /^send users online/){
    my $reply = 'Online: ';
    for($i = 0; $i <= $online->count(); $i++){
      $reply .= $online->text($i) . " ";
    }
    &msg($who, $reply);
  }
  if($mesg = /Online: (.+)\s+$/){
    &tell("*I* Updating user list from $who");
    $online->addEvent("Update user list: $who");
    my $users = $1;
    while($online->count() > 0){
        $online->removeItem(0);
    }
    $users_online = {};
    foreach $user (split(/\s+/, $users)){
      $users_online->{$user}++;
      $online->insertText($user, -1);
    }
  }
}

&addhook("msg", "get_users");

sub cmd_refresh_users {
  $online->clear();
  $users_online = {};
  my $output = `$WHO list`;
  my @users = split(/\n/, $output);
  #    while($online->count() > 0){
  #    $online->removeItem(0);
  #}
  foreach $user (@users){
    $user =~ s/(\S+)\@\S+/$1/g;
    next if $user eq '';
    next if $user =~ /administrator/;
    $users_online->{$user}++;
    $online->insertText($user, -1);
  }
}

&addcmd("refresh_users");
