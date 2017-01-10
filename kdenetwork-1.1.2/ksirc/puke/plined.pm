
&::PukeSendMessage($PUKE_WIDGET_LOAD, 
		   $::PUKE_CONTROLLER, 
		   $PWIDGET_LINED,
		   "plined.so",
		   sub { my %ARG = %{shift()};
			 if($ARG{'iArg'} == 1){
			   print "*E* PLineEdit Load failed!\n";
			 }
		       }
		  );

package PLineEdit;
@ISA = qw(PWidget);
use strict;

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_LINED;
  $self->{maxLength} = -1;

  if($class eq 'PLineEdit'){
    $self->create();
  }

  $self->installHandler($::PUKE_WIDGET_EVENT_TIMER, sub{});
  $self->installHandler($::PUKE_LINED_GET_TEXT_ACK, sub{
			my %ARG = %{shift()};
			$ARG{'cArg'} =~ s/^([^\000]*).*/$1/;
			$self->{text} = $ARG{'cArg'};});

  return $self;

}

sub setMaxLength {
  my $self = shift;

  my $length = shift;

  $self->{maxLength} = $length;
  $self->sendMessage('iCommand' => $::PUKE_LINED_SET_MAXLENGTH,
		     'iArg' => $length,
		     'CallBack' => sub {my %ARG = %{shift()};
					$self->{maxLength} = $ARG{'iArg'};});
  
}

sub setEchoMode {
  my $self = shift;

  my $mode = shift;

  $self->sendMessage('iCommand' => $::PUKE_LINED_SET_ECHOMODE,
		     'iArg' => $mode,
		     'CallBack' => sub {my %ARG = %{shift()};
					$self->{echoMode} = $ARG{'iArg'};});

}

sub setText {
  my $self = shift;
  
  my $text = shift;
  
  $self->{text} = $text;

  # Don't need the ouput since GET_TEXT_ACK will be called and
  # we'll set it there
  $self->sendMessage('iCommand' => $::PUKE_LINED_SET_TEXT,
		     'cArg' => $text,
		     'CallBack' => sub {});

}

sub text {
  my $self = shift;

  return $self->{text};
}

package main;
