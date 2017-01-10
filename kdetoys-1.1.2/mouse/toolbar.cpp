
    KIconLoader *loader = a.getIconLoader();
    QPixmap pixmap;


    KToolBar* toolbar = new KToolBar(&w);    

    pixmap = loader->loadIcon("filenew2.xpm");
    toolbar->insertButton(pixmap, 0,
			  SIGNAL(clicked()), &o,
			  SLOT(help()), TRUE, klocale->translate("New Document"));
    
    
    pixmap = loader->loadIcon("fileopen.xpm");
    toolbar->insertButton(pixmap, 0,
			  SIGNAL(clicked()), &o,
			  SLOT(help()), TRUE, klocale->translate("Open Document"));

    toolbar->setMaxWidth(100);
    w.addToolBar(toolbar);


    toolbar = new KToolBar(&w);    

    pixmap = loader->loadIcon("filenew2.xpm");
    toolbar->insertButton(pixmap, 0,
			  SIGNAL(clicked()), &o,
			  SLOT(help()), TRUE, klocale->translate("gash"));
    
    
    pixmap = loader->loadIcon("fileopen.xpm");
    toolbar->insertButton(pixmap, 0,
			  SIGNAL(clicked()), &o,
			  SLOT(help()), TRUE, klocale->translate("Goosh"));

    toolbar->setMaxWidth(150);
    w.addToolBar(toolbar);
    
