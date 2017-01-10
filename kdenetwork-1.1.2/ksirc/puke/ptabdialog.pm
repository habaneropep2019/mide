
&::PukeSendMessage($PUKE_WIDGET_LOAD, 
                   $PUKE_CONTROLLER,
                   $PWIDGET_TABDIALOG,
		   "ptabdialog.so",
		   sub { my %ARG = %{shift()};
			 if($ARG{'iArg'} == 1){
			   print "*E* PTabDialog Load failed!\n";
			 }
		       }
		  );

package PTabDialog;
@ISA = qw(PWidget);
use strict;

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_TABDIALOG;

  if($class eq 'PTabDialog'){
    $self->create();
  }

  return $self;

}

sub addTab {
  my $self = shift;

  my $widget = shift;
  my $text = shift;

  if($widget->{iWinId} <= 0){
    print "*E* Trying to add invalid widget " . ref($widget) . "\n";
    return;
  }
  
  $self->sendMessage('iCommand' => $::PUKE_TABDIALOG_ADDTAB,
		     'iArg' => $widget->{iWinId},
		     'cArg' => $text,
                     'CallBack' => sub {});

}

package main;
