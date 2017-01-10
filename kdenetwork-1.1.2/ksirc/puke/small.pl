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
&docommand("/load plabel.pm");

$pw = new PWidget;
$pw->resize(250,500);

$pf = new PFrame($pw);
$pf->setFrameStyle($PFrame::Panel|$PFrame::Sunken, 1);
$pf->move(50,50);
$pf->resize(150,400);
$pf->setMinimumSize(50,50);

$pf2 = new PFrame($pw);
$pf2->setFrameStyle($PFrame::Panel|$PFrame::Raised, 1);
$pf2->setMinimumSize(50,50);


$pl = new PListBox($pw);
$pl->move(50,50);
$pl->resize(300, 50);
$pl->setMinimumSize(50,50);

$label = new PLabel($pw);
$label->setText("Label");
$label->setMinimumSize(50,50);

$pb = new PBoxLayout($pw, $PBoxLayout::TopToBottom, 5);
$pb2 = new PBoxLayout($PBoxLayout::LeftToRight, 5);
$pb3 = new PBoxLayout($PBoxLayout::TopToBottom, 5);
$pb->addLayout($pb2);
$pb->addLayout($pb3);
$pb->addWidget($pf, 1, $PBoxLayout::AlignCenter);
$pb2->addWidget($pf2, 10, $PBoxLayout::AlignCenter);
$pb2->addWidget($pl, 10, $PBoxLayout::AlignCenter);
$pb2->addWidget($label, 10, $PBoxLayout::AlignCenter);

$pb4 = new PBoxLayout($PBoxLayout::RightToLeft, 1);
$pb3->addLayout($pb4);

$pb4->addWidget($pf2, 0, $PBoxLayout::AlignCenter);

$pbutton = new PPushButton($pw);
$pbutton->setText("Hello");
#$pbutton->setPixmap("/opt/kde/share/icons/ksirc.gif");
$pbutton->setMaximumSize(50,50);
$pbutton->setMinimumSize(50,50);
$pb->addWidget($pbutton, 10, $PBoxLayout::AlignCenter);

$pline = new PLineEdit($pw);
$pline->setMinimumSize(50, 30);
$pline->resize(200, 20);
$pline->setMaximumSize(1000, 30);
$pline->setText("Hello!");
$pb->addWidget($pline, 0);

$pw->show;