// proc.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sched.h>

#include "qps.h"
#include "proc.h"
#include "svec.C"
#include "uidstr.h"
#include "ttystr.h"
#include "wchan.h"
#include "details.h"

const int Procinfo::CPUTIMES = CPU_IDLE + 1;
const int Procinfo::MAX_CMD_LEN = 4096;
const int Procview::cpu_avg_time = 30 * 1000;	// averaging time for WCPU (ms)

char *procdir = "/proc";

int Procinfo::pagesize = getpagesize();

Procinfo::Procinfo(int proc_pid)
{
    if(readproc(proc_pid) < 0)
	pid = -1;		// invalidate object, will be deleted
    selected = FALSE;
    viewed = FALSE;
    details = 0;
    sock_inodes = 0;
    socks_current = FALSE;
    socks.setAutoDelete(TRUE);
}

Procinfo::~Procinfo()
{
    if(sock_inodes)
	delete sock_inodes;
}

// return number of bytes read if ok, -1 if failed
int Procinfo::read_file(char *name, char *buf, int max)
{	
    int fd = open(name, O_RDONLY);
    if(fd < 0) return -1;
    int r = read(fd, buf, max);
    if(r < 0) return -1;
    close(fd);
    return r;
}

int Procinfo::readproc(int proc_pid)
{
    char path[256];
    char buf[256];
    char sbuf[384];		// should be enough to acommodate /proc/X/stat
    char cmdbuf[MAX_CMD_LEN];

    sprintf(path, "%s/%d", procdir, proc_pid);

    // read /proc/XX/cmdline
    strcpy(buf, path);
    strcat(buf, "/cmdline");
    int cmdlen;
    if((cmdlen = read_file(buf, cmdbuf, MAX_CMD_LEN - 1)) < 0) return -1;
    if(cmdlen == 0) {
	cmdline = "";
    } else {
	for(int i = 0; i < cmdlen; i++)
	    if(cmdbuf[i] == '\0') cmdbuf[i] = ' ';
	int j = cmdlen - 1;
	while(j >= 0 && cmdbuf[j] == ' ')
	    j--;
	cmdbuf[j + 1] = '\0';
	cmdline = cmdbuf;
    }

    // read /proc/XX/stat
    strcpy(buf, path);
    strcat(buf, "/stat");
    int statlen;
    if((statlen = read_file(buf, sbuf, sizeof(sbuf) - 1)) <= 0) return -1;
    sbuf[statlen] = '\0';
    char *p = strrchr(sbuf, ')');
    *p = '\0';			// split in two parts
    memset(buf, 0, sizeof(buf));
    sscanf(sbuf, "%d (%39c", &pid, buf);
    comm = buf;
    sscanf(p + 2, "%c %d %d %d %d %d %u %u %u %u %u "
	   "%d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u "
	   "%d %d %d %d %u",
	   &state, &ppid, &pgrp, &session, &tty, &tpgid,
	   &flags, &minflt, &cminflt, &majflt, &cmajflt,
	   &utime, &stime, &cutime, &cstime, &priority, &nice,
	   &timeout, &itrealvalue, &starttime, &vsize, &rss, &rlim,
	   &startcode, &endcode, &startstack, &kstkesp, &kstkeip,
	   &signal, &blocked, &sigignore, &sigcatch, &wchan);
    rss *= pagesize / 1024;	// rss given in pages, display in Kbytes
    pmem = 100.0 * rss / mem_total;

    // read /proc/XX/statm
    strcpy(buf, path);
    strcat(buf, "/statm");
    if((statlen = read_file(buf, sbuf, sizeof(sbuf) - 1)) <= 0) return -1;
    sbuf[statlen] = '\0';
    sscanf(sbuf, "%u %u %u %u %u %u %u",
	   &size, &resident, &share, &trs, &lrs, &drs, &dt);
    size *= pagesize / 1024;
    resident *= pagesize / 1024;
    share *= pagesize / 1024;
    trs *= pagesize / 1024;
    lrs *= pagesize / 1024;
    drs *= pagesize / 1024;

    // read /proc/XX/status
    strcpy(buf, path);
    strcat(buf, "/status");
    if((statlen = read_file(buf, sbuf, sizeof(sbuf) - 1)) <= 0) return -1;
    sbuf[statlen] = '\0';
    p = sbuf;
    for(int i = 0; i < 4; i++)
	p = strchr(p + 1, '\n');	// skip four lines
    sscanf(p + 1, "Uid: %d %d %d %d Gid: %d %d %d %d",
	   &uid, &euid, &suid, &fsuid,
	   &gid, &egid, &sgid, &fsgid);

    gettimeofday(&tv, 0);
    policy = -1;		// will get it when needed
    rtprio = -1;		// ditto

    return pid;
}

