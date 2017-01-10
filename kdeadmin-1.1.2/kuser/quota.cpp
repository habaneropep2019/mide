#ifdef _KU_QUOTA

#include <kmsgbox.h>

#include "misc.h"
#include "maindlg.h"
#include "mnt.h"
#include "quota.h"

#ifdef HAVE_LINUX_QUOTA_H
#  ifndef QUOTACTL_IN_LIBC
#    include <syscall.h>

int quotactl(int cmd, const char * special, int id, caddr_t addr) {
  return syscall(SYS_quotactl, cmd, special, id, addr);
}

#  endif
#endif

QuotaMnt::QuotaMnt() {
  fcur = 0; fsoft = 0; fhard = 0; ftime = 0;
  icur = 0; isoft = 0; ihard = 0; itime = 0;
}
  
QuotaMnt::QuotaMnt(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard) {
  fcur = afcur; fsoft = afsoft; fhard = afhard; ftime = 0;
  icur = aicur; isoft = aisoft; ihard = aihard; itime = 0;
}

QuotaMnt::QuotaMnt(int afcur, int afsoft, int afhard, int aicur, int aisoft, int aihard, int aftime, int aitime) {
  fcur = afcur; fsoft = afsoft; fhard = afhard; ftime = aftime;
  icur = aicur; isoft = aisoft; ihard = aihard; itime = aitime;
}

QuotaMnt::QuotaMnt(const QuotaMnt *q) {
  fcur = q->fcur; fsoft = q->fsoft; fhard = q->fhard; ftime = q->ftime;
  icur = q->icur; isoft = q->isoft; ihard = q->ihard; itime = q->itime;
}

QuotaMnt::~QuotaMnt() {
}

long QuotaMnt::getfcur() {
  return (fcur);
}

long QuotaMnt::getfsoft() {
  return (fsoft);
}

long QuotaMnt::getfhard() {
  return (fhard);
}

long QuotaMnt::geticur() {
  return (icur);
}

long QuotaMnt::getisoft() {
  return (isoft);
}

long QuotaMnt::getihard() {
  return (ihard);
}

long QuotaMnt::getftime() {
  return (ftime);
}

long QuotaMnt::getitime() {
  return (itime);
}

void QuotaMnt::setfcur(long data) {
  fcur = data;
}

void QuotaMnt::setfsoft(long data) {
  fsoft = data;
}

void QuotaMnt::setfhard(long data) {
  fhard = data;
}

void QuotaMnt::seticur(long data) {
  icur = data;
}

void QuotaMnt::setisoft(long data) {
  isoft = data;
}

void QuotaMnt::setihard(long data) {
  ihard = data;
}

void QuotaMnt::setftime(long data) {
  ftime = data;
}

void QuotaMnt::setitime(long data) {
  itime = data;
}

Quota::Quota(unsigned int auid, bool doget) {
  uid = auid;

  q.setAutoDelete(TRUE);
  if (is_quota == 0)
    return;

  if (!doget) {
    for (uint i=0; i<mounts->getMountsNumber(); i++)
      q.append(new QuotaMnt);
    return;
  }

  static int warned = 0;
  struct dqblk dq;
#ifdef _KU_UFS_QUOTA
  int fd;
  struct quotctl qctl;
  int dd = 0;
#endif

#ifdef _KU_UFS_QUOTA
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    qctl.op = Q_GETQUOTA;
    qctl.uid = uid;
    qctl.addr = (caddr_t) &dq;
    
    fd = open((const char *)mounts->getMount(i)->getquotafilename(), O_RDONLY);
    
    if ((dd = ioctl(fd, Q_QUOTACTL, &qctl)) != 0)
      if (errno == ESRCH) {
        q.append(new QuotaMnt());
        close(fd);
        continue;
      }
      else {
/*
        if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
*/
	warned++;
//	KMsgBox::message(0, i18n("Error"), i18n("Quotas are not compiled into this kernel."), KMsgBox::STOP);
//	printf("errno: %i, ioctl: %i\n", errno, dd);
	sleep(3);
	is_quota = 0;
	close(fd);
	break;
      }
    q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
                          dbtob(dq.dqb_bsoftlimit)/1024,
                          dbtob(dq.dqb_bhardlimit)/1024,
                          dq.dqb_curfiles,
                          dq.dqb_fsoftlimit,
                          dq.dqb_fhardlimit,
                          dq.dqb_btimelimit/(3600*1600),
                          dq.dqb_ftimelimit/(3600*1600)));
    close(fd);
  }
#endif

#ifdef _KU_EXT2_QUOTA
  int qcmd = QCMD(Q_GETQUOTA, USRQUOTA);

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    if (quotactl(qcmd, (const char *)mounts->getMount(i)->getfsname(), uid, (caddr_t) &dq) != 0) {
/*
        if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
*/
	warned++;
//	KMsgBox::message(0, i18n("Error"), i18n("Quotas are not compiled into this kernel."), KMsgBox::STOP);
//	sleep(3);
	is_quota = 0;
	break;
	/*
	  }
	*/
      }
      q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
			    dbtob(dq.dqb_bsoftlimit)/1024,
			    dbtob(dq.dqb_bhardlimit)/1024,
			    dq.dqb_curinodes,
			    dq.dqb_isoftlimit,
			    dq.dqb_ihardlimit,
			    dq.dqb_btime/3600,
			    dq.dqb_itime/3600));
  }
