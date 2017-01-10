#include <kapp.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <qtimer.h>
#include <kwm.h>
#include <qpainter.h>
#include <stdlib.h>
#include <time.h>
#include "docked.h"

#define PROCFILE "/proc/loadavg"
#define PROCFILE1 "/proc/stat"

#define M_OPEN_KPM 1
#define M_EXIT 1000


QString FQFN(const char *fn) {
  QString s = fn;

  // check for full path
  if(s[0] == '/')
    return s;

  // check path
  char buf[1024];
  if(getenv("PATH")) {
    strcpy(buf, getenv("PATH"));
    char *p = strtok(buf, ":");
    while(p) {
      s = p;
      s.append("/");
      s.append(fn);
      if(access(s.data(), X_OK) == 0) {
	if(s.left(2) == "./") {
	  char pathbuf[1024];
	  getcwd(pathbuf, sizeof(pathbuf));
	  s = pathbuf;
	  s.append("/");
	  s.append(fn);
	}
	return s;
      }
      p = strtok(NULL, ":");
    }
  }

  s = fn;
  return s;
}


Sysload::Sysload() : KTMainWindow() {
  conf = kapp->getConfig();

  clearHistory();
  QTimer *t = new QTimer(this);
  connect(t, SIGNAL(timeout()),
	  this, SLOT(doUpdate()));
  t->start(5000);
  resize(ICONWIDTH, ICONHEIGHT);
  KWM::setDockWindow(winId());
  show();
  doUpdate();
  proc << "kpm";

  // cpu load
  idle = -1;
  QTimer *t1 = new QTimer(this);
  connect(t1, SIGNAL(timeout()),
	  this, SLOT(updateCPULoad()));
  t1->start(1000);

  // make popup menu
  menu = new QPopupMenu;
  options = new QPopupMenu;
  options->setCheckable(true);
  int id;
  id = options->insertItem(i18n("Show CPU load"), this, SLOT(showCPULoad()));
  id = options->insertItem(i18n("Show system load"), this, SLOT(showSysLoad()));
  menu->insertItem(i18n("Open system monitor"), M_OPEN_KPM);
  menu->insertItem(i18n("Options"), options);
  menu->insertSeparator();
  menu->insertItem(i18n("Exit"), M_EXIT);

  connect(menu, SIGNAL(activated(int)),
	  this, SLOT(menuCallback(int)));

  qh = new KQuickHelpWindow();
}


void Sysload::showCPULoad() {
    conf->writeEntry("ShowCPULoad", 1);
    conf->writeEntry("ShowSysLoad", 0);
    conf->sync();
    clearHistory();
    doUpdate();
}


void Sysload::showSysLoad() {
    conf->writeEntry("ShowCPULoad", 0);
    conf->writeEntry("ShowSysLoad", 1);
    conf->sync();
    clearHistory();
    doUpdate();
}


void Sysload::mousePressEvent(QMouseEvent *e) {
  if(e->button() == LeftButton)
    toggleKPM();
  else if(e->button() == RightButton) {
    QPoint p;
    p = mapToGlobal(e->pos());
    popupMenu(p);
  } else if(e->button() == MidButton) {
    QPoint p;
    p = mapToGlobal(e->pos());
    QString s(2048);
    s.sprintf("<+><bold>Load average</b><-><br><i+>Last minute: %0.2f"
	      "<br>Last 5 minutes: %0.2f<br>Last 15 minutes: %0.2f<i->"
	      "<br><br><+><b>CPU load:</b> %0.0f%%<->",
	      av1, av2, av3, 100.0*cpuload <= 100 ? 100*cpuload : 100);

    qh->popup(s.data(), p.x(), p.y());
  }
}


void Sysload::clearHistory() {
  idx = ICONWIDTH-1;
  for(int i = 0; i < ICONWIDTH; i++)
    history[i] = -1;
}


