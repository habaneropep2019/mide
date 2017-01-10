
package PWidget;
@ISA = qw(PBase);
use strict;

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_WIDGET;

  # Default handlers
  $self->installHandler($::PUKE_WIDGET_EVENT_RESIZE, 
			sub {$self->resizeEvent(@_)});
  $self->installHandler($::PUKE_WIDGET_EVENT_PAINT, 
			sub {$self->paintEvent(@_)});
  $self->installHandler($::PUKE_WIDGET_EVENT_MOVE, 
			sub {$self->moveEvent(@_)});
  $self->installHandler($::PUKE_EVENT_UNKOWN, 
                          sub {$self->miscEvent(@_)});
  # Examplesto listen for mouse events
  #  $self->installHandler($::PUKE_WIDGET_EVENT_MOUSEBUTTONPRESS,
  #                        sub {$self->mousePressEvent(@_)});
  #$self->installHandler($::PUKE_WIDGET_EVENT_MOUSEBUTTONRELEASE,
  #                        sub {$self->mouseReleaseEvent(@_)});



  if($class eq 'PWidget'){
    $self->create();
  }

  return $self;

}

sub close {
  my $self = shift;
  
  $self->hide();

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_DELETE,
		    'CallBack' => sub {});

  $self->{DESTROYED} = 1;
  
}

sub show {
  my $self = shift;

  # make sure we are runable then show continue.
  my @ARG = ();
  #  $self->canRun($self, \&PWidget::show, \@ARG) || return;

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_SHOW);
}

sub hide {
  my $self = shift;
  $self->sendMessage('iCommand' => $::PUKE_WIDGET_HIDE);
}

sub repaint {
  my $self = shift;
  
  my $erase = shift; # 1 for erase and reapint

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_REPAINT,
		     'iArg' => $erase);
}

sub resize {
  my $self = shift;

  my $width = shift;
  my $height = shift;

  my $carg = "$width\t$height";

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_RESIZE,
		     'cArg' => $carg);

}

sub move {
  my $self = shift;

  my $x = shift;
  my $y = shift;

  my $carg = "$x\t$y";

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_MOVE,
		     'cArg' => $carg);

}

sub setMinimumSize {
  my $self = shift;

  my $w = shift;
  my $h = shift;

  my $carg = "$w\t$h";

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_SETMINSIZE,
		     'cArg' => $carg,
		     'CallBack' => sub {});

}

sub setMaximumSize {
  my $self = shift;

  my $w = shift;
  my $h = shift;

  my $carg = "$w\t$h";

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_SETMAXSIZE,
		     'cArg' => $carg,
		     'CallBack' => sub {});

}


sub setCaption {
  my $self = shift;

  my $text = shift;
  
  $self->sendMessage('iCommand' => $::PUKE_WIDGET_SETCAPTION,
		     'cArg' => $text,
		     'CallBack' => sub {});

}

sub resizeEvent {
  my $self = shift;

  my %ARG = %{shift()};

  my($h, $w, $oh, $ow) = unpack("iiii", $ARG{'cArg'});
  $self->{'height'} = $h;
  $self->{'width'} = $w;

}

sub paintEvent {
}

sub moveEvent {
  my $self = shift;

  my %ARG = %{shift()};

  my($x, $y, $ox, $oy) = unpack("iiii", $ARG{'cArg'});
  $self->{'x'} = $x;
  $self->{'y'} = $y;
  
}

sub miscEvent {
}

sub backgroundColor {
  my $self = shift;

  my %arg = $self->sendMessage('iCommand' => $::PUKE_WIDGET_GET_BACKGROUND_COLOUR,
                               'WaitFor' => 1);

  #  print "Got: " . $arg{'cArg'} . "\n";

  $arg{'cArg'} =~ s/\000//g;
  my ($r, $g, $b) = split(/,/, $arg{'cArg'});

  return ($r, $g, $b);
}

sub setBackgroundColor {
  my $self = shift;

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_SET_BACKGROUND_COLOUR,
                     'cArg' => pack("i3", shift(), shift(), shift()),
                     'CallBack' => sub {});

}

sub setBackgroundPixmap {
  my $self = shift;

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_SET_BACKGROUND_PIXMAP,
                     'cArg' => shift(),
                     'CallBack' => sub {});
}

sub setBackgroundMode {
  my $self = shift;

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_SET_BACKGROUND_MODE,
                     'iArg' => shift(),
                     'CallBack' => sub {});
}


sub setEnabled {
  my $self = shift;

  $self->sendMessage('iCommand' => $::PUKE_WIDGET_SET_ENABLED,
                     'iArg' => shift(),
                     'CallBack' => sub {});

}

sub recreate {
  my $self = shift;

  my $nparent = shift;
  my $wflags = shift; # Not used!
  
  $self->sendMessage('iCommand' => $::PUKE_WIDGET_RECREATE,
                     'iArg' => $nparent ? $nparent->{'iWinId'} : 0,
		     'cArg' => pack("iii", shift(), shift(), shift()),
		     'CallBack' => sub {});

}

package main;

1;