float Procinfo::loadavg[] = {0.0, 0.0, 0.0};
int Procinfo::mem_total = 0;
int Procinfo::mem_free = 0;
int Procinfo::mem_shared = 0;
int Procinfo::mem_buffers = 0;
int Procinfo::mem_cached = 0;
int Procinfo::swap_total = 0;
int Procinfo::swap_free = 0;
unsigned Procinfo::cpu_time[];
unsigned Procinfo::old_cpu_time[];
unsigned Procinfo::boot_time = 0;
int Procinfo::uptime = 0;
QIntDict<Sockinfo> Procinfo::socks(101);
bool Procinfo::socks_current = FALSE;

// just grab the load averages
void Procinfo::read_loadavg()
{
    char path[80];
    char buf[512];
    strcpy(path, procdir);
    strcat(path, "/loadavg");
    if(read_file(path, buf, sizeof(buf)) <= 0) {
	fprintf(stderr,	"qps: Cannot open /proc/loadavg"
		        " (make sure /proc is mounted)\n");
	exit(1);
    }
    sscanf(buf, "%f %f %f", &loadavg[0], &loadavg[1], &loadavg[2]);
}

// read information common to all processes
void Procinfo::read_common()
{
    char path[80];
    char buf[4096];    // fix for segfault in 2.2.x kernels

    // read memory info
    strcpy(path, procdir);
    strcat(path, "/meminfo");
    if(read_file(path, buf, sizeof(buf)) <= 0) return;

    // Skip the old /meminfo cruft, making this work in post-2.0.42 kernels
    // as well.  (values are now in kB)
    char *p = strstr(buf, "MemTotal:");
    sscanf(p, "MemTotal: %d kB\nMemFree: %d kB\nMemShared: %d kB\nBuffers: %d"
              " kB\nCached: %d kB\nSwapTotal: %d kB\n SwapFree: %d",
	   &mem_total, &mem_free, &mem_shared, &mem_buffers,
	   &mem_cached, &swap_total, &swap_free);

    // read system status
    strcpy(path, procdir);
    strcat(path, "/stat");
    if(read_file(path, buf, sizeof(buf)) <= 0) return;
    for(int i = 0; i < CPUTIMES; i++)
	old_cpu_time[i] = cpu_time[i];
    sscanf(buf, "cpu %u %u %u %u",
	   &cpu_time[CPU_USER], &cpu_time[CPU_NICE],
	   &cpu_time[CPU_SYSTEM], &cpu_time[CPU_IDLE]);
    p = strstr(buf, "btime");
    if(!p) {
      fprintf(stderr, "Warning: btime not found, unknown btime!\n");
      boot_time = time(NULL);
    } else
    sscanf(p+6, "%u", &boot_time);

    // read uptime
    strcpy(path, procdir);
    strcat(path, "/uptime");
    if(read_file(path, buf, sizeof(buf)) < 0) return;
    float up_s;
    sscanf(buf, "%f", &up_s);
    uptime = (int)up_s;
}

int Procinfo::get_policy()
{
    if(policy == -1)
	policy = sched_getscheduler(pid);
    return policy;
}

int Procinfo::get_rtprio()
{
    if(rtprio < 0) {
	struct sched_param p;
	if(sched_getparam(pid, &p) == 0)
	    rtprio = p.sched_priority;
    }
    return rtprio;
}

