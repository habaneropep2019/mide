&docommand("/load pbase.pm");
&docommand("/load pwidget.pm");
&docommand("/load pframe.pm");
&docommand("/load pboxlayout.pm");
&docommand("/load plined.pm");
&docommand("/load pbutton.pm");
&docommand("/load ppushbt.pm");
&docommand("/load pprogress.pm");
&docommand("/load ptablevw.pm");
&docommand("/load plistbox.pm");

$pw = new PWidget;
$pw->resize(250,500);

$pf = new PFrame($pw);
$pf->setFrameStyle($PFrame::Panel|$PFrame::Sunken, 1);
$pf->move(50,50);
$pf->resize(150,400);

$pf2 = new PListBox($pw);
#$pf2->setFrameStyle($PFrame::Box|$PFrame::Raised, 1);
$pf2->move(50,50);
$pf2->resize(50,300);

$pb = new PBoxLayout($pw, $PBoxLayout::TopToBottom, 5);
$pb->addWidget($pf, 10); 
$pb->addWidget($pf2, 5);

$pb2 = new PBoxLayout($PBoxLayout::LeftToRight, 5);
$pb->addLayout($pb2, 0);

$pl = new PLineEdit($pw);
$pl->setMinimumSize(30,50);
$pl->setMaximumSize(30,1000);
$pl->setText("Blah blah");
#$pl->resize(50,75);
$pb2->addWidget($pl, 0, $PBoxLayout::AlignCenter);

$pp = new PPushButton($pw);
$pp->setMinimumSize(65,65);
$pp->setMaximumSize(65,65);
$pp->setPixmap("/opt/kde/share/icons/ksirc.gif");
$pp->installHandler($::PUKE_BUTTON_CLICKED_ACK, sub 
		    {
		      print "*I* Clocked!\n"; 
		      $pw->close;
		    }
		   );
$pb2->addWidget($pp, 0, $PBoxLayout::AlignRight);

for(my $i = 0; $i < 100; $i++){
  $pf2->insertPixmap("/opt/kde/share/icons/ksirc.gif", -1);
  $pf2->insertText("Test $i", -1);
}

$pp = new PProgress;

$pw->onNext(sub{$pw->show()});
$pp->onNext(sub{$pw->show(); $pp->show()});
#$pw->show();
#$pf->show();
#$pf2->show();

