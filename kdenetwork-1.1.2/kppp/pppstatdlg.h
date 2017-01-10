/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: pppstatdlg.h,v 1.6 1998/09/20 12:48:50 mario Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _PPPSTATSDLG_H_
#define _PPPSTATSDLG_H_


#include <qpushbt.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qlined.h>
#include <qtimer.h>

const int MAX_GRAPH_WIDTH   = 600;
const int GRAPH_UPDATE_TIME = 1000;

class PPPStatsDlg : public QWidget {

  Q_OBJECT

public:

  PPPStatsDlg(QWidget *parent=0, const char *name=0,QWidget *main=0);
  ~PPPStatsDlg();

  enum {PIXLEFT, PIXNONE,PIXRIGHT,PIXBOTH, PIXINIT};

protected:

  void closeEvent( QCloseEvent *e );
  void paintEvent (QPaintEvent *e) ;

public slots:

  void timeclick();
  void cancel();
  void take_stats();
  void stop_stats();
  void paintIcon();
  void updateGraph();

public:

  void startClock();
  void update_data(bool data_available);

private:
  void paintGraph();

  QLabel *pixmap_l;
  QWidget *main;
  QPushButton *cancelbutton;
  QTimer *clocktimer;
  bool left;
  int pixstate;

  QLabel *labela1[5];
  QLabel *labela2[5];
  QLabel *labelb1[5];
  QLabel *labelb2[5];

  QLabel *ip_address_label1;
  QLineEdit *ip_address_label2;
  QLabel *ip_address_label3;
  QLineEdit *ip_address_label4;

  QLabel *modem_pic_label;
  QPixmap modem_pixmap;
  QPixmap modem_left_pixmap;
  QPixmap modem_right_pixmap;
  QPixmap modem_both_pixmap;
  QPixmap big_modem_both_pixmap;
  QPixmap big_modem_left_pixmap;
  QPixmap big_modem_right_pixmap;
  QPixmap big_modem_none_pixmap;

  bool ips_set; /* are the ip linedits filled in already?*/
  int ibytes_last;
  int obytes_last;
  bool need_to_paint;

  QString 	ibytes_string;
  QString 	ipackets_string;
  QString 	compressedin_string;
  QString 	uncompressedin_string;
  QString 	errorin_string;
  QString 	obytes_string;
  QString	opackets_string;
  QString	compressed_string;
  QString 	packetsunc_string;
  QString 	packetsoutunc_string;
  QGroupBox    *box;

  // graph widget
  QFrame *graph;
  int bin[MAX_GRAPH_WIDTH];
  int bout[MAX_GRAPH_WIDTH];
  int bin_last;
  int bout_last;
  int ringIdx;
  int max;
  QTimer *graphTimer;
  QColor bg, text, in, out;
};

#endif