// return TRUE if /proc/PID/fd could be read, FALSE otherwise
bool Procinfo::read_socket_inodes()
{
    if(!sock_inodes)
	sock_inodes = new Svec<int>;

    sock_inodes->clear();

    char path[80], *p;
    sprintf(path, "%s/%d/fd", procdir, pid);

    DIR *d = opendir(path);
    if(!d) return FALSE;

    p = path + strlen(path) + 1;
    p[-1] = '/';

    struct dirent *e;
    char buf[80];
    while((e = readdir(d)) != 0) {
	strcpy(p, e->d_name);
	int len;
	if((len = readlink(path, buf, sizeof(buf) - 1)) >= 0) {
	    buf[len] = '\0';
	    unsigned dev, ino;
	    sscanf(buf, "[%x]:%d", &dev, &ino);
	    if(dev == 0 && Procinfo::socks[ino] != 0)
		sock_inodes->add(ino);
	}
    }
    closedir(d);
    return TRUE;
}

bool Procinfo::read_socket_list(Sockinfo::proto_t proto, char *pseudofile)
{
    char path[80];
    strcpy(path, procdir);
    strcat(path, "/net/");
    strcat(path, pseudofile);
    FILE *f = fopen(path, "r");
    if(!f) return FALSE;

    char buf[256];
    while(fgets(buf, sizeof(buf), f) != 0) {
	Sockinfo *si = new Sockinfo();
	si->proto = proto;
	unsigned local_port, rem_port, st, tr;
	sscanf(buf + 6, "%x:%x %x:%x %x %x:%x %x:%x %x %d %d %d",
	       &si->local_addr, &local_port, &si->rem_addr, &rem_port,
	       &st, &si->tx_queue, &si->rx_queue,
	       &tr, &si->tm_when, &si->rexmits,
	       &si->uid, &si->timeout, &si->inode);
	// fix fields that aren't sizeof(int)
	si->local_port = local_port;
	si->rem_port = rem_port;
	si->st = st;
	si->tr = tr;

	socks.insert(si->inode, si);
    }
    fclose(f);
    return TRUE;
}

void Procinfo::read_sockets()
{
    if(socks_current)
	return;

    socks.clear();
    if(!read_socket_list(Sockinfo::TCP, "tcp")
       || !read_socket_list(Sockinfo::UDP, "udp"))
	return;

    socks_current = TRUE;
}

void Procinfo::invalidate_sockets()
{
    socks_current = FALSE;
}

int Category::compare(Procinfo *a, Procinfo *b)
{
    return strcmp(string(a), string(b));
}

Cat_int::Cat_int(char *heading, char *explain, int w, int Procinfo::*member)
       : Category(heading, explain), int_member(member), field_width(w)
{}

QString Cat_int::string(Procinfo *p)
{
    QString s(10);
    s.setNum(p->*int_member);	
    return s;
}

int Cat_int::compare(Procinfo *a, Procinfo *b)
{
    int ai = a->*int_member, bi = b->*int_member;
    return ai > bi ? 1 : (ai < bi ? -1 : 0);
}

Cat_uint::Cat_uint(char *heading, char *explain, int w,
		   unsigned Procinfo::*member)
        : Category(heading, explain), uint_member(member), field_width(w)
{}

QString Cat_uint::string(Procinfo *p)
{
    QString s(10);
    s.setNum(p->*uint_member);	
    return s;
}

int Cat_uint::compare(Procinfo *a, Procinfo *b)
{
    unsigned ai = a->*uint_member, bi = b->*uint_member;
    return ai < bi ? 1 : (ai > bi ? -1 : 0);
}

Cat_hex::Cat_hex(char *heading, char *explain, int w,
		 unsigned Procinfo::*member)
        : Cat_uint(heading, explain, w, member)
{}

QString Cat_hex::string(Procinfo *p)
{
    QString s(11);
    s.sprintf("%8x", p->*uint_member);
    return s;
}

Cat_swap::Cat_swap(char *heading, char *explain)
        : Category(heading, explain)
{}

QString Cat_swap::string(Procinfo *p)
{
    QString s(10);
    s.setNum(p->size - p->resident);
    return s;
}

int Cat_swap::compare(Procinfo *a, Procinfo *b)
{
    unsigned as = a->size - a->resident, bs = b->size - b->resident;
    return as < bs ? 1 : (as > bs ? -1 : 0);
}

Cat_string::Cat_string(char *heading, char *explain, QString Procinfo::*member)
        : Category(heading, explain), str_member(member)
{}

QString Cat_string::string(Procinfo *p)
{
    return p->*str_member;
}

