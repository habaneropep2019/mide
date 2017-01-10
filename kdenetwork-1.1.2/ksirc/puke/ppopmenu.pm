
&::PukeSendMessage($PUKE_WIDGET_LOAD, 
		   $PUKE_CONTROLLER, 
		   $PWIDGET_POPMENU,
		   "ppopmenu.so",
		   sub { my %ARG = %{shift()};
			 if($ARG{'iArg'} == 1){
			   print "*E* PPopMenu Load failed!\n";
			 }
		       }
		  );

package PPopupMenu;
@ISA = qw(PTableView PMenuData);
use strict;

sub new {
  my $class = shift;
  my $self = $class->SUPER::new($class, @_);

  $self->{widgetType} = $::PWIDGET_POPMENU;

  if($class eq 'PPopupMenu'){
    $self->create();
  }

  $self->installHandler($::PUKE_POPUPMENU_ACTIVATED_ACK,
                          sub {$self->activated(@_)});

  return $self;

}

sub activated {
  my $self = shift;

  my %ARG = %{shift()};

  if($self->{'menu_id'}{$ARG{'iArg'}}){
    &{$self->{'menu_id'}{$ARG{'iArg'}}}(%ARG);
  }
  else {
    # There's not handler, don't make noise, since if we used fetchWidget()
    # We don't handlers for everything
    # &::tell("*E* No handler for id: $ARG{iArg}\n");
  }
}

sub installMenu {
  my $self = shift;

  my $id = shift;
  my $func = shift;
  $self->{'menu_id'}{$id} = $func;
}

sub popupAtCurrent {
  my $self = shift;

  $self->sendMessage('iCommand' => $::PUKE_POPUPMENU_POPUP_CURRENT,
		     'CallBack' => sub {});
}


package main;
