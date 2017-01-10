#use lib "/opt/kde/share/apps/ksirc";

&docommand("/load pbase.pm");
&docommand("/load pwidget.pm");
&docommand("/load pframe.pm");
&docommand("/load ptablevw.pm");
&docommand("/load plistbox.pm");
&docommand("/load pbutton.pm");;
&docommand("/load pboxlayout.pm");
&docommand("/load plabel.pm");
&docommand("/load pbutton.pm");
&docommand("/load ppushbt.pm");
&docommand("/load plined.pm");
&docommand("/load pkfiledialog.pm");
&docommand("/load pmenudta.pm");
&docommand("/load ppopmenu.pm");

&docommand("/load dcc_progress.pm");

&docommand("/load plined.pm");
&docommand("/load pprogress.pm");  

use POSIX qw(getcwd);
use strict;

package DCCSendDialog;
use vars qw(@ISA);
@ISA = qw(PFrame);

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);
  $self->create();  

  my $gm_main = new PBoxLayout($self, $PBoxLayout::TopToBottom, 5);

  my $gm_to = new PBoxLayout($PBoxLayout::LeftToRight, 5);
  $gm_main->addLayout($gm_to);
  
  my $label_to = new PLabel($self);
  $label_to->setText("To Nick:");
  $label_to->setMaximumSize(1000,30);
  $label_to->setMinimumSize(50,30);
  $gm_to->addWidget($label_to, 0, $PBoxLayout::AlignCenter);
  
  my $line_to = new PLineEdit($self);
  $gm_to->addWidget($line_to, 5, $PBoxLayout::AlignCenter);

  my $gm_file = new PBoxLayout($PBoxLayout::LeftToRight, 5);
  $gm_main->addLayout($gm_file);

  my $label_file = new PLabel($self);
  $label_file->setText("Filename:");
  $label_file->setMaximumSize(1000,30);
  $label_file->setMinimumSize(50,30);
  $gm_file->addWidget($label_file, 0, $PBoxLayout::AlignCenter);
  
  my $line_file = new PLineEdit($self);
  $gm_file->addWidget($line_file, 5, $PBoxLayout::AlignLeft);

  my $button_file = new PPushButton($self);
  $button_file->setText("&Browse");
  $button_file->installHandler($::PUKE_BUTTON_CLICKED_ACK, sub{$self->browseClicked});
  $gm_file->addWidget($button_file, 2, $PBoxLayout::AlignRight);

  my $button_send = new PPushButton($self);
  $button_send->setText("&Send");
  $button_send->installHandler($::PUKE_BUTTON_CLICKED_ACK, sub{$self->sendClicked});
  $gm_main->addWidget($button_send, 4, $PBoxLayout::AlignRight);

  @$self{'gm_main', 'gm_to', 'gm_file', 'label_to', 'line_to', 'label_file', 'line_file', 'button_file', 'button_send'}
      = ($gm_main,  $gm_to,  $gm_file,  $label_to,  $line_to,  $label_file,  $line_file,  $button_file,  $button_send);

  print "*I* Finished creating DCCSend\n";

  $self->{fileDialog} = new PKFileDialog();
  my $dlg =   $self->{fileDialog};
  $dlg->setDir(POSIX::getcwd());
  $dlg->installHandler($::PUKE_KBFD_FILE_SELECTED_ACK, sub{$self->fileSelected(shift())});

  $self->setMinimumSize(450, 110);
  $self->setMaximumSize(2000, 2000);
  $self->resize(450, 110);

  return $self;
  
}

sub browseClicked {
  my $self = shift;
  
  $self->{fileDialog}->show();

}

sub fileSelected {
  my $self = shift;

  my $hargs = shift;
  
  $self->{fileDialog}->hide();
  
  my $file = $hargs->{'cArg'};

  if($file ne ''){
    $self->{'line_file'}->setText($file);
  }
}

sub sendClicked {
  my $self = shift;

  my $to_nick = $self->{'line_to'}->text();
  my $to_file = $self->{'line_file'}->text();

  if($to_nick eq '' || $to_file eq ''){
    return;
  }

  &::docommand("dcc send $to_nick $to_file");
  $self->hide();  
}


use vars qw(@ISA $KSIRC_DCC %KSIRC_DCC);