Cat_user::Cat_user(char *heading, char *explain)
        : Cat_string(heading, explain)
{}

QString Cat_user::string(Procinfo *p)
{
    if(p->uid == p->euid)
	return Uidstr::name(p->uid);
    else {
	QString s = Uidstr::name(p->uid);
	s.detach();
	s.append(p->euid == 0 ? "*" : "+");
	return s;
    }
}

Cat_wchan::Cat_wchan(char *heading, char *explain)
        : Cat_string(heading, explain)
{}

QString Cat_wchan::string(Procinfo *p)
{
    return Wchan::name(p->wchan);
}

Cat_cmdline::Cat_cmdline(char *heading, char *explain)
            : Cat_string(heading, explain)
{}

QString Cat_cmdline::string(Procinfo *p)
{
    if(p->cmdline.isEmpty()) {
	QString s(p->comm.length() + 2);
	s[0] = '('; s[1] = '\0';
	s.append(p->comm);
	s.append(")");
	return s;
    } else {
	if(Qps::show_cmd_path)
	    return p->cmdline;
	else {
	    QString s(p->cmdline);
	    s.detach();
	    int i = s.find(' ');
	    if(i < 0)
		i = s.length();
	    if(i > 0) {
		i = s.findRev('/', i - 1);
		if(i >= 0)
		    s.remove(0, i + 1);
	    }
	    return s;
	}
    }
}

Cat_state::Cat_state(char *heading, char *explain)
        : Category(heading, explain)
{}

QString Cat_state::string(Procinfo *p)
{
    QString s("   ");
    s[0] = p->state;
    s[1] = (p->rss == 0 && p->state != 'Z') ? 'W' : ' ';
    s[2] = (p->nice > 0) ? 'N' : ((p->nice < 0) ? '<' : ' ');
    return s;
}

Cat_policy::Cat_policy(char *heading, char *explain)
           : Category(heading, explain)
{}

QString Cat_policy::string(Procinfo *p)
{
    char c;
    switch(p->get_policy()) {
    case SCHED_FIFO:
	c = 'F'; break;
    case SCHED_RR:
	c = 'R'; break;
    case SCHED_OTHER:
	c = 'O'; break;
    default:
	c = '?'; break;
    }
    QString s(" ");
    s[0] = c;
    return s;
}

int Cat_policy::compare(Procinfo *a, Procinfo *b)
{
    int ap = a->get_policy(), bp = b->get_policy();
    return ap < bp ? 1 : (ap > bp ? -1 : 0);
}

Cat_rtprio::Cat_rtprio(char *heading, char *explain)
        : Category(heading, explain)
{}

QString Cat_rtprio::string(Procinfo *p)
{
    QString s(5);
    s.setNum(p->get_rtprio());
    return s;
}

int Cat_rtprio::compare(Procinfo *a, Procinfo *b)
{
    unsigned ap = a->get_rtprio(), bp = b->get_rtprio();
    return ap < bp ? 1 : (ap > bp ? -1 : 0);
}

Cat_time::Cat_time(char *heading, char *explain)
        : Category(heading, explain)
{}

QString Cat_time::string(Procinfo *p)
{
    QString s(8);
    int t = (p->utime + p->stime) / CLK_TCK;
    if(t >= 100 * 60) {
	if(t >= 100 * 3600) {
	    int d = t / 86400;
	    t /= 3600;
            if(d >= 100)
              s.sprintf("%d.%dd", d, ((t - 24*d)*10)/24);
            else
	      s.sprintf("%dd%2dh", d, t - 24*d);
	} else {
	    int h = t / 3600;
	    t %= 3600;
	    s.sprintf("%2d:%02dh", h, t / 60);
	}
    } else if(t < 10) {
	int hundreds = (p->utime + p->stime) / (CLK_TCK / 100) % 100;
	s.sprintf("%1d.%02ds", t, hundreds);
    } else {
	s.sprintf("%2d:%02d", t / 60, t % 60);
    }
    return s;
}

int Cat_time::compare(Procinfo *a, Procinfo *b)
{
    int at = a->utime + a->stime, bt = b->utime + b->stime;
    return at < bt ? 1 : (at > bt ? -1 : 0);
}

Cat_start::Cat_start(char *heading, char *explain)
          : Category(heading, explain)
{}

