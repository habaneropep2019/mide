
&::PukeSendMessage($::PUKE_WIDGET_LOAD,
                     $::PUKE_CONTROLLER,
                     $::PWIDGET_TABLEVW,
                     "ptablevw.so",
                     sub { my %ARG = %{shift()};
                     if($ARG{'iArg'} == 1){
                       print "*E* PTableView Load failed!\n";
                     }
                     }
                  );


package PTableView;
@ISA = qw(PFrame);
use strict;

if($PTableView::usage == undef){
    $PTableView::usage = 0;
}

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  if($PTableView::usage == 0){
  }
  $PTableView::usage++;
  
  $self->{widgetType} = $::PWIDGET_TABLEVW;

  if($class eq 'PTableView'){
    $self->create();
  }

  return $self;

}

sub DESTROY {
    my $self = shift;
    $self->SUPER::DESTROY(@_);
    $PTableView::usage--;
    if($PTableView::usage == 0){
      #        &::PukeSendMessage($::PUKE_WIDGET_UNLOAD,
      #                     0,
      #                     $::PWIDGET_TABLEVW,
      #                     "",
      #                     sub {}
      #                    );

    }
}


package main;
1;
