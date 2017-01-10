
&::PukeSendMessage($::PUKE_WIDGET_LOAD,
                     $::PUKE_CONTROLLER,
                     $::PWIDGET_ALISTBOX,
                     "palistbox.so",
                     sub { my %ARG = %{shift()};
                     if($ARG{'iArg'} == 1){
                       print "*E* PAListBox Load failed!\n";
                     }
                     }
                  );


package PAListBox; #FOLD00
@ISA = qw(PListBox);
use strict;

if($PListBox::usage == undef){
    $PListBox::usage = 0;
}

sub new { #FOLD00
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_ALISTBOX;

  if($class eq 'PAListBox'){
    $self->create();
  }

  $self->installHandler($::PUKE_LISTBOX_SELECTED_ACK, sub{$self->selected(@_)});

  return $self;

}

sub DESTROY { #FOLD00
    my $self = shift;
    $self->SUPER::DESTROY(@_);
    $PAListBox::usage--;
    if($PAListBox::usage == 0){
        &::PukeSendMessage($::PUKE_WIDGET_UNLOAD,
                           0,
                           $::PWIDGET_ALISTBOX,
                           "",
                           sub {}
                          );

    }
}

sub inSort {
  my $self = shift;
  my $text = shift;
  my $top = shift;

  $self->sendMessage('iCommand' => $::PUKE_LISTBOX_INSERT_SORT,
		     'cArg' => $text,
                     'iArg' => $text >= 1 ? 1 : 0,
                     'CallBack' => sub {});

}

sub isTop {
  my $self = shift;
  my $text = shift;

  my %ret = $self->sendMessage('iCommand' => $::PUKE_ALISTBOX_ISTOP,
			       'cArg' => $text,
			       'CallBack' => sub {});

  return $ret{'iArg'};

}

sub findNick {
  my $self = shift;
  my $text = shift;

  my %ret = $self->sendMessage('iCommand' => $::PUKE_ALISTBOX_FIND_NICK,
			       'cArg' => $text,
			       'CallBack' => sub {},
			       'WaitFor' => 1);

  return $ret{'iArg'};
  
}

sub smallHighligh {
  my $self = shift;
  my $text = shift;
  my $highlight = shift;

  $self->sendMessage('iCommand' => $::PUKE_ALISTBOX_SMALL_HIGHLIGHT,
		     'cArg' => $text,
		     'iArg' => $highlight,
		     'CallBack' => sub {});  

}

sub bigHighligh {
  my $self = shift;
  my $text = shift;
  my $highlight = shift;

  $self->sendMessage('iCommand' => $::PUKE_ALISTBOX_BIG_HIGHLIGHT,
		     'cArg' => $text,
		     'iArg' => $highlight,
		     'CallBack' => sub {});  

}


package main; #FOLD00

1;