void Sysload::doUpdate() {
  sysload(&av1, &av2, &av3);

  idx = (idx+1) % ICONWIDTH;

  if(conf->readNumEntry("ShowSysLoad", 1))
    history[idx] = av1;
  else if(conf->readNumEntry("ShowCPULoad", 0))
    history[idx] = cpuload;

  // find highest load
  float max_load = history[0];
  for(int i = 1; i < idx; i++)
    if(max_load < history[i])
      max_load = history[i];

  // round toward nearest integer
  if(max_load < 1)
    max_load = 1;
  else
    max_load = int(max_load + 0.999999);

  QPixmap pm(ICONWIDTH, ICONHEIGHT);
  pm.fill(backgroundColor());
  QPainter p;
  p.begin(&pm);

  // draw baseline
  p.setPen(QPen(black, 1, DotLine));
  p.drawLine(0, ICONHEIGHT-1, ICONWIDTH, ICONHEIGHT-1);

  // draw graph
  p.setPen(red);
  int midx = idx;
  for(int i = 0; i < ICONWIDTH; i++) {
    midx = (midx+1) % ICONWIDTH;
    p.drawLine(i, ICONHEIGHT, i, ICONHEIGHT-(history[midx] / max_load) * ICONHEIGHT +1);
  }

  if(conf->readNumEntry("ShowSysLoad", 1)) {
    // draw scale
    p.setPen(black);
    for(int j = 1; j < (int)max_load; j++)
      p.drawLine(0, int(j / max_load * ICONHEIGHT), ICONWIDTH, int((j / max_load) * ICONHEIGHT));
  }

  p.end();
  setBackgroundPixmap(pm);
}


bool Sysload::sysload(float *av1, float *av2, float *av3) {
  FILE *f = fopen(PROCFILE, "r");

  if(f) {
    char buf[128];
    fgets(buf, sizeof(buf), f);
    fclose(f);
    buf[sizeof(buf)-1] = 0;
    sscanf(buf, "%f %f %f", av1, av2, av3);
    return true;
  } else
    return false;
}


void Sysload::updateCPULoad() {
  FILE *f = fopen(PROCFILE1, "r");
  if(f) {
    char buf[128];
    int newidle;

    fgets(buf, sizeof(buf), f);
    fclose(f);
    buf[sizeof(buf)-1] = 0;
    sscanf(buf, "%*s %*d %*d %*d %d", &newidle);
    if(idle == -1)
      cpuload = 0;
    else
      cpuload = (float)(100-(newidle-idle))/100.0;
    idle = newidle;
  } else
    cpuload = 0;
}


void Sysload::menuCallback(int id) {
  switch(id) {
  case M_EXIT:
    kapp->quit();
    break;
  case M_OPEN_KPM:
    toggleKPM();
    break;
  }
}


void Sysload::popupMenu(QPoint p) {
  if(proc.isRunning())
    menu->changeItem(i18n("Close system monitor"), M_OPEN_KPM);
  else
    menu->changeItem(i18n("Open system monitor"), M_OPEN_KPM);

  options->setItemChecked(-2, conf->readNumEntry("ShowCPULoad", 0));
  options->setItemChecked(-3, conf->readNumEntry("ShowSysLoad", 1));
  menu->popup(p);
}


void Sysload::toggleKPM() {
  if(!proc.isRunning()) {
    proc.clearArguments();
    proc << "kpm";
    proc.start();
  }  else
    proc.kill();
}

int main(int argc, char **argv) {
  sleep(5);
  KApplication a(argc, argv, "kpmdocked");

#ifdef linux
  if(access(PROCFILE, R_OK) != 0) {
    QMessageBox::warning(0,
			 i18n("Error"),
			 i18n("This system lacks /proc filesystem support!\n"
			      "This support is needed for proper operation,\n"
			      "so you should consider recompiling your kernel\n"
			      "with the /proc filesystem option enabled."));
    return 1;
  }

  Sysload sl;

  kapp->setTopWidget(new QWidget);
  kapp->enableSessionManagement(true);
  kapp->setWmCommand(FQFN(argv[0]));

#ifdef DEBUGGING
  FILE *f = fopen("/home/mario/x.log", "a");
  time_t t;
  time(&t);
  struct tm *tm = localtime(&t);
  fprintf(f, "\n\n%s\tSTARTED AS %s, FQ=%s\n", asctime(tm), argv[0], FQFN(argv[0]).data());
  fclose(f);
#endif

  return a.exec();
#else
    QMessageBox::warning(0,
			 i18n("Error"),
			 i18n("This program requires a Linux OS, sorry!"));
    reurn 1;
#endif
}

#include "docked.moc"
