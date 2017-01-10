// -*- c++ -*-

#ifndef APP_H
#define APP_H

#include <kapp.h>
#include <qstring.h>
#include <qpixmap.h>
#include <kfm.h>

class MyApp : public KApplication {
  Q_OBJECT

public:
  MyApp( int &argc, char **argv, const QString );
  static void usage();

  KFM *kfm;
  QPixmap *clipboard_;
};

#define myapp ((MyApp *) MyApp::getKApplication())

#endif // APP_H