QString Cat_start::string(Procinfo *p)
{
    time_t start = p->boot_time + p->starttime / CLK_TCK;
    QString s;
    char *ct = ctime(&start);
    if(p->tv.tv_sec - start < 86400) {
	ct[16] = '\0';
	s = ct + 11;
    } else {
	ct[10] = '\0';
	s = ct + 4;
    }
    return s;
}

int Cat_start::compare(Procinfo *a, Procinfo *b)
{
    int at = a->boot_time + a->starttime / CLK_TCK;
    int bt = b->boot_time + b->starttime / CLK_TCK;
    return at < bt ? 1 : (at > bt ? -1 : 0);
}

Cat_percent::Cat_percent(char *heading, char *explain, int w,
			 float Procinfo::*member)
        : Category(heading, explain), float_member(member), field_width(w)
{}

QString Cat_percent::string(Procinfo *p)
{
    QString s(8);
    s.sprintf("%01.2f", (double)(p->*float_member));
    return s;
}

int Cat_percent::compare(Procinfo *a, Procinfo *b)
{
    float at = a->*float_member, bt = b->*float_member;
    return at < bt ? 1 : (at > bt ? -1 : 0);
}

Cat_tty::Cat_tty(char *heading, char *explain)
        : Category(heading, explain)
{}

QString Cat_tty::string(Procinfo *p)
{
    return Ttystr::name(p->tty);
}

Proc::Proc()
{
    cats[F_PID] = new Cat_int("PID", "Process ID", 44, &Procinfo::pid);
    cats[F_PPID] = new Cat_int("PPID", "Parent process ID", 44,
			       &Procinfo::ppid);
    cats[F_PGID] = new Cat_int("PGID", "Process group ID", 44,
			       &Procinfo::pgrp);
    cats[F_SID] = new Cat_int("SID", "Session ID", 44,
			      &Procinfo::session);
    cats[F_TTY] = new Cat_tty("TTY", "Controlling tty");
    cats[F_TPGID] = new Cat_int("TPGID", "Process group ID of tty owner", 44,
				&Procinfo::tpgid);
    cats[F_USER] = new Cat_user("USER",
				"Owner (*=suid root, +=suid other user)");
    cats[F_NICE] = new Cat_int("NICE",
			       "Priority (more positive means less cpu time)",
			       36, &Procinfo::nice);
    cats[F_PRI] = new Cat_int("PRI",
			      "Time left of possible timeslice (rescaled)", 32,
			      &Procinfo::priority);
    cats[F_PLCY] = new Cat_policy("PLCY",
				  "Scheduling policy (FIFO, RR or OTHER)");
    cats[F_RPRI] = new Cat_rtprio("RPRI",
				  "Realtime priority (0-99, more is better)");
    cats[F_MAJFLT] = new Cat_uint("MAJFLT",
				  "Number of major faults (loading from disk)",
				  56, &Procinfo::majflt);
    cats[F_MINFLT] = new Cat_uint("MINFLT",
				  "Number of minor faults (no disk access)",
				  52, &Procinfo::minflt);
    cats[F_TRS] = new Cat_uint("TRS", "Text resident set size in Kbytes", 40,
			       &Procinfo::trs);
    cats[F_DRS] = new Cat_uint("DRS", "Data resident set size in Kbytes", 40,
			       &Procinfo::drs);
    cats[F_SIZE] = new Cat_uint("SIZE",
				"Virtual image size of process in Kbytes", 40,
				&Procinfo::size);
    cats[F_SWAP] = new Cat_swap("SWAP", "Kbytes on swap device");
    cats[F_RSS] = new Cat_uint("RSS",
			     "Resident set size; Kbytes of program in memory",
			       40, &Procinfo::resident);
    cats[F_SHARE] = new Cat_uint("SHARE", "Shared memory in Kbytes",
				 48, &Procinfo::share);
    cats[F_DT] = new Cat_uint("DT", "Number of dirty (non-written) pages",
			      40, &Procinfo::dt);
    cats[F_STAT] = new Cat_state("STAT", "State of the process");
    cats[F_FLAGS] = new Cat_hex("FLAGS", "Process flags (hex)", 60,
				&Procinfo::flags);
    cats[F_WCHAN] = new Cat_wchan("WCHAN",
				  "Kernel function where process is sleeping");
    cats[F_UID] = new Cat_int("UID", "User ID", 40, &Procinfo::uid);
    cats[F_WCPU] = new Cat_percent("%WCPU",
				   "Weighted percentage of CPU (30 s average)",
				   52, &Procinfo::wcpu);
    cats[F_CPU] = new Cat_percent("%CPU",
				  "Percentage of CPU used since last update",
				  42, &Procinfo::pcpu);
    cats[F_MEM] = new Cat_percent("%MEM",
				  "Percentage of memory used (RSS/total mem)",
				  46, &Procinfo::pmem);
    cats[F_START] = new Cat_start("START", "Time process started");
    cats[F_TIME] = new Cat_time("TIME", "Total CPU time used since start");
    cats[F_COMM] = new Cat_string("COMM", "Command that started the process",
				  &Procinfo::comm);
    cats[F_CMDLINE] = new Cat_cmdline("CMDLINE",
				      "Command line that started the process");

    for(int i = 0; i < cats.size(); i++)
	cats[i]->index = i;

#if 0
    // try downcasing the headings, to see if that's more readable
    // (but less ps/top compatible, probably not worth it)
    for(int i = 0; i < cats.size(); i++) {
	char *p = strdup(cats[i]->name);
	for(char *q = p + 1 + (*p == '%'); *q; q++)
	    *q += 'a' - 'A';
	cats[i]->name = p;
    }
#endif
}

