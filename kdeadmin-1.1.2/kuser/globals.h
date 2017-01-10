#ifndef _KU_GLOBALS_H_
#define _KU_GLOBALS_H_ "$Id: globals.h,v 1.12.2.5 1999/06/29 06:41:41 garbanzo Exp $"

#define _KU_VERSION "1.0"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kconfig.h>
#include "kerror.h"

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

extern int is_quota;
extern int is_shadow;

#define KU_BACKUP_EXT ".bak"

#define MAIL_SPOOL_DIR "/var/spool/mail"

#ifdef _KU_NIS
  #define SHELL_FILE "/etc/shells"
  #define PASSWORD_FILE "/etc/yppasswd"
  #define PASSWORD_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
  #define GROUP_FILE "/etc/ypgroup"
  #define GROUP_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
  #undef _KU_SHADOW
  #define PWMKDB "cd /var/yp; make 2>&1 >> /var/log/kuser"
  #define GRMKDB "cd /var/yp; make 2>&1 >> /var/log/kuser"
  #define SKELDIR "/etc/skel"
  #define SKEL_FILE_PREFIX ""
#else
  #ifdef __FreeBSD__
    #undef _KU_SHADOW
    #include <pwd.h>
    #include <paths.h>
    #define SHELL_FILE _PATH_SHELLS
    #define PASSWORD_FILE _PATH_MASTERPASSWD
    #define PASSWORD_FILE_MASK S_IRUSR | S_IWUSR
    #define PWMKDB _PATH_PWD_MKDB" -p "PASSWORD_FILE
    #define GROUP_FILE "/etc/group"
    #define GROUP_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
    #define SKELDIR "/usr/share/skel"
    #define SKEL_FILE_PREFIX "dot"
  #else
    #define SHELL_FILE "/etc/shells"
    #define PASSWORD_FILE "/etc/passwd"
    #define PASSWORD_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
    #define GROUP_FILE "/etc/group"
    #define GROUP_FILE_MASK S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
    #define SKELDIR "/etc/skel"
    #define SKEL_FILE_PREFIX ""
  #endif
#endif

extern KConfig *config;
extern KError *err;

#ifdef _KU_SHADOW
#define SHADOW_FILE "/etc/shadow"
#define SHADOW_FILE_MASK S_IRUSR | S_IWUSR
#endif

#ifdef AIX
extern "C" int getuid(void);
extern "C" int unlink(const char *);
#endif

#define KU_HOMEDIR_PERM 0755
#define KU_MAILBOX_PERM 0660

#define KU_MAILBOX_GID 0

#define KU_FIRST_USER 1001

#endif // _KU_GLOBALS_H_