#endif

#ifdef HAVE_IRIX
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    if (quotactl(Q_GETQUOTA, (const char *)mounts->getMount(i)->getfsname(), uid, (caddr_t) &dq) != 0) {
	if(!warned) {
            warned++;
            printf("errno: %i\n", errno);
            sleep(3);
            is_quota = 0;
	  }
	  break;
      }
      q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
                            dbtob(dq.dqb_bsoftlimit)/1024,
                            dbtob(dq.dqb_bhardlimit)/1024,
                            dq.dqb_curfiles,
                            dq.dqb_fsoftlimit,
                            dq.dqb_fhardlimit,
                            dq.dqb_btimelimit/3600,
                            dq.dqb_ftimelimit/3600));
  }
#endif

#ifdef BSD
  int qcmd = QCMD(Q_GETQUOTA, USRQUOTA);

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    if (quotactl((const char *)mounts->getMount(i)->getdir(), qcmd, uid, (caddr_t) &dq) !=0) {
      //  printf("%d\n",errno);
      warned++;
      if (errno != EINVAL) /* For _SOME_ reason quotactl returns EINVAL vs EPERM or ENODEV when quotas are disabled*/
            fprintf(stderr,"error: \"%s\" while calling quotactl\n", strerror(errno));
//      KMsgBox::message(0, i18n("Error"), i18n("Quotas are not compiled into this kernel."), KMsgBox::STOP);
     /* Why punish people who don't want restrictions? sleep(3); */
      is_quota = 0;
      break;
    }
    q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
			  dbtob(dq.dqb_bsoftlimit)/1024,
			  dbtob(dq.dqb_bhardlimit)/1024,
			  dq.dqb_curinodes,
			  dq.dqb_isoftlimit,
			  dq.dqb_ihardlimit,
			  dq.dqb_btime/3600,
			  dq.dqb_btime/3600));
  }
#endif

#ifdef _KU_HPUX_QUOTA
  int fd;
  int dd = 0;

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    
    if ((dd = quotactl(Q_GETQUOTA, (const char *)mounts->getMount(i)->getquotafilename(), uid, &dq)) != 0)
      if (errno == ESRCH) {
        q.append(new QuotaMnt());
        close(fd);
        continue;
      }
      else {
/*
        if ((errno == EOPNOTSUPP || errno == ENOSYS) && !warned) {
*/
	warned++;
//	KMsgBox::message(0, i18n("Error"), i18n("Quotas are not compiled into this kernel."), KMsgBox::STOP);
//	printf("errno: %i, ioctl: %i\n", errno, dd);
//	sleep(3);
	is_quota = 0;
	close(fd);
	break;
      }
    q.append(new QuotaMnt(dbtob(dq.dqb_curblocks)/1024,
                          dbtob(dq.dqb_bsoftlimit)/1024,
                          dbtob(dq.dqb_bhardlimit)/1024,
                          dq.dqb_curfiles,
                          dq.dqb_fsoftlimit,
                          dq.dqb_fhardlimit,
                          dq.dqb_btimelimit/3600,
                          dq.dqb_ftimelimit/3600));
    close(fd);
  }
#endif

}

Quota::~Quota() {
  q.clear();
}

QuotaMnt *Quota::getQuotaMnt(uint mntnum) {
  return (q.at(mntnum));
}

uint Quota::getMountsNumber() {
  return (q.count());
}

bool Quota::save() {
  if (is_quota == 0)
    return (TRUE);

  struct dqblk dq;

#ifdef _KU_UFS_QUOTA
  int fd;
  struct quotctl qctl;
  int dd = 0;

  qctl.op = Q_SETQUOTA;
  qctl.uid = uid;
  qctl.addr = (caddr_t) &dq;

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q.at(i)->getfcur()*1024);
    dq.dqb_bsoftlimit = btodb(q.at(i)->getfsoft()*1024);
    dq.dqb_bhardlimit = btodb(q.at(i)->getfhard()*1024);
    dq.dqb_curfiles   = q.at(i)->geticur();
    dq.dqb_fsoftlimit = q.at(i)->getisoft();
    dq.dqb_fhardlimit = q.at(i)->getihard();
    dq.dqb_btimelimit = q.at(i)->getftime()*3600;
    dq.dqb_ftimelimit = q.at(i)->getitime()*3600;
    
    fd = open((const char *)mounts->getMount(i)->getquotafilename(), O_WRONLY);

    if ((dd = ioctl(fd, Q_QUOTACTL, &qctl)) != 0)
      if (errno == ESRCH) {
//      printf("Warning ESRCH %il \n", id);
      }
      else
      {
        sleep(3);
        is_quota = 0;
        close(fd);
        break;
      }
    close(fd);
  }