// update the process list
void Proc::refresh()
{
    Procinfo::read_common();
    Svec<Procinfo *> oldprocs = procs;
    procs.clear();
    DIR *d = opendir(procdir);
    struct dirent *e;
    while((e = readdir(d)) != 0) {
	if(e->d_name[0] >= '0' && e->d_name[0] <= '9') {
	    Procinfo *pi = new Procinfo(atoi(e->d_name));
	    if(pi->pid == -1)
		delete pi;		// already gone
	    else {
		procs.add(pi);
	    }
	}
    }
    closedir(d);
    // correlate information from old generation
    for(int i = 0; i < procs.size(); i++) {
	Procinfo *ip = procs[i];
	int j, pid = ip->pid;
	if(i < oldprocs.size() && pid == oldprocs[i]->pid)
	    j = i;	 // heuristic to avoid quadratic behaviour
	else {
	    for(j = 0; j < oldprocs.size(); j++)
		if(pid == oldprocs[j]->pid)
		    break;
	}
	if(j < oldprocs.size()) {
	    Procinfo *jp = oldprocs[j];
	    
	    // calculate pcpu and wcpu from previous procinfo
	    int dt = (ip->tv.tv_usec - jp->tv.tv_usec) / (1000000 / CLK_TCK)
		     + (ip->tv.tv_sec - jp->tv.tv_sec) * CLK_TCK;
	    int dcpu = ip->stime - jp->stime + ip->utime - jp->utime;
	    ip->pcpu = 100.0 * dcpu / dt;
	    if(ip->pcpu > 99.99) ip->pcpu = 99.99;

	    const float a = Procview::avg_factor;
	    ip->wcpu = a * jp->wcpu + (1 - a) * ip->pcpu;

	    // propagate some fields to new incarnation
	    ip->selected = jp->selected;
	    ip->details = jp->details;
	    jp->details = 0;
	    if(ip->details)
		ip->details->set_procinfo(ip);
	} else {
	    // %cpu first time = (cpu time since start) / (time since start)
	    int jiffies_since_boot = ip->tv.tv_usec / (1000000 / CLK_TCK)
		                   + (ip->tv.tv_sec - ip->boot_time) * CLK_TCK;
	    int dt = jiffies_since_boot - ip->starttime;
	    int dcpu = ip->stime + ip->utime;
	    ip->pcpu = 100.0 * dcpu / dt;
	    if(dt == 0 || ip->pcpu > 99.99 || ip->pcpu < 0)
		ip->pcpu = 0.0;
	    ip->selected = FALSE;
	    ip->wcpu = ip->pcpu;	// just a start
	}
    }
    // remove useless old Procinfos
    for(int i = 0; i < oldprocs.size(); i++) {
	Procinfo *oi = oldprocs[i];
	if(oi->details)
	    oi->details->process_gone();
	if(!oi->viewed) {
	    delete oi;
	}
    }
}

