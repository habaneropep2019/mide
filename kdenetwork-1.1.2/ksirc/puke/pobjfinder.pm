
&::PukeSendMessage($PUKE_WIDGET_LOAD, 
		   $::PUKE_CONTROLLER, 
		   $PWIDGET_OBJFINDER,
		   "pobjfinder.so",
		   sub { my %ARG = %{shift()};
			 if($ARG{'iArg'} == 1){
			   print "*E* PLabel Load failed!\n";
			 }
		       }
		  );

package PObjFinder;
@ISA = qw(PBase);
use strict;

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_OBJFINDER;

  if($class eq 'PObjFinder'){
    $self->create();
  }

  $self->installHandler($::PUKE_OBJFINDER_NEWOBJECT, sub {$self->newObject(shift())});
  
  return $self;

}

sub newObject {
}

sub interAllObjects {
  my $self = shift;
  my %REPLY = $self->sendMessage('iCommand' => $::PUKE_OBJFINDER_ALLOBJECTS,
                                 'WaitFor' => 1);

  return $REPLY{'cArg'};

}

sub allObjects {
  my $self = shift;
  my $line = $self->interAllObjects();

  $line =~ s/\S+::unnamed//gm;

  return $line;

}


sub allObjectsHash {
  my $self = shift;
  
  my $line = $self->allObjects();

  my $widget;
  my %hash;
  
  foreach $widget (split(/\n/, $line)){
    $hash{$widget} = 1;
  }

  return \%hash;
  
}

sub allObjectsArray {
  my $self = shift;
  
  my $line = $self->allObjects();

  my $widget;
  my @arr;
  
  foreach $widget (split(/\n/, $line)){
    next if $widget eq '';
    $arr[$#arr+1] = $widget;
  }

  return @arr;
}

package main;