#endif

#ifdef _KU_EXT2_QUOTA
  int dd = 0;
  int qcmd = QCMD(Q_SETQUOTA, USRQUOTA);
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q.at(i)->getfcur()*1024);
    dq.dqb_bsoftlimit = btodb(q.at(i)->getfsoft()*1024);
    dq.dqb_bhardlimit = btodb(q.at(i)->getfhard()*1024);
    dq.dqb_curinodes  = q.at(i)->geticur();
    dq.dqb_isoftlimit = q.at(i)->getisoft();
    dq.dqb_ihardlimit = q.at(i)->getihard();
    dq.dqb_btime = q.at(i)->getftime()*3600;
    dq.dqb_itime = q.at(i)->getitime()*3600;

    if ((dd =quotactl(qcmd, (const char *)mounts->getMount(i)->getfsname(), uid, (caddr_t) &dq)) != 0) {
      printf("Quotactl returned: %d\n", dd);
      continue;
    }
  }
#endif

#ifdef HAVE_IRIX
  int dd = 0;
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q.at(i)->getfcur()*1024);
    dq.dqb_bsoftlimit = btodb(q.at(i)->getfsoft()*1024);
    dq.dqb_bhardlimit = btodb(q.at(i)->getfhard()*1024);
    dq.dqb_curfiles  = q.at(i)->geticur();
    dq.dqb_fsoftlimit = q.at(i)->getisoft();
    dq.dqb_fhardlimit = q.at(i)->getihard();
    dq.dqb_btimelimit = q.at(i)->getftime()*3600;
    dq.dqb_ftimelimit = q.at(i)->getitime()*3600;

    if ((dd =quotactl(Q_SETQUOTA, (const char *)mounts->getMount(i)->getfsname(), uid, (caddr_t) &dq)) != 0) {
      printf("Quotactl returned: %d\n", dd);
      continue;
    }
  }
#endif

#ifdef BSD
  int dd = 0;
  int qcmd = QCMD(Q_SETQUOTA,USRQUOTA);

  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q.at(i)->getfcur()*1024);
    dq.dqb_bsoftlimit = btodb(q.at(i)->getfsoft()*1024);
    dq.dqb_bhardlimit = btodb(q.at(i)->getfhard()*1024);
    dq.dqb_curinodes  = q.at(i)->geticur();
    dq.dqb_isoftlimit = q.at(i)->getisoft();
    dq.dqb_ihardlimit = q.at(i)->getihard();
    dq.dqb_btime = q.at(i)->getftime()*3600;
    dq.dqb_itime = q.at(i)->getitime()*3600;

    if ((dd =quotactl((const char *)mounts->getMount(i)->getfsname(), qcmd, uid, (caddr_t) &dq)) != 0) {
      printf("Quotactl returned: %d\n", dd);
      continue;
    }
  }
#endif

#ifdef _KU_HPUX_QUOTA
  int dd = 0;
  for (uint i=0; i<mounts->getMountsNumber(); i++) {
    dq.dqb_curblocks  = btodb(q.at(i)->getfcur()*1024);
    dq.dqb_bsoftlimit = btodb(q.at(i)->getfsoft()*1024);
    dq.dqb_bhardlimit = btodb(q.at(i)->getfhard()*1024);
    dq.dqb_curfiles  = q.at(i)->geticur();
    dq.dqb_fsoftlimit = q.at(i)->getisoft();
    dq.dqb_fhardlimit = q.at(i)->getihard();
    dq.dqb_btimelimit = q.at(i)->getftime()*3600;
    dq.dqb_ftimelimit = q.at(i)->getitime()*3600;

    if ((dd =quotactl(Q_SETQUOTA, (const char *)mounts->getMount(i)->getfsname(), uid, &dq)) != 0) {
      printf("Quotactl returned: %d\n", dd);
      continue;
    }
  }
#endif

  return (TRUE);
}

unsigned int Quota::getUid() {
  return (uid);
}

Quotas::Quotas() {
  q.setAutoDelete(TRUE);
}

Quotas::~Quotas() {
  q.clear();
}

Quota *Quotas::getQuota(unsigned int uid) {
  return (q[uid]);
}

void Quotas::addQuota(unsigned int uid) {
  Quota *tmpQ = NULL;

  if (!q[uid]) {
    tmpQ = new Quota(uid);
    q.insert(uid, tmpQ);
  }
}

void Quotas::addQuota(Quota *aq) {
  unsigned int uid;

  uid = aq->getUid();

  if (!q[uid])
    q.insert(uid, aq);
}

void Quotas::delQuota(unsigned int uid) {
  q.remove(uid);
}

bool Quotas::save() {
  QIntDictIterator<Quota> qi(q);

  while (qi.current()) {
    if (!qi.current()->save())
      return (FALSE);
    ++qi;
  }
  return (TRUE);
}

#endif // _KU_QUOTA