int Procview::user_fields[] = {F_PID, F_USER, F_PRI, F_NICE, F_SIZE, F_RSS,
			       F_SHARE, F_STAT, F_CPU, F_MEM, F_TIME,
			       F_CMDLINE, F_END};
int Procview::jobs_fields[] = {F_PID, F_PPID, F_PGID, F_SID, F_TTY, F_TPGID,
			       F_STAT, F_UID, F_TIME, F_CMDLINE, F_END};
int Procview::mem_fields[] = {F_PID, F_TTY, F_MAJFLT, F_MINFLT, F_TRS, F_DRS,
			      F_SIZE, F_SWAP, F_RSS, F_SHARE, F_DT, F_CMDLINE,
			      F_END};

float Procview::avg_factor = 1.0;

Procview::Procview(Proc *p)
        : proc(p)
{
    procs = p->procs;
    sortcat = p->cats[F_WCPU];
    reversed = FALSE;
    viewproc = ALL;
    viewfields = USER;
    set_fields();
}

// read new process info
void Procview::refresh()
{
    for(int i = 0; i < old_procs.size(); i++) {
	delete old_procs[i];
    }
    old_procs = procs;
    procs.clear();
    proc->refresh();
    rebuild();
}

// re-sort the process info
void Procview::rebuild()
{
    int my_uid;
    switch(viewproc) {
    case ALL:
	procs = proc->procs;
	break;
    case OWNED:
	my_uid = getuid();
	procs.clear();
	for(int i = 0; i < proc->procs.size(); i++) {
	    Procinfo *p = proc->procs[i];
	    if(my_uid == p->uid)
		procs.add(p);
	    else
		p->selected = 0;
	}
	break;
    case NROOT:
	procs.clear();
	for(int i = 0; i < proc->procs.size(); i++) {
	    Procinfo *p = proc->procs[i];
	    if(p->uid != 0)
		procs.add(p);
	    else
		p->selected = 0;
	}
	break;
    case RUNNING:
	procs.clear();
	for(int i = 0; i < proc->procs.size(); i++) {
	    Procinfo *p = proc->procs[i];
	    if(strchr("RDW", p->state) != 0)
		procs.add(p);
	    else
		p->selected = 0;
	}
	break;
    }
    static_sortcat = sortcat;
    procs.sort(reversed ? compare_backwards : compare);
    for(int i = 0; i < proc->procs.size(); i++)
	proc->procs[i]->viewed = FALSE;
    for(int i = 0; i < procs.size(); i++)
	procs[i]->viewed = TRUE;
}

void Procview::set_fields_list(int fields[])
{
    cats.clear();
    for(int i = 0; fields[i] != F_END; i++)
	cats.add(proc->cats[fields[i]]);
}

void Procview::set_fields()
{
    switch(viewfields) {
    case USER:
	set_fields_list(user_fields);
	break;
    case JOBS:
	set_fields_list(jobs_fields);
	break;
    case MEM:
	set_fields_list(mem_fields);
	break;
    case CUSTOM:
	;
    }
}

// return the column number of a field, or -1 if not displayed
int Procview::findCol(int field)
{
    for(int i = 0; i < cats.size(); i++)
	if(cats[i] == proc->cats[field])
	    return i;
    return -1;
}

// add a category at the right place (no duplication check)
void Procview::add_cat(Category *c)
{
    int i = cats.size() - 1;
    while(i >= 0 && cats[i]->index > c->index) {
	cats[i + 1] = cats[i];
	i--;
    }
    cats[i + 1] = c;
}

void Procview::remove_cat(int index)
{
    cats.remove(index);
}

Category *Procview::static_sortcat = 0;

int Procview::compare(Procinfo **a, Procinfo **b)
{
    int r = static_sortcat->compare(*a, *b);
    return (r == 0) ? ((*a)->pid > (*b)->pid ? 1 : -1) : r;
}

int Procview::compare_backwards(Procinfo **a, Procinfo **b)
{
    int r = static_sortcat->compare(*a, *b);
    return (r == 0) ? ((*a)->pid > (*b)->pid ? 1 : -1) : -r;
}