package DCCStatus;
use vars qw(@ISA);
@ISA = qw(PFrame);

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);
  $self->create();  

  my $gm_main = new PBoxLayout($self, $PBoxLayout::TopToBottom, 5);

  my $label = new PLabel($self);
  $label->setText("Pending DCC");
  $label->setMaximumSize(1000,20);
  $gm_main->addWidget($label, 5, $PBoxLayout::AlignCenter);  
  my $lb = new PListBox($self);
  $gm_main->addWidget($lb, 5, $PBoxLayout::AlignCenter);

  my $gm_but1 = new PBoxLayout($PBoxLayout::LeftToRight, 5);
  $gm_main->addLayout($gm_but1);
  
  my $button_get = new PPushButton($self);
  $button_get->setText("&Open Connection");
  $button_get->setMaximumSize(1000,30);
  $button_get->setMinimumSize(10,30);
  $button_get->installHandler($::PUKE_BUTTON_CLICKED_ACK, sub{$self->openClicked});
  $gm_but1->addWidget($button_get, 5);

  my $button_forget = new PPushButton($self);
  $button_forget->setText("&Forget Connection");
  $button_forget->setMaximumSize(1000,30);
  $button_forget->setMinimumSize(10,30);
  $button_forget->installHandler($::PUKE_BUTTON_CLICKED_ACK, sub{$self->forgetClicked});
  $gm_but1->addWidget($button_forget, 5);

  my $gm_but2 = new PBoxLayout($PBoxLayout::LeftToRight, 5);
  $gm_main->addLayout($gm_but2);
  
  my $button_send = new PPushButton($self);
  $button_send->setText("&Send File");
  $button_send->setMaximumSize(1000,30);
  $button_send->setMinimumSize(10,30);
  $button_send->installHandler($::PUKE_BUTTON_CLICKED_ACK, sub{$self->sendClicked});
  $gm_but2->addWidget($button_send, 5);
  
  $gm_main->activate();

  $self->setMinimumSize(400, 275);
  $self->setMaximumSize(2000, 2000);
  $self->resize(400, 275);
  
  @$self{'gm_main', 'label1', 'lb', 'button_get', 'button_chat', 'button_forget', 'gm_but1', 'gm_but2', 'button_send' }
      = ( $gm_main, $label,   $lb,  $button_get,  $button_send,  $button_forget,  $gm_but1,  $gm_but2,  $button_send );


  print "*I* Finished creating DCCStatus\n";

  return $self;
}

sub addItem {
  my $self = shift;

  my %hargs = @_;

  my $line = $hargs{'line'};

  if($self->{'lines'}->{$line}){
    return -1;
  }

  $self->{'lines'}->{$line}->{'OpenCode'} = $hargs{'open'};
  $self->{'lines'}->{$line}->{'ForgetCode'} = $hargs{'forget'};

  $self->{'lb'}->insertText($line, -1);
  if($self->{'lb'}->currentText() eq ''){
    $self->{'lb'}->setCurrentItem(0);
  }

  return 0;
}

sub openClicked {
  my $self = shift;

  my $line = $self->{'lb'}->currentText();
  if($line eq ''){
    return;
  }

  &{$self->{'lines'}->{$line}->{'OpenCode'}};
  $self->{'lb'}->removeItem($self->{'lb'}->current());

  delete $self->{'lines'}->{$line};
  
}

sub forgetClicked {
  my $self = shift;

  my $line = $self->{'lb'}->currentText();
  if($line eq ''){
    return;
  }

  &{$self->{'lines'}->{$line}->{'ForgetCode'}};
  $self->{'lb'}->removeItem($self->{'lb'}->current());

  delete $self->{'lines'}->{$line};
  
}

sub sendClicked {
  my $self = shift;

  if(!$self->{sendDialog}){
    my $dlg =  new DCCSendDialog();
    if($dlg == undef){
      &print("*E* Could not load DCCSendDialog");
      return;
    }
    $self->{sendDialog} = $dlg;
  }

  $self->{sendDialog}->show();  
}

sub DESTROY {
  print "*E* Destroying dcc status widget\n";
  shift()->close();
}

sub close {
  my $self = shift;
  $self->sendMessage('iCommand' => $::PUKE_WIDGET_DELETE,
                     'CallBack' => sub {},
                     'WaitFor'  => 1);
  # $self->{'gm_main'}->DESTROY;
  #elete $self->{'gm_main'};
}

