
package PBoxLayout;
@ISA = qw(PBase);
use strict;

# setup default handlers

$PBoxLayout::LeftToRight = 0;
$PBoxLayout::RightToLeft = 1;
$PBoxLayout::TopToBottom = 2;
$PBoxLayout::BottomToTop = 3;

$PBoxLayout::AlignLeft        = 0x0001;
$PBoxLayout::AlignRight       = 0x0002;
$PboxLayout::AlignHCenter     = 0x0004;
$PBoxLayout::AlignTop         = 0x0008;
$PBoxLayout::AlignBottom      = 0x0010;
$PBoxLayout::AlignVCenter     = 0x0020;
$PBoxLayout::AlignCenter      = $PBoxLayout::AlignVCenter | 
                                $PBoxLayout::AlignHCenter;

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  my $widget = shift;

  #  print "Widget: " . ref($widget) . "\n";

  #  if(ref($widget) eq ''){
  #  print "*E* Error Creating PBoxLayout, did not give valid parent\n";
  #  return;
  #}
  #  elsif(ref($widget) eq 'PBoxLayout'){
  #  $self->{Parent} = $widget;
  #  $self->{ParentType} = 'Layout';
  #  $self->{Direction} = shift;
  #  $self->{Border} = shift;
  #  $self->{Added} = 0;
  #}
  #  else{
  if(ref($widget) ne ''){
      #    print "*\cbI\cb* Generic Non-topleve layout type\n";
    $self->{Parent} = $widget;
    $self->{ParentType} = 'Widget';
    $self->{Direction} = shift;
    $self->{Border} = shift;
    $self->{Added} = 1;
  }
  else{
    $self->{Parent} = undef;
    $self->{ParentType} = 'Layout';
    $self->{Direction} = $widget;
    $self->{Border} = shift;
    $self->{Added} = 0;

  }

  $self->{IAmALayout} = 1;
  $self->{Widgets} = ();

  $self->create();

  return $self;

}

sub create {
  my $self = shift;

  #
  # PLayout redefines create since it uses a special cArg
  #
  my($paren_id) = 0;
  $paren_id = $self->{Parent}->{iWinId} if $self->{Parent} != -1;

  if($paren_id eq ''){
    $paren_id = "0";
  }

  my $carg =  $paren_id . "\t" . $::POBJECT_LAYOUT . "\t" . $self->{Direction} . "\t" . $self->{Border} . "\t" . $self->{initId},
  
  my %REPLY;
  %REPLY = $self->sendMessage('iCommand' => $::PUKE_WIDGET_CREATE,
                              'iWinId' => $::PUKE_CONTROLLER,
                              'cArg' => $carg,
                              'CallBack' => sub { },
                              'WaitFor' => 1);
  
  $self->ackWinId(%REPLY);

}

sub addWidget {
  my $self = shift;

  my $widget = shift;
  my $stretch = shift;
  my $align = shift;

  if($self->{Added} == 0){
    print "*E* Burp: Can't add widget without first being added to parent layout\n";
    return;
  }

  $align = $PBoxLayout::AlignCenter if $align == undef;
  $stretch = 0 if $stretch == undef;

  #  $widget->immortal(); # If it's a widget, it cannot be deleted
  if($widget->{iWinId} <= 0){
    print "*E* Trying to add invalid widget " . ref($widget) . "\n";
    return;
  }

  my $cArg = pack("CC", $stretch, $align);
  
  $self->sendMessage('iCommand' => $::PUKE_LAYOUT_ADDWIDGET,
		     'iArg' => $widget->{iWinId},
		     'cArg' => $cArg,
                     'CallBack' => sub { },
                     'WaitFor' => 1);

  $self->{Widgets}->[ $#{$self->{Widgets}} + 1] = $widget;
  
}

sub addLayout {
  my $self = shift;

  if($self->{Added} == 0){
    print "*E* Burp: Can't add layout without first being added to parent layout\n";
  }

  
  my $layout = shift;
  if(ref($layout) ne 'PBoxLayout'){
    print "*E* Passed non layout type to addLayout\n";
    return 1;
  }

  if($layout->{iWinId} <= 0){
    print "*E* Trying to add invalid layout " . ref($layout) . "\n";
    return;
  }


  # make sure we can run, and the widget we want to add can run.
  #  my @ARG = ($layout);
  #$self->canRun($self, \&PBoxLayout::addLayout, \@ARG) || return;
  #$layout->canRun($self, \&PBoxLayout::addLayout, \@ARG) || return;

  my %REPLY = $self->sendMessage('iCommand' => $::PUKE_LAYOUT_ADDLAYOUT,
                                 'iWinId' => $self->{iWinId},
                                 'iArg' => $layout->{iWinId},
                                 'cArg' => pack("C", 0),
                                 'CallBack' => sub { },
                                 'WaitFor' => 1);

  #  print "*I* Adding layout\n";
  if($REPLY{'iArg'} != 0){
    print "*E* AddLayout call failed\n";
  }
  else{
    #    print "*I* Added new Layout for " . $layout->{iWinId} . "\n";
    $layout->{Added} = 1;
  }

}

sub DESTROY {
  my $self = shift;

  #  print "*I* Layout Deleted\n";

  #  if($self->{DESTROYED} != 1){
  #  $self->sendMessage('iCommand' => $::PUKE_WIDGET_DELETE,
  #      	       'CallBack' => sub { print "Deleted\n"; });
  #}

  $self->{iWinId} = -1;
  $self->{DESTROYED} = 1;

}

sub activate {
  my $self = shift;
  
  if($self->{ParentType} != 'Widget'){
    print "*E* Only call for TopLevel managers\n";
    return;
  }

  $self->sendMessage('iCommand' => $::PUKE_LAYOUT_ACTIVATE,
                     'CallBack' => sub { },
                     'WaitFor' => 1);

}


package main;
1;
