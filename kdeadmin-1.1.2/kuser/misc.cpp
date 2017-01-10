// $Id: misc.cpp,v 1.15.2.2 1999/06/29 06:48:55 garbanzo Exp $

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "globals.h"

#include <unistd.h>

#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif

#ifdef HAVE_SYS_MNTENT_H
#include <sys/mntent.h>
#define BAD_GETMNTENT
#endif

#ifdef HAVE_SYS_MNTTAB_H
#include <sys/mnttab.h>
#endif

#include <kmsgbox.h>
#include "misc.h"
#include "globals.h"
#include "maindlg.h"

QString readentry(const QString &name, const QString def) {
  if (config->hasKey(name))
    return config->readEntry(name);
  else
    return def;
}

int readnumentry(const QString &name) {
  if (config->hasKey(name))
    return config->readNumEntry(name);
  else
    return (0);
}

void backup(char const *name) {
  char tmp[1024];

  sprintf(tmp, "%s%s", name, KU_BACKUP_EXT);
  unlink(tmp);

  if (rename(name, tmp) == -1) {
    sprintf(tmp, i18n("Can't create backup file for %s"), name);
    err->addMsg(tmp, STOP);
    err->display();
    return;
  }
}

char *convertdate(char *buf, const long int base, const long int days) {
  time_t tmpd;

  tm *t;

  if ((base+days)<24855l)
    if (days >0) {
      tmpd = (base+days)*24*60*60;
      t = localtime(&tmpd);

      sprintf(buf, "%d %d %d", t->tm_mday, t->tm_mon+1, t->tm_year+1900);
    }
    else {
      strcpy(buf, "Nothing");
    }
  else {
    strcpy(buf, "Never");
  }

  return buf;
}

long today() {
  return (time(NULL)/(24*60*60));
}

QLabel *addLabel(QWidget *parent, const char *name, int x, int y, int w, int h, const char *text) {

  QLabel *tmpLabel = new QLabel(parent, name);
  tmpLabel->setGeometry(x, y, w, h);
  tmpLabel->setAutoResize(TRUE);
  tmpLabel->setText(text);

  return (tmpLabel);
}

QLineEdit *addLineEdit(QWidget *parent, const char *name, int x, int y, int w, int h, const char *text) {

  QLineEdit *tmpLE = new QLineEdit(parent, name);
  tmpLE->setGeometry(x, y, w, h);
  tmpLE->setText(text);

  return (tmpLE);
}

char *updateString(char *d, const char *t) {
  free(d);
  d = (char *)malloc(strlen(t)+1);  
  strcpy(d,t);
  return d;
}

int getValue(long int &data, const char *text, const char *msg) {
  char *check;
  long int value = strtol(text, &check, 0);
  if (check[0]) {
    err->addMsg(msg, STOP);
    err->display();
    return (-1);
  }
  data = value;

  return 0;
}

int getValue(int &data, const char *text, const char *msg) {
  char *check;
  long int value = strtol(text, &check, 0);
  if (check[0]) {
    err->addMsg(msg, STOP);
    err->display();
    return (-1);
  }
  data = (int)value;

  return 0;
}

int getValue(unsigned int &data, const char *text, const char *msg) {
  char *check;
  long int value = strtol(text, &check, 0);
  if (check[0]) {
    err->addMsg(msg, STOP);
    err->display();
    return (-1);
  }
  data = (unsigned int)value;

  return 0;
}

int copyFile(QString from, QString to) {
  QFile fi;
  QFile fo;
  char buf[4096];

#ifdef _KU_DEBUG
  printf("%s -> %s\n", (const char *)from, (const char *)to);
#endif
  
  fi.setName(from);
  fo.setName(to);
  
  if (!fi.exists()) {
    QString tmp;
    
    tmp.sprintf(i18n("File %s does not exist."), (const char *)from);
    err->addMsg(tmp, STOP);
    return (-1);
  }

  if (!fi.open(IO_ReadOnly)) {
    QString tmp;
    
    tmp.sprintf(i18n("Can not open file %s for reading."), (const char *)from);
    err->addMsg(tmp, STOP);
    return (-1);
  }
    
  if (!fo.open(IO_Raw | IO_WriteOnly | IO_Truncate)) {
    QString tmp;
    
    tmp.sprintf(i18n("Can not open file %s for writing."), (const char *)to);
    err->addMsg(tmp, STOP);
    return (-1);
  }
  
  while (!fi.atEnd()) {
    int len = fi.readBlock(buf, 4096);
    fo.writeBlock(buf, len);
  }
  
  fi.close();
  fo.close();
    
  return 0;
}
