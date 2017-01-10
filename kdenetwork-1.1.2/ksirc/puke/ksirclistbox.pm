
&::PukeSendMessage($::PUKE_WIDGET_LOAD,
                     $::PUKE_CONTROLLER,
                     $::PWIDGET_KSIRCLISTBOX,
                     "ksirclistbox.so",
                     sub { my %ARG = %{shift()};
                     if($ARG{'iArg'} == 1){
                       print "*E* PKSircListBox Load failed!\n";
                     }
                     }
                  );


package PKSircListBox; #FOLD00
@ISA = qw(PListBox);
use strict;

sub new { #FOLD00
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_KSIRCLISTBOX;

  if($class eq 'PKSircListBox'){
    $self->create();
  }

  return $self;

}

sub DESTROY { #FOLD00
    my $self = shift;
    $self->SUPER::DESTROY(@_);
}


sub scrollToBottom {
  my $self = shift;

  my $force = shift;
  
  $self->sendMessage('iCommand' => $::PUKE_KSIRCLISTBOX_TOBOTTOM,
                     'iArg' => $force,
                     'CallBack' => sub {});

}



package main; #FOLD00

1;
