
&::PukeSendMessage($PUKE_WIDGET_LOAD,
		   $PUKE_CONTROLLER, 
		   $PWIDGET_KSPROGRESS,
		   "pprogress.so",
		   sub { my %ARG = %{shift()};
			 if($ARG{'iArg'} == 1){
			   print "*E* PProgress Load failed!\n";
			 }
		       }
		  );

package PProgress;
@ISA = qw(PWidget);
use strict;

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_KSPROGRESS;

  $self->installHandler($::PUKE_KSPROGRESS_CANCEL_ACK,
                          sub {$self->cancelPressed(@_)});


  if($class eq 'PProgress'){
    $self->create();
  }

  return $self;

}

sub setTopText {
  my $self = shift;

  my $text = shift;
  
  $self->sendMessage('iCommand' => $::PUKE_KSPROGRESS_SET_TOPTEXT,
                     'cArg' => $text,
                     'CallBack' => sub {});

}

sub setBotText {
  my $self = shift;

  my $text = shift;
  
  $self->sendMessage('iCommand' => $::PUKE_KSPROGRESS_SET_BOTTEXT,
                     'cArg' => $text,
                     'CallBack' => sub {});

}

sub setRange {
  my $self = shift;

  my $lower = shift;
  my $upper = shift;

  my $carg = "$lower\t$upper";
  
  $self->sendMessage('iCommand' => $::PUKE_KSPROGRESS_SET_RANGE,
		     'cArg' => $carg,
                     'CallBack' => sub {});

}

sub setValue {
  my $self = shift;

  my $value = shift;
  
  $self->sendMessage('iCommand' => $::PUKE_KSPROGRESS_SET_VALUE,
		     'iArg' => $value,
                     'CallBack' => sub {});

}

sub cancelPressed {
  print "*E* Cancel pressed\n";
}

package main;
