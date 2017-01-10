/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: pppstatdlg.cpp,v 1.19.2.1 1999/06/09 20:00:47 porten Exp $
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


#include <kmsgbox.h>
#include <qlayout.h>
#include <qmsgbox.h>
#include <kapp.h>
#include <kwm.h>

#include "macros.h"
#include "pppdata.h"
#include "pppstatdlg.h"
#include "kpppconfig.h"
#include "iplined.h"

extern PPPData gpppdata;
extern bool do_stats();
extern bool init_stats();

extern int 	ibytes;
extern int 	ipackets;
extern int 	compressedin;
extern int 	uncompressedin;
extern int 	errorin;
extern int 	obytes;
extern int	opackets;
extern int 	compressed;
extern int 	packetsunc;
extern int 	packetsoutunc;
extern QString  local_ip_address;
extern QString  remote_ip_address;
extern int      totalbytes; // main.cpp


PPPStatsDlg::PPPStatsDlg(QWidget *parent, const char *name, QWidget *)
  : QWidget(parent, name, 0 )
{
  int i;
  max = 1024;

  pixstate = PIXINIT;
  need_to_paint = true;
  setCaption(i18n("kppp Statistics"));
  KWM::setMiniIcon(winId(), kapp->getMiniIcon());

  QVBoxLayout *tl = new QVBoxLayout(this, 10);
  QGridLayout *l1 = new QGridLayout(4, 4);
  tl->addLayout(l1, 1);
  box = new QGroupBox(i18n("Statistics"), this);
  l1->addMultiCellWidget(box, 0, 3, 0, 3);
  l1->addRowSpacing(0, fontMetrics().lineSpacing() - 10);
  l1->setRowStretch(1, 1);
  l1->setColStretch(1, 1);

  // inner part of the grid
  QVBoxLayout *l11 = new QVBoxLayout;
  l1->addLayout(l11, 1, 1);

  // modem pixmap and IP labels
  QHBoxLayout *l111 = new QHBoxLayout;
  l11->addLayout(l111);

  QString pixdir = KApplication::kde_datadir() + "/kppp/pics/";
  QString tmp;

#define PMERROR(pm) \
  { tmp.sprintf(i18n("Could not load %s !"), pm); \
  QMessageBox::warning(this, i18n("Error"), tmp); \
  }

  if ( !big_modem_both_pixmap.load(pixdir + "modemboth.xpm") )
    PMERROR("modemboth.xpm");

  if ( !big_modem_left_pixmap.load(pixdir + "modemleft.xpm") )
    PMERROR("modemleft.xpm");

  if ( !big_modem_right_pixmap.load(pixdir + "modemright.xpm") )
    PMERROR("modemright.xpm"); 

  if ( !big_modem_none_pixmap.load(pixdir + "modemnone.xpm") )
    PMERROR("modemnone.xpm");
  

  pixmap_l = new QLabel(this);
  pixmap_l->setMinimumSize(big_modem_both_pixmap.size());
  l111->addWidget(pixmap_l, 1);
  pixmap_l->setAlignment(AlignVCenter|AlignLeft);

  QGridLayout *l1112 = new QGridLayout(3, 2);
  l111->addLayout(l1112);

  ip_address_label1 = new QLabel(this);
  ip_address_label1->setText(i18n("Local Addr:"));

  ip_address_label2 = new IPLineEdit(this);
  ip_address_label2->setFocusPolicy(QWidget::NoFocus);

  ip_address_label3 = new QLabel(this);
  ip_address_label3->setText(i18n("Remote Addr:"));

  ip_address_label4 = new IPLineEdit(this);
  ip_address_label4->setFocusPolicy(QWidget::NoFocus);

  MIN_SIZE(ip_address_label1);
  MIN_SIZE(ip_address_label2);
  MIN_SIZE(ip_address_label3);
  MIN_SIZE(ip_address_label4);

  l1112->addWidget(ip_address_label1, 0, 0);
  l1112->addWidget(ip_address_label2, 0, 1);
  l1112->addWidget(ip_address_label3, 1, 0);
  l1112->addWidget(ip_address_label4, 1, 1);

  // consumes space on bottom
  l1112->setRowStretch(2, 1);

  QGridLayout *l112 = new QGridLayout(5, 4);
  l11->addLayout(l112);
  for(i =0 ; i < 5; i++) {
    labela1[i] = new QLabel(this);

    labela2[i] = new QLabel(this);
    labela2[i]->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

    labelb1[i] = new QLabel(this);

    labelb2[i] = new QLabel(this);
    labelb2[i]->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
 }

  labela1[0]->setText(i18n("bytes in"));
  labelb1[0]->setText(i18n("bytes out"));

  labela1[1]->setText(i18n("packets in"));
  labelb1[1]->setText(i18n("packets out"));

  labela1[2]->setText(i18n("vjcomp in"));
  labelb1[2]->setText(i18n("vjcomp out"));

  labela1[3]->setText(i18n("vjunc in"));
  labelb1[3]->setText(i18n("vjunc out"));

  labela1[4]->setText(i18n("vjerr"));
  labelb1[4]->setText(i18n("non-vj"));

  for(i = 0; i < 5; i++) {
    labela2[i]->setText("88888888");
    labelb2[i]->setText("88888888");
    MIN_SIZE(labela1[i]);
    MIN_SIZE(labela2[i]);
    MIN_SIZE(labelb1[i]);
    MIN_SIZE(labelb2[i]);
    labela2[i]->setText("");
    labelb2[i]->setText("");

    // add to layout
    l112->addWidget(labela1[i], i, 0);
    l112->addWidget(labela2[i], i, 1);
    l112->addWidget(labelb1[i], i, 2);
    l112->addWidget(labelb2[i], i, 3);
  }

  l112->setColStretch(1, 1);
  l112->setColStretch(3, 1);

  tl->addSpacing(5);
  QHBoxLayout *l12 = new QHBoxLayout;
  tl->addLayout(l12);
  l12->addStretch(1);

  if(gpppdata.graphingEnabled()) {
    bool dummy;

    gpppdata.graphingOptions(dummy, bg, text, in, out);

    graph = new QFrame(this);
    graph->setFrameStyle(QFrame::Box | QFrame::Sunken);
    l1->addMultiCellWidget(graph, 2, 2, 1, 2);
    graph->setMinimumWidth(300);
    graph->setFixedHeight(76+4);
    graph->setBackgroundColor(bg);
  }

  cancelbutton = new QPushButton(this, "cancelbutton");
  cancelbutton->setText(i18n("Close"));
  cancelbutton->setFocus();
  connect(cancelbutton, SIGNAL(clicked()), this,SLOT(cancel()));
  FIXED_HEIGHT(cancelbutton);
  cancelbutton->setMinimumWidth(QMAX(cancelbutton->sizeHint().width(), 70));
  l12->addWidget(cancelbutton);

  clocktimer = new QTimer(this);
  connect(clocktimer, SIGNAL(timeout()), SLOT(timeclick()));

  if(gpppdata.graphingEnabled()) {
    graphTimer = new QTimer(this);
    connect(graphTimer, SIGNAL(timeout()), SLOT(updateGraph()));
  }

  tl->freeze();
}


