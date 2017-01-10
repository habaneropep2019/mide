
&::PukeSendMessage($::PUKE_WIDGET_LOAD,
                     $::PUKE_CONTROLLER,
                     $::PWIDGET_LISTBOX,
                     "plistbox.so",
                     sub { my %ARG = %{shift()};
                     if($ARG{'iArg'} == 1){
                       print "*E* PListBox Load failed!\n";
                     }
                     }
                  );


package PListBox; #FOLD00
@ISA = qw(PTableView);
use strict;

if($PListBox::usage == undef){
    $PListBox::usage = 0;
}

sub new { #FOLD00
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_LISTBOX;

  if($class eq 'PListBox'){
    $self->create();
  }

  $self->{count} = 0;
  $self->{items} = ();

  $self->installHandler($::PUKE_LISTBOX_SELECTED_ACK, sub{$self->selected(@_)});

  return $self;

}

sub DESTROY { #FOLD00
    my $self = shift;
    $self->SUPER::DESTROY(@_);
    $PListBox::usage--;
    if($PListBox::usage == 0){
        &::PukeSendMessage($::PUKE_WIDGET_UNLOAD,
                           0,
                           $::PWIDGET_LISTBOX,
                           "",
                           sub {}
                          );

    }
}

sub insertText { #FOLD00
  my $self = shift;
  
  my $text = shift;
  my $index = shift;
  my $rindex = $index;
  
  if($index < 0 || $index >= $self->{count}){
    $rindex = $self->{count};
  }

  $self->{count} ++;

  # Don't need the ouput since GET_TEXT_ACK will be called and
  # we'll set it there
  $self->sendMessage('iCommand' => $::PUKE_LISTBOX_INSERT,
                     'iArg' => $rindex,
		     'cArg' => $text,
                     'CallBack' => sub {});

}

sub text { #FOLD00
  my $self = shift;
  my $index = shift;

  my %arg = $self->sendMessage('iCommand' => $::PUKE_LISTBOX_GETTEXT,
			       'iArg' => $index,
			       'WaitFor' => 1);

  if($arg{'iArg'} != 1){
    return undef;
  }
  $arg{'cArg'} =~ s/\000//g;
  return $arg{'cArg'};
}

sub insertPixmap { #FOLD00
  my $self = shift;
  
  my $file = shift;
  my $index = shift;
  my $rindex = $index;
  
  if($index < 0 || $index >= $self->{count}){
    $rindex = $self->{count};
  }
  #  $self->{items}->[$rindex] = "***PIXMAP***" . $file;
  $self->{count} ++;


  # Don't need the ouput since GET_TEXT_ACK will be called and
  # we'll set it there
  $self->sendMessage('iCommand' => $::PUKE_LISTBOX_INSERT_PIXMAP,
                     'iArg' => $rindex,
		     'cArg' => $file,
                     'CallBack' => sub {});

}
sub selected {
  my $self = shift;
  my %ARGS = %{shift()};

  $self->{current} = $ARGS{'iArg'};
  $ARGS{'cArg'} =~ s/\000//g;
  $self->{currentText} = $ARGS{'cArg'};
}

sub current {
    my $self = shift;
    return   $self->{current};
}

sub currentText {
    my $self = shift;
    return $self->text($self->{current});
}

sub setCurrentItem {
  my $self = shift;

  my $index = shift;
  my $rindex = $index;

  # Async call be default, no need to wait result
  $self->sendMessage('iCommand' => $::PUKE_LISTBOX_HIGHLIGHT,
                     'iArg' => $index,
                     'CallBack' => sub {});


}

sub removeItem {
  my $self = shift;

  my $index = shift;

  $self->{count} --;

  # Async call be default, no need to wait result
  $self->sendMessage('iCommand' => $::PUKE_LISTBOX_REMOVE,
                     'iArg' => $index,
                     'CallBack' => sub {});


}

sub setScrollBar {
  my $self = shift;

  $self->sendMessage('iCommand' => $::PUKE_LISTBOX_SET_SCROLLBAR,
                     'iArg' => shift(),
                     'CallBack' => sub {});

}

sub setAutoScrollBar {
  my $self = shift;

  $self->sendMessage('iCommand' => $::PUKE_LISTBOX_SET_AUTO_SCROLLBAR,
                     'iArg' => shift(),
                     'CallBack' => sub {});

}

sub clear {
  my $self = shift;

  $self->{count} = 0;
  $self->{items} = ();
  
  $self->sendMessage('iCommand' => $::PUKE_LISTBOX_CLEAR,
                     'CallBack' => sub {});

}



package main; #FOLD00

1;
