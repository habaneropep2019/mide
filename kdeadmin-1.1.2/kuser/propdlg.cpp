#include <qvalidator.h>
#include <kmsgbox.h>
#include <kstring.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef __FreeBSD__
#include <qdatetm.h>
#endif
#include "globals.h"
#include "propdlg.h"
#include "propdlg.moc"
#include "pwddlg.h"
#include "maindlg.h"
#include "misc.h"
#include "kdatectl.h"


#ifdef _KU_QUOTA
propdlg::propdlg(KUser *auser, Quota *aquota, QWidget *parent, const char *name, int)
#else
propdlg::propdlg(KUser *auser, QWidget *parent, const char *name, int)
#endif
       :QDialog(parent, name, FALSE,
       WStyle_Customize | WStyle_DialogBorder | WStyle_SysMenu |
       WStyle_MinMax | WType_Modal) {
  user = auser;
  olduid = user->getUID();
#ifdef __FreeBSD__
  QDateTime *epoch = new QDateTime();
  QDateTime *temp_time = new QDateTime();
#endif

#ifdef _KU_QUOTA
  if (aquota == NULL)
    is_quota = 0;
  quota = aquota;
  chquota = 0;
#endif

  tw = new KTabCtl(this, "tw");
  tw->setGeometry(0, 0, 450, 410);

  pbok = new QPushButton(i18n("OK"), this, "pbok");
  pbok->setGeometry(200, 425, 100, 30);
  pbok->setDefault(TRUE);
  QObject::connect(pbok, SIGNAL(pressed()), this, SLOT(ok()));
  pbcancel = new QPushButton(i18n("Cancel"), this, "pbcancel");
  pbcancel->setGeometry(320, 425, 100, 30);
  QObject::connect(pbcancel, SIGNAL(pressed()), this, SLOT(cancel()));

  w1 = new QWidget(this, "wd_Password");

  leuser = addLabel(w1, "leuser", 10, 27, 150, 27, user->getName());
  QToolTip::add(leuser, i18n("User name"));
  l1 = addLabel(w1, "ml1", 10, 10, 50, 20, i18n("User login"));
  leid = addLineEdit(w1, "leid", 200, 30, 70, 22, "");
  QObject::connect(leid, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leid, i18n("User identificator"));
  l2 = addLabel(w1, "ml2", 200, 10, 50, 20, i18n("User id"));

  pbsetpwd = new QPushButton(w1, "pbsetpwd");
  pbsetpwd->setGeometry(260, 180, 120, 30);
  pbsetpwd->setText(i18n("Set password"));
  QObject::connect(pbsetpwd, SIGNAL(clicked()), this, SLOT(setpwd()));

  lefname = addLineEdit(w1, "lefname", 10, 80, 160, 22, "");
  QObject::connect(lefname, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(lefname, i18n("Full name"));
  l4 = addLabel(w1, "ml4", 10, 60, 50, 20, i18n("Full name"));

  leshell = new QComboBox(TRUE, w1, "leshell");
  QToolTip::add(leshell, i18n("Login shell"));
  leshell->clear();
  leshell->insertItem(i18n("<Empty>"));

  FILE *f = fopen(SHELL_FILE,"r");
  if (f) {
    while (!feof(f)) {
      char s[200];

      fgets(s, 200, f);
      if (feof(f))
        break;

      s[strlen(s)-1]=0;
      if ((s[0])&&(s[0]!='#'))
        leshell->insertItem(s);
    }
    fclose(f);
  }

  leshell->setGeometry(10, 122, 160, 27);
  QObject::connect(leshell, SIGNAL(activated(const char *)), this, SLOT(shactivated(const char *)));

  l5 = addLabel(w1, "ml5", 10, 105, 50, 20, i18n("Login shell"));

  lehome = addLineEdit(w1, "lehome", 10, 175, 160, 22, "");
  QObject::connect(lehome, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));

  QToolTip::add(lehome, i18n("Home directory"));
  l6 = addLabel(w1, "ml6", 10, 155, 50, 20, i18n("Home directory"));

#ifdef __FreeBSD__
  // FreeBSD appears to use the comma separated fields in the GECOS entry
  // differently than Linux.
  leoffice = addLineEdit(w1, "leoffice", 10, 220, 160, 22, "");
  QObject::connect(leoffice, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leoffice, i18n("Office"));
  l7 = addLabel(w1, "ml7", 10, 200, 50, 20, i18n("Office"));

  ld7 = addLabel(w1, "mld7", 190, 225, 50, 20, i18n("Office description"));

  leophone = addLineEdit(w1, "leophone", 10, 265, 160, 22, "");
  QObject::connect(leophone, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leophone, i18n("Office phone"));
  l8 = addLabel(w1, "ml8", 10, 245, 50, 20, i18n("Office phone"));

  ld8 = addLabel(w1, "mld8", 190, 270, 50, 20, i18n("Office phone number"));

  lehphone = addLineEdit(w1, "lehphone", 10, 310, 160, 22, "");
  QObject::connect(lehphone, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(lehphone, i18n("Home phone"));
  l9 = addLabel(w1, "ml9", 10, 290, 50, 20, i18n("Home phone"));

  ld9 = addLabel(w1, "mld9", 190, 315, 50, 20, i18n("Home phone number"));
  tw->addTab(w1, i18n("User info"));

  // put the new fields on a separate tab page just to avoid rewriting all
  // the positioning code.
  w5 = new QWidget(this, "wd_Extended");

  leclass = addLineEdit(w5, "leclass", 10, 80, 160, 22, "");
  QObject::connect(leclass, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leclass, i18n("Login class"));
  l19 = addLabel(w5, "ml19", 10, 60, 50, 20, i18n("Login class"));

  ld19 = addLabel(w5, "mld19", 190, 85, 50, 20, i18n("Login resource class"));

  // KDateCtl works in `days since the epoch' so we convert from seconds.
  // Set the `base' to 1 so getDate returns 0 (base - 1) if the `never expires'
  // box is checked.
  epoch->setTime_t(0);
  temp_time->setTime_t(user->getLastChange());
  lechange = new KDateCtl(w5, "lechange", i18n("Password never expires"),
                         i18n("Date when password expires"),
                         epoch->daysTo(*temp_time), 1,
                         10, 150);

  QObject::connect(lechange, SIGNAL(textChanged()), this, SLOT(changed()));

  temp_time->setTime_t(user->getExpire());

  leexpire = new KDateCtl(w5, "leexpire", i18n("Account never expires"),
                         i18n("Date when account expires"),
                         epoch->daysTo(*temp_time), 1,
                         10, 250);

  QObject::connect(leexpire, SIGNAL(textChanged()), this, SLOT(changed()));

  delete epoch;
  delete temp_time;
  tw->addTab(w5, i18n("Extended"));

#else

  leoffice1 = addLineEdit(w1, "leoffice1", 10, 220, 160, 22, "");
  QObject::connect(leoffice1, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leoffice1, i18n("The first office"));
  l7 = addLabel(w1, "ml7", 10, 200, 50, 20, i18n("Office1"));

  ld7 = addLabel(w1, "mld7", 190, 225, 50, 20, i18n("First office description"));

  leoffice2 = addLineEdit(w1, "leoffice2", 10, 265, 160, 22, "");
  QObject::connect(leoffice2, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leoffice2, i18n("The second office"));
  l8 = addLabel(w1, "ml8", 10, 245, 50, 20, i18n("Office2"));

  ld8 = addLabel(w1, "mld8", 190, 270, 50, 20, i18n("Second office description"));
  leaddress = addLineEdit(w1, "leaddress", 10, 310, 160, 22, "");
  QObject::connect(leaddress, SIGNAL(textChanged(const char *)), this, SLOT(charchanged(const char *)));
  QToolTip::add(leaddress, i18n("Postal address"));
  l9 = addLabel(w1, "ml9", 10, 290, 50, 20, i18n("Address"));
  ld9 = addLabel(w1, "mld9", 190, 315, 50, 20, i18n("Full postal address"));

  tw->addTab(w1, i18n("User info"));
#endif

#ifdef _KU_SHADOW
  if (is_shadow != 0) {
    w2 = new QWidget(this, "wd_Shadow");

    char buf[1024];

    convertdate(buf, 0, user->getLastChange());
    leslstchg = addLabel(w2, "leslstchg", 140, 30, 70, 20, buf);
    l16 = addLabel(w2, "ml16", 10, 30, 50, 20, i18n("Last password change"));

    lesmin = new KDateCtl(w2, "lesmin", i18n("Password change not allowed"),
               i18n("Date before which password cannot\nbe changed"),
                  user->getLastChange()+user->getMin(),
                  user->getLastChange(), 10, 70);
    QObject::connect(lesmin, SIGNAL(textChanged()), this, SLOT(changed()));

    lesmax = new KDateCtl(w2, "lesmax", i18n("Password will not expire"),
                          i18n("Date when password expires and\nmust be changed"),
                          user->getLastChange()+user->getMax(),
                          user->getLastChange(), 10, 130);
    QObject::connect(lesmax, SIGNAL(textChanged()), this, SLOT(changed()));


    leswarn = new KDateCtl(w2, "leswarn", i18n("No warning will be given"),
                           i18n("Date after which warning about pending\npassword expiration will be given"),
                           user->getLastChange()+user->getWarn(),
                           user->getLastChange(), 10, 190);
    QObject::connect(leswarn, SIGNAL(textChanged()), this, SLOT(changed()));

    lesinact = new KDateCtl(w2, "lesinact", i18n("Account will remain enabled"),
                            i18n("Date when account will be disabled\nif password has expired"),
                            user->getLastChange()+user->getInactive(),
                            user->getLastChange(), 10, 250);
    QObject::connect(lesinact, SIGNAL(textChanged()), this, SLOT(changed()));

    lesexpire = new KDateCtl(w2, "lesexpire", i18n("Account never expires"),
                             i18n("Date when account expires and will\nbe disabled"),
                             user->getExpire(),
                             0, 10, 310);
    QObject::connect(lesexpire, SIGNAL(textChanged()), this, SLOT(changed()));

    tw->addTab(w2, i18n("Extended"));
  }
#endif

#ifdef _KU_QUOTA
  if (is_quota != 0) {
    w3 = new QWidget(this, "wd_Quota");

    leqmnt = new QComboBox(TRUE, w3, "leqmnt");
    QToolTip::add(leqmnt, i18n("Filesystem with quotas"));
    leqmnt->clear();

    for (uint i = 0; i<mounts->getMountsNumber(); i++)
       leqmnt->insertItem(mounts->getMount(i)->getdir());

    leqmnt->setGeometry(250, 20, 160, 27);
    QObject::connect(leqmnt, SIGNAL(highlighted(int)), this, SLOT(mntsel(int)));

    l10a = addLabel(w3, "ml10a", 20, 28, 230, 20, i18n("Filesystem with quotas:"));
    l10a->setAlignment(AlignVCenter | AlignRight);

    leqfs = addLineEdit(w3, "leqfs", 10, 80, 70, 22, "");
    leqfs->setValidator(new QIntValidator(w3, "vaqfs"));
    QObject::connect(leqfs, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqfs, i18n("Disk space soft quota"));
    l10 = addLabel(w3, "ml10", 95, 85, 50, 20, i18n("Disk space soft quota"));

    leqfh = addLineEdit(w3, "leqfh", 10, 120, 70, 22, "");
    leqfh->setValidator(new QIntValidator(w3, "vaqfh"));
    QObject::connect(leqfh, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqfh, i18n("Disk space hard quota"));
    l11 = addLabel(w3, "ml11", 95, 125, 50, 20, i18n("Disk space hard quota"));

    leqfcur = addLabel(w3, "leqfcur", 13, 160, 70, 20, "");
    QToolTip::add(leqfcur, i18n("Disk space usage"));
    l14 = addLabel(w3, "ml14", 95, 160, 50, 20, i18n("Disk space usage"));

#ifndef BSD
    leqft = addLineEdit(w3, "leqft", 10, 190, 70, 22, "");
    leqft->setValidator(new QIntValidator(w3, "vaqft"));
    QObject::connect(leqft, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqft, i18n("Time limit allowed for Disk space over soft quota"));
    l18 = addLabel(w3, "ml18", 95, 195, 50, 20, i18n("Time limit allowed for Disk space over soft quota"));
#endif

    leqis = addLineEdit(w3, "leqis", 10, 235, 70, 22, "");
    leqis->setValidator(new QIntValidator(w3, "vaqis"));
    QObject::connect(leqis, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqis, i18n("File number soft quota"));
    l12 = addLabel(w3, "ml12", 95, 240, 50, 20, i18n("File number soft quota"));

    leqih = addLineEdit(w3, "leqih", 10, 275, 70, 22, "");
    leqih->setValidator(new QIntValidator(w3, "vaqih"));
    QObject::connect(leqih, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqih, i18n("File number hard quota"));
    l13 = addLabel(w3, "ml13", 95, 280, 50, 20, i18n("File number hard quota"));

    leqicur = addLabel(w3, "leqicur", 13, 315, 70, 20, "");
    QToolTip::add(leqicur, i18n("File number usage"));
    l15 = addLabel(w3, "ml15", 95, 315, 50, 20, i18n("File number usage"));

#ifndef BSD
    leqit = addLineEdit(w3, "leqit", 10, 345, 70, 22, "");
    leqit->setValidator(new QIntValidator(w3, "vaqit"));
    QObject::connect(leqit, SIGNAL(textChanged(const char *)), this, SLOT(qcharchanged(const char *)));
    QToolTip::add(leqit, i18n("Time limit allowed for File number over soft quota"));
    l17 = addLabel(w3, "ml17", 95, 350, 50, 20, i18n("Time limit allowed for File number over soft quota"));
#endif

    tw->addTab(w3, i18n("Quota"));
  }
#endif

  w4 = new QWidget(this, "wd_Groups");

  m_Other = new QListBox( w4, "m_Other" );
  m_Other->setGeometry( 15, 85, 160, 130 );
  m_Other->setFrameStyle( 51 );
  m_Other->setLineWidth( 2 );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel(w4, "Label_1");
  tmpQLabel->setGeometry(20, 65, 150, 20);
  tmpQLabel->setText(i18n("Groups not belonged to"));
  tmpQLabel->setAutoResize(true);
  tmpQLabel->setAlignment(289);
  tmpQLabel->setMargin(-1);

  m_Group = new QListBox(w4, "m_Group");
  m_Group->setGeometry(250, 85, 160, 130);
  m_Group->setFrameStyle(51);
  m_Group->setLineWidth(2);

  pbadd = new QPushButton(w4, "pbadd");
  pbadd->setGeometry(195, 105, 40, 30);
  connect( pbadd, SIGNAL(clicked()), SLOT(add()) );
  pbadd->setText("->");
  pbadd->setAutoRepeat( FALSE );
  pbadd->setAutoResize( FALSE );

  pbdel = new QPushButton( w4, "pbdel" );
  pbdel->setGeometry( 195, 155, 40, 30 );
  connect( pbdel, SIGNAL(clicked()), SLOT(del()) );
  pbdel->setText("<-");
  pbdel->setAutoRepeat( FALSE );
  pbdel->setAutoResize( FALSE );

  tmpQLabel = new QLabel(w4, "Label_2");
  tmpQLabel->setGeometry(255, 65, 150, 20);
  tmpQLabel->setText(i18n("Groups belonged to"));
  tmpQLabel->setAutoResize(true);
  tmpQLabel->setAlignment(289);
  tmpQLabel->setMargin(-1);

  cbpgrp = new QComboBox(FALSE, w4, "cbpgrp");
  cbpgrp->setGeometry(250, 30, 160, 30);
  QObject::connect(cbpgrp, SIGNAL(activated(const char *)), this, SLOT(setpgroup(const char *)));

  tmpQLabel = new QLabel(w4, "Label_3");
  tmpQLabel->setGeometry(100, 35, 140, 20);
  tmpQLabel->setText(i18n("Primary group"));
  tmpQLabel->setAlignment(AlignVCenter | AlignRight);
  tmpQLabel->setMargin(-1);

  tw->addTab(w4, i18n("Groups"));

  loadgroups();
  selectuser();
  setFixedSize(450, 470);

  ischanged = FALSE;
#ifdef _KU_QUOTA
  isqchanged = FALSE;
#endif

  setCaption(i18n("User properties"));
}

propdlg::~propdlg() {
  delete w1;
  
#ifdef _KU_SHADOW
  if (is_shadow != 0)
    delete w2;
#endif
  
#ifdef _KU_QUOTA
  if (is_quota != 0)
    delete w3;
#endif

  delete w4;
}

void propdlg::loadgroups() {
  uint i;

  for (i = 0; i<groups->count(); i++) {
    if (groups->group(i)->lookup_user(user->getName()))
      m_Group->insertItem(groups->group(i)->getName());
    else
      m_Other->insertItem(groups->group(i)->getName());

    cbpgrp->insertItem(groups->group(i)->getName());
    if (groups->group(i)->getGID() == user->getGID())
      cbpgrp->setCurrentItem(i);
  }

  if (m_Other->count() != 0)
    m_Other->setCurrentItem(0);
  if (m_Group->count() != 0)
    m_Group->setCurrentItem(0);
}

void propdlg::setpgroup(const char *) {
  ischanged = TRUE;
}

void propdlg::changed() {
  ischanged = TRUE;
}

void propdlg::add() {
  int cur = m_Other->currentItem();

  if (cur == -1)
    return;

  m_Group->insertItem(m_Other->text(cur));
  m_Other->removeItem(cur);

  if (((uint)cur) == m_Other->count())
    m_Other->setCurrentItem(cur-1);
  else
    m_Other->setCurrentItem(cur);

  m_Group->setCurrentItem(m_Group->count()-1);
  m_Group->centerCurrentItem();

  ischanged = TRUE;
}

void propdlg::del() {
  int cur = m_Group->currentItem();

  if (cur == -1)
    return;

  m_Other->insertItem(m_Group->text(cur));
  m_Group->removeItem(cur);

  if (((uint)cur) == m_Group->count())
    m_Group->setCurrentItem(cur-1);
  else
    m_Group->setCurrentItem(cur);
  
  m_Other->setCurrentItem(m_Other->count()-1);
  m_Other->centerCurrentItem();

  ischanged = TRUE;
}

void propdlg::charchanged(const char *) {
  ischanged = TRUE;
}

void propdlg::save() {
  QString tmp;
  uint newuid;
  tmp.setStr(leid->text());
  newuid = tmp.toInt();
#ifdef __FreeBSD__
  QDateTime *epoch = new QDateTime();
  QDateTime *temp_time = new QDateTime();
#endif
  
  user->setUID(newuid);

  user->setFullName(lefname->text());
#ifdef __FreeBSD__
  user->setOffice(leoffice->text());
  user->setWorkPhone(leophone->text());
  user->setHomePhone(lehphone->text());
  user->setClass(leclass->text());

  // getDate() returns days since the epoch if a date is specified
  // which we convert to seconds since the epoch.
  // getDate() returns 0 if the `never expires' box is checked which
  // ends up being converted to 0 seconds which is what we need.
  epoch->setTime_t(0);

  temp_time->setTime_t(0);
  *temp_time = temp_time->addDays(lechange->getDate());
  user->setLastChange(epoch->secsTo(*temp_time));

  temp_time->setTime_t(0);
  *temp_time = temp_time->addDays(leexpire->getDate());
  user->setExpire(epoch->secsTo(*temp_time));

  delete epoch;
  delete temp_time;
#else
  user->setOffice1(leoffice1->text());
  user->setOffice2(leoffice2->text());
  user->setAddress(leaddress->text());
#endif

  user->setHomeDir(lehome->text());
  if (leshell->currentItem() != 0)
    user->setShell(leshell->text(leshell->currentItem()));
  else
    user->setShell("");

#ifdef _KU_SHADOW
  if (is_shadow) {
    user->setMin(lesmin->getDate()-user->getLastChange());
    user->setMax(lesmax->getDate()-user->getLastChange());
    user->setWarn(leswarn->getDate()-user->getLastChange());
    user->setInactive(lesinact->getDate()-user->getLastChange());
    user->setExpire(lesexpire->getDate());
    user->setLastChange(today());
  }
#endif
}

void propdlg::saveg() {
  uint i;

  for (i=0;i<m_Group->count();i++)
    if (groups->lookup(m_Group->text(i))->lookup_user(user->getName()))
      groups->lookup(m_Group->text(i))->addUser(user->getName());
  
  for (i=0;i<m_Other->count();i++)
    if (groups->lookup(m_Other->text(i))->lookup_user(user->getName()))
      groups->lookup(m_Other->text(i))->removeUser(user->getName());

  user->setGID(groups->lookup(cbpgrp->text(cbpgrp->currentItem()))->getGID());
}

#ifdef _KU_QUOTA
void propdlg::mntsel(int index) {
  saveq();

  chquota = index;
  selectuser();
}

void propdlg::qcharchanged(const char *) {
  isqchanged = TRUE;
}

void propdlg::saveq() {
  QuotaMnt *tmpq = quota->getQuotaMnt(chquota);

  tmpq->setfhard(atol(leqfh->text()));
  tmpq->setfsoft(atol(leqfs->text()));
#ifndef BSD
  tmpq->setftime(atol(leqft->text()));
#endif
  tmpq->setihard(atol(leqih->text()));
  tmpq->setisoft(atol(leqis->text()));
#ifndef BSD
  tmpq->setitime(atol(leqit->text()));
#endif
}
#else
void propdlg::qcharchanged(const char *) {
}

void propdlg::mntsel(int) {
}
#endif

void propdlg::shactivated(const char *text) { 
  int id = leshell->currentItem();
  leshell->changeItem(text, id);
  changed();
}

bool propdlg::check() {
  bool ret = FALSE;

  if (ischanged == TRUE) {
    save();
    saveg();
    ret = TRUE;
  }

#ifdef _KU_QUOTA
  if (isqchanged == TRUE) {
    saveq();
    ret = TRUE;
  }
#endif

  return (ret);
}

void propdlg::selectuser() {
  QString uname;

  leuser->setText(user->getName());

  ksprintf(&uname, "%i",user->getUID());
  leid->setText(uname);

//  sprintf(uname,"%i",user->getp_gid());
//  legid->setText(uname);

  lefname->setText(user->getFullName());

  if (user->getShell().isEmpty() != TRUE) {
    int tested = 0;
    for (int i=0; i<leshell->count(); i++)
      if (!strcmp(leshell->text(i), user->getShell())) {
        tested = 1;
        leshell->setCurrentItem(i);
        break;
      }
    if (!tested) {
      leshell->insertItem(user->getShell());
      leshell->setCurrentItem(leshell->count()-1);
    }
  } else
    leshell->setCurrentItem(0);

  lehome->setText(user->getHomeDir());
#ifdef __FreeBSD__
  leoffice->setText(user->getOffice());
  leophone->setText(user->getWorkPhone());
  lehphone->setText(user->getHomePhone());
  leclass->setText(user->getClass());
  // the date fields get set when the dialogue is built
#else
  leoffice1->setText(user->getOffice1());
  leoffice2->setText(user->getOffice2());
  leaddress->setText(user->getAddress());
#endif

#ifdef _KU_QUOTA
  if (is_quota != 0) {
    int q = 0;
    if (chquota != -1)
      q = chquota;

    ksprintf(&uname,"%li",quota->getQuotaMnt(q)->getfsoft());
    leqfs->setText(uname);

    ksprintf(&uname,"%li",quota->getQuotaMnt(q)->getfhard());
    leqfh->setText(uname);

    ksprintf(&uname,"%li",quota->getQuotaMnt(q)->getisoft());
    leqis->setText(uname);

    ksprintf(&uname,"%li",quota->getQuotaMnt(q)->getihard());
    leqih->setText(uname);

    ksprintf(&uname,"%li",quota->getQuotaMnt(q)->getfcur());
    leqfcur->setText(uname);

    ksprintf(&uname,"%li",quota->getQuotaMnt(q)->geticur());
    leqicur->setText(uname);

#ifndef BSD
    ksprintf(&uname,"%li",quota->getQuotaMnt(q)->getftime());
    leqft->setText(uname);

    ksprintf(&uname,"%li",quota->getQuotaMnt(q)->getitime());
    leqit->setText(uname);
#endif
  }
#endif
}

void propdlg::setpwd() {
  pwddlg *pd;

  pd = new pwddlg(user, this, "pwddlg");
  pd->exec();
  ischanged = TRUE;
  delete pd;
}

void propdlg::ok() {
  QString tmp;
  uint newuid;
  tmp.setStr(leid->text());
  newuid = tmp.toInt();

  if (olduid != newuid)
    if (users->lookup(newuid) != NULL) {
      tmp.sprintf(i18n("User with UID %u already exists"), newuid);
      err->addMsg(tmp, STOP);
      err->display();
      return;
    }
    
  if (check() == TRUE)
    accept();
  else
    reject();
}

void propdlg::cancel() {
  reject();
}