package DCCPopupMenu;
use vars qw(@ISA);
@ISA = qw(PPopupMenu);

sub new {
    my $class = shift;
    my $self = $class->SUPER::new($class, @_);
    $self->create();
    return $self;
}

sub insertText {
  my $self = shift;

  my $id = $self->SUPER::insertText(@_);

  my @arr;
  
  if(ref($self->{'Ids'}) ne 'ARRAY'){
      $self->{'Ids'} = \@arr;
  }
  
  @arr = @{$self->{'Ids'}};
  
  $arr[$#arr+1] = $id;

  $self->{'Ids'} = \@arr;

  return $id;
}

sub DESTROY {
  my $self = shift;

  my @arr = @{$self->{'Ids'}};
  my $id;

  foreach $id (@arr) {
    $self->removeItem($id);
  }

  $self->sendMessage('iCommand' => $::PUKE_RELEASEWIDGET,
                     'CallBack' => sub {});

  
}


package main;
use vars qw($KSIRC_DCC %KSIRC_DCC $who $KSIRC_DCCSTATUS $silent $nick $KSIRC_POPSC $KSIRC_POPDOCK);

sub hook_ksirc_dcc_request {
  my($type) = shift;
  my($machine) = shift;
  my($port) = shift;
  my($file) = shift;
  my($size) = shift;
  my($mwho) = $who;
  if($KSIRC_DCCSTATUS == undef){
    $KSIRC_DCCSTATUS = new DCCStatus;
    $KSIRC_DCCSTATUS->resize(400, 275);
  }

  if($type eq 'SEND'){
    my $open = sub {
      &docommand("/dcc get $mwho $file");
    };
    my $forget = sub {
      &docommand("/dcc close get $mwho $file");
    };
    $::KSIRC_FILE_SIZES{$file} = $size;
    $KSIRC_DCCSTATUS->addItem('line' => "SEND: $who offered $file at size $size",
                              'open' => $open,
                              'forget' => $forget);
  }
  elsif($type eq 'CHAT'){
    $KSIRC_DCCSTATUS->addItem('line' => "CHAT: $who",
                              'open' => sub { &docommand("/dcc chat $mwho"); },
                              'forget' => sub { &docommand("/dcc close chat $mwho"); });

  }
  $KSIRC_DCCSTATUS->show;

}

&addhook("dcc_request", "ksirc_dcc_request");

sub hook_ksirc_dcc_send {
  my $nick = shift;
  my $file = shift;
  my $size = shift;
  my $fh = shift;

  #  print "*I* Starting dcc into with: $nick, $file, $size, $fh\n";
  
  my($window) =  new DCCProgress;
  $size = 10240 if $size == 0;
  $window->setRange(0, $size);
  $window->setCaption("$file=>$nick");
  $window->setTopText("Sending: $file Size: $size");
  $window->setBotText("Status: pending");
  $window->setCancel("dcc close send $nick $file");
  $KSIRC_DCC{$fh}{$file}{'Window'} = $window;
  $KSIRC_DCC{$fh}{$file}{'StartTime'} = time() - 1;
  $window->show;
}

&addhook("dcc_send", "ksirc_dcc_send");

sub hook_ksirc_dcc_send_status {
  my $file = shift;
  my $bytes = shift;
  my $fh = shift;

  my $window = $KSIRC_DCC{$fh}{$file}{'Window'};
  if($window == undef){
    my($window) =  new DCCProgress;
    $window->setRange(0, 1);
    $window->setCaption("$file=>$nick");
    $window->setTopText("Sending: $file Size: Unkown");
    $window->setCancel("dcc close send $nick $file");
    $KSIRC_DCC{$fh}{$file}{'Window'} = $window;
    $KSIRC_DCC{$fh}{$file}{'StartTime'} = time() - 1;
    $window->show;
  }
  $window->setBotText("BPS: " . int($bytes/(time() -  $KSIRC_DCC{$fh}{$file}{'StartTime'})));
  $window->setValue($bytes);
}

&addhook("dcc_send_status", "ksirc_dcc_send_status");

sub hook_ksirc_dcc_get {
  my $nick = shift;
  my $file = shift;
  my $fh = shift;

  my $size = $::KSIRC_FILE_SIZES{$file};

#print "*I* Starting dcc into with: $nick, $file, $size, $fh\n";

  if($KSIRC_DCC{$fh}{$file}{'Window'} == undef){
    my($window) =  new DCCProgress;
    $size = 10240 if $size == 0;
    $window->setRange(0, $size);
    $window->setCaption("$file<=$nick");
    $window->setTopText("Receiver: $file Size: $size");
    $window->setBotText("Status: pending");
    $window->setCancel("dcc close get $nick $file");
    $KSIRC_DCC{$fh}{$file}{'Window'} = $window;
    $KSIRC_DCC{$fh}{$file}{'StartTime'} = time() - 1;
    $window->show;
  }
}

&addhook("dcc_get", "ksirc_dcc_get");

sub hook_ksirc_dcc_get_status {
  my $file = shift;
  my $bytes = shift;
  my $fh = shift;

  my $window = $KSIRC_DCC{$fh}{$file}{'Window'};
  if($window == undef){
    my($window) =  new DCCProgress;
    $window->setRange(0, 1);
    $window->setCaption("$file<=$nick");
    $window->setTopText("Receiver: $file Size: Unkown");
    $window->setBotText("Status: pending");
    $window->setCancel("dcc close get $nick $file");
    $KSIRC_DCC{$fh}{$file}{'Window'} = $window;
    $KSIRC_DCC{$fh}{$file}{'StartTime'} = time() - 1;
    $window->show;
  }
  $window->setBotText("BPS: " . int($bytes/(time() -  $KSIRC_DCC{$fh}{$file}{'StartTime'})));
  $window->setValue($bytes);
}

&addhook("dcc_get_status", "ksirc_dcc_get_status");


sub hook_ksirc_dcc_disconnect {
  my $nick = shift;
  my $file = shift;
  my $bytes = shift;
  my $time = shift;
  my $fh = shift;

  if($fh){
    my $window = $KSIRC_DCC{$fh}{$file}{'Window'};
    $window->close();
    delete $KSIRC_DCC{$fh}{$file};
    delete $KSIRC_DCC{$fh};
    print "*D* DCC transfer with $nick ($file) terminated; $bytes transferred in $time seconds (" . int(($bytes/($time+1))/1024) . "KBps)";
    $silent = 1;
  }
}

addhook("dcc_disconnect", "ksirc_dcc_disconnect");



&print("*I* Done DCC Status");
#$::test = new DCCStatus;
#$::test->resize(400, 275);
#$::test->show();

sub popup_dccstatus{
  if($KSIRC_DCCSTATUS == undef){
    $KSIRC_DCCSTATUS = new DCCStatus;
    $KSIRC_DCCSTATUS->resize(400, 275);
  }
  $KSIRC_DCCSTATUS->show();
}

sub popup_dccsend{
  if($KSIRC_DCCSTATUS == undef){
    $KSIRC_DCCSTATUS = new DCCStatus;
    $KSIRC_DCCSTATUS->resize(400, 275);
  }
  $KSIRC_DCCSTATUS->sendClicked();
}

if(!$KSIRC_POPSC){
  $KSIRC_POPSC = new DCCPopupMenu();
  if($KSIRC_POPSC->fetchWidget("servercontroller_menu_file") >= 0){
    my $id_control = $KSIRC_POPSC->insertText("Show DCC Control ($::server)");
    my $id_send =    $KSIRC_POPSC->insertText("Show DCC Send ($::server)");
    $KSIRC_POPSC->installMenu($id_control, sub{&popup_dccstatus();});
    $KSIRC_POPSC->installMenu($id_send, sub{&popup_dccsend();});
  }
}

if(!$KSIRC_POPDOCK){
  $KSIRC_POPDOCK = new DCCPopupMenu();
  if($KSIRC_POPDOCK->fetchWidget("dockServerController_menu_pop") >= 0){
    my $id_control = $KSIRC_POPDOCK->insertText("Show DCC Control ($::server)");
    my $id_send =    $KSIRC_POPDOCK->insertText("Show DCC Send ($::server)");
    $KSIRC_POPDOCK->installMenu($id_control, sub { &popup_dccstatus(); } );
    $KSIRC_POPDOCK->installMenu($id_send, sub { &popup_dccsend(); } );
  }
}

sub hook_quit_release {
  $KSIRC_POPDOCK->DESTROY();
  $KSIRC_POPSC->DESTROY();
}

&addhook("quit", "quit_release");

1;