PPPStatsDlg::~PPPStatsDlg() {
  clocktimer->stop();
}


void PPPStatsDlg::cancel() {
  hide();
}


void PPPStatsDlg::take_stats() {
  init_stats();
  ips_set = false;
  bin_last = ibytes;
  bout_last = obytes;  
  ringIdx = 0;
  for(int i = 0; i < MAX_GRAPH_WIDTH; i++) {
    bin[i] = -1;
    bout[i] = -1;
  }
  clocktimer->start(PPP_STATS_INTERVAL);

  if(gpppdata.graphingEnabled())
    graphTimer->start(GRAPH_UPDATE_TIME);  
}


void PPPStatsDlg::stop_stats() {
  clocktimer->stop();
  if(gpppdata.graphingEnabled())
    graphTimer->stop();
}

void PPPStatsDlg::paintGraph() {
  // why draw that stuff if not visible?
  if(!isVisible())
    return;

  QPixmap pm(graph->width() - 4, graph->height() - 4);
  QPainter p;
  pm.fill(graph->backgroundColor());
  p.begin(&pm);

  int x;
  int idx = ringIdx - pm.width() + 1;
  if(idx < 0)
    idx += MAX_GRAPH_WIDTH;
    
  // find good scaling factor    
  int last_h_in = 
    pm.height() - (int)((float)bin[idx]/max * (pm.height() - 8))-1;
  int last_h_out = 
    pm.height() - (int)((float)bout[idx]/max * (pm.height() - 8))-1;
  
  // plot scale line
  p.setPen(text);
  p.setFont(QFont("fixed", 8));
  QRect r;
  QString s;
  s.sprintf(i18n("%d kb/s"), max/1024);
  p.drawText(0, 0, pm.width(), 2*8, AlignRight|AlignVCenter, s.data(), -1, &r);
  p.drawLine(0, 8, r.left() - 8, 8);

  // plot data
  for(x = 1; x < pm.width(); x++) {
    int h_in, h_out;
    
    h_in = pm.height() - (int)((float)bin[idx]/max * (pm.height() - 8))-1;
    h_out = pm.height() - (int)((float)bout[idx]/max * (pm.height() - 8))-1;
    
    p.setPen(out);    
    if(bout[idx]!=-1)
      p.drawLine(x-1, last_h_out, x, h_out);
    p.setPen(in);
    if(bin[idx]!=-1)
      p.drawLine(x-1, last_h_in, x, h_in);
    last_h_in = h_in;
    last_h_out = h_out;

    idx = (idx + 1) % MAX_GRAPH_WIDTH;
  }

  p.end();
  bitBlt(graph, 2, 2, &pm, 0, 0, pm.width(), pm.height(), CopyROP);
}

