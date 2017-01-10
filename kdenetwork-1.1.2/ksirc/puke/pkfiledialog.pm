
&::PukeSendMessage($PUKE_WIDGET_LOAD, 
		   $::PUKE_CONTROLLER, 
		   $PWIDGET_KFILEDIALOG,
		   "pkfiledialog.so",
		   sub { my %ARG = %{shift()};
			 if($ARG{'iArg'} == 1){
			   print "*E* PKFileDialog Load failed!\n";
			 }
		       }
                  );

use strict;

package PKFileDialog;
use vars qw(@ISA);
@ISA = qw(PWidget);

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_KFILEDIALOG;

  if($class eq 'PKFileDialog'){
    $self->create();
  }

  $self->installHandler($::PUKE_KBFD_FILE_SELECTED_ACK, sub{$self->fileSelected(shift())});

  return $self;

}

sub setDir {
  my $self = shift;

  my $dir = shift;
  
  $self->sendMessage('iCommand' => $::PUKE_KBFD_SET_PATH,
                     'cArg' => $dir,
                     'CallBack' => sub{});
  
}

sub setFilter {
  my $self = shift;

  my $filter = shift;
  
  $self->sendMessage('iCommand' => $::PUKE_KBFD_SET_FILTER,
		     'cArg' => $filter,
                     'CallBack' => sub{});
  
}

sub setSelected {
  my $self = shift;

  my $sel = shift;
  
  $self->sendMessage('iCommand' => $::PUKE_KBFD_SET_SELECTION,
		     'cArg' => $sel,
                     'CallBack' => sub{});
  
}

sub fileSelected {
  my $self = shift;
  my $rargs = shift;

  &::print("*I* File Selected: " . $rargs->{'cArg'});
  
}

package main;
