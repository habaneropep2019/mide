## -*- makefile -*-
# Ktalkd - Main Makefile.am

if KDE_INSTALLED
SUBDIRS 	=	ktalkd kotalkd doc mail.local kcmktalkd ktalkdlg
else
SUBDIRS 	=	ktalkd kotalkd doc mail.local
endif

EXTRA_DIST	=	ChangeLog TODO ktalkd.lsm

install-data-local:
	@echo "**************************************************************************"
	@echo
	@echo "Don't forget to update /etc/inetd.conf :"
	@echo
	@echo "For example, on a linux system, if kde is in /opt/kde, set it to :"
	@echo "talk	dgram	udp	wait	root	/usr/sbin/tcpd	/opt/kde/bin/kotalkd"
	@echo "ntalk	dgram	udp	wait	root	/usr/sbin/tcpd	/opt/kde/bin/ktalkd"
	@echo	
	@echo "Alternatively, you can use the script post-install.sh, to do the job"
	@echo
	@echo "Anyway, you'll have to restart inetd after this."
	@echo "On most linux system, do a  'killall -HUP inetd'"
	@echo
	@echo "**************************************************************************"

