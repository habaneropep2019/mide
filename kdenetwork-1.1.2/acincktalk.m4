## AC_FIND_USER_METHOD implemented here, 
## to avoid changing acinclude.m4.in (on Kulow's request)
## David.Faure@insa-lyon.fr

AC_DEFUN(AC_FIND_USER_METHOD,
[ 
AC_MSG_CHECKING(ktalkd find_user method)
if test -n "`echo $target_os | grep linux`" ; then
    if test -d /proc; then
       AC_DEFINE(PROC_FIND_USER)

## Sufficient if all xdm and kdm would call sessreg to log the user into utmp
       AC_DEFINE(UTMP_AND_PROC_FIND_USER)

## Waiting for this, here is complement, looking for DISPLAY set in any process
## in /proc that the user owns
       AC_DEFINE(ALL_PROCESSES_AND_PROC_FIND_USER)

       AC_MSG_RESULT(using /proc.)
    else
       AC_MSG_RESULT(/proc not found, using utmp.)
    fi
else
    AC_MSG_RESULT(not a linux system, using utmp.)
fi

])

