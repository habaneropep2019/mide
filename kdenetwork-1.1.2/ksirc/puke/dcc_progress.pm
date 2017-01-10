
&docommand("/load pprogress.pm");

package DCCProgress;

@ISA = qw(PProgress);
use strict;

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);
  
  $self->create();

  $self->installHandler($::PUKE_KSPROGRESS_CANCEL_ACK,
                          sub {$self->cancelPressed(@_)});

  return $self;
}


sub setRange {
  my $self = shift;
  my $min = shift;
  my $max = shift;

  $self->{'min'} = $min;
  $self->{'max'} = $max;
  $self->{'step'} = ($max - $min) / 100;
  $self->{'step'} = 10240 if $self->{'step'} == 0; # if sized messed don't divide by 0 :)
  $self->PProgress::setRange(0, 100);
}

sub setValue {
  my $self = shift;

  my $value = shift;

  my $steps = int($value / $self->{'step'});

  $self->PProgress::setValue($steps);
}

sub setCancel {
  my $self = shift;
  $self->{'cancelMessage'} = shift;
}

sub cancelPressed {
  my $self = shift;

  &::docommand($self->{'cancelMessage'});
  $self->hide;
  
}