void PPPStatsDlg::updateGraph() {
  bin[ringIdx] = ibytes - bin_last;
  bout[ringIdx] = obytes - bout_last;
  if(bin[ringIdx] > max)
    max = ((bin[ringIdx] / 1024) + 1) * 1024;
 
 if(bout[ringIdx] > max)
    max = ((bout[ringIdx] / 1024) + 1) * 1024;
 
  bin_last = ibytes;
  bout_last = obytes;
  ringIdx = (ringIdx + 1) % MAX_GRAPH_WIDTH;
  paintGraph();
}


void PPPStatsDlg::paintEvent (QPaintEvent *) {
  need_to_paint = true;
  paintIcon();
  if(gpppdata.graphingEnabled())
    paintGraph();
}


void PPPStatsDlg::paintIcon() {

    if((ibytes_last != ibytes) && (obytes_last != obytes)) {
      bitBlt( pixmap_l, 0, 0, &big_modem_both_pixmap );
      ibytes_last = ibytes;
      obytes_last = obytes;
      pixstate = PIXBOTH;
      return;
    }

    if (ibytes_last != ibytes) {
      bitBlt( pixmap_l, 0, 0, &big_modem_left_pixmap );
      ibytes_last = ibytes;
      obytes_last = obytes;
      pixstate = PIXLEFT;
      return;
    }

    if(obytes_last != obytes) {
      bitBlt( pixmap_l, 0, 0, &big_modem_right_pixmap );
      ibytes_last = ibytes;
      obytes_last = obytes;
      pixstate = PIXRIGHT;
      return;
    }

    bitBlt( pixmap_l,0,0, &big_modem_none_pixmap );
    ibytes_last = ibytes;
    obytes_last = obytes;
    pixstate = PIXNONE;
}


void PPPStatsDlg::timeclick() {
  // volume accounting
  switch(gpppdata.VolAcctEnabled()) {
  case 0: // no accounting
    break;

  case 1: // bytes in
    totalbytes = gpppdata.totalBytes() + ibytes;
    break;

  case 2:
    totalbytes = gpppdata.totalBytes() + obytes;
    break;

  case 3:
    totalbytes = gpppdata.totalBytes() + ibytes + obytes;
    break;
  }

  update_data(do_stats());
  if( isVisible()) {
    paintIcon();
  }
}


void PPPStatsDlg::closeEvent( QCloseEvent *e ) {
  e->ignore();
}


void PPPStatsDlg::update_data(bool) {
  ibytes_string.sprintf("%d",ibytes);
  ipackets_string.sprintf("%d",ipackets);
  compressedin_string.sprintf("%d",compressedin);
  uncompressedin_string.sprintf("%d",uncompressedin);
  errorin_string.sprintf("%d",errorin);
  obytes_string.sprintf("%d",obytes);
  opackets_string.sprintf("%d",opackets);
  compressed_string.sprintf("%d",compressed);
  packetsunc_string.sprintf("%d",packetsunc);
  packetsoutunc_string.sprintf("%d",packetsoutunc);

  labela2[0]->setText(ibytes_string);
  labela2[1]->setText(ipackets_string);
  labela2[2]->setText(compressedin_string);
  labela2[3]->setText(uncompressedin_string);
  labela2[4]->setText(errorin_string);

  labelb2[0]->setText(obytes_string);
  labelb2[1]->setText(opackets_string);
  labelb2[2]->setText(compressed_string);
  labelb2[3]->setText(packetsunc_string);
  labelb2[4]->setText(packetsoutunc_string);

  if(!ips_set) {
    // if I don't resort to this trick it is imposible to
    // copy/paste the ip out of the lineedits due to
    // reset of cursor position on setText()

    if( !local_ip_address.isEmpty() )
      ip_address_label2->setText(local_ip_address);
    else
      ip_address_label2->setText(i18n("unavailable"));

    if( !remote_ip_address.isEmpty() )
      ip_address_label4->setText(remote_ip_address);
    else
      ip_address_label4->setText(i18n("unavailable"));
    
    ips_set = true;
  }
}

#include "pppstatdlg.moc"

