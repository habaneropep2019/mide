%define kdeprefix /usr
%define version 1.1
%define kderelease 1
%define sourcedir stable/1.1pre1/distribution/tar/generic/source
%define tag 1.1final

%define compiler egcs
%define qtver  qt >= 1.42

%define kdename  korganizer
Name: %{kdename}
Summary: KOrganizer - Calendar and Scheduling Program for KDE
Version: %{version}
Release: %{kderelease}
Source: ftp://ftp.kde.org:/pub/kde/%{sourcedir}/%{kdename}-%{tag}.tar.gz
Copyright: GPL
Group: X11/KDE/Apps
Buildroot: /var/tmp/korganizer-buildroot
Requires: %{qtver} kdesupport
Prefix: %{kdeprefix}

%description
KOrganizer is a complete calendar and scheduling program for KDE.  It
allows interchange with other calendar applications through the industry
standard vCalendar file format.

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n korganizer-%{version}

%build
export KDEDIR=%{kdeprefix}
make -f Makefile.cvs
CC=%{compiler} ./configure \
	--prefix=%{kdeprefix} \
	--with-install-root=$RPM_BUILD_ROOT \
	--disable-path-check
make CXXFLAGS="$RPM_OPT_FLAGS" KDEDIR=%{kdeprefix}

%install
export KDEDIR=%{kdeprefix}
make install-strip prefix=$RPM_BUILD_ROOT%{kdeprefix}

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > \
        $RPM_BUILD_DIR/file.list.%{kdename}

find . -type f | sed -e 's,^\.,\%attr(-\,root\,root) ,' \
        -e '/\/config\//s|^|%config|' \
        -e '/\/applnk\//s|^|%config|' >> \
        $RPM_BUILD_DIR/file.list.%{kdename}

find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> \
        $RPM_BUILD_DIR/file.list.%{kdename}

%clean
rm -rf $RPM_BUILD_ROOT $RPM_BUILD_DIR/file.list.%{kdename}

%files -f ../file.list.%{kdename}

%changelog
* Sun Jan 24 1999 Preston Brown <pbrown@redhat.com>
* updated for 1.1 KDE release.

* Tue Jan 19 1999 Preston Brown <pbrown@redhat.com>
- updated to today's snapshot, called it '3pgb'.

* Thu Jan 07 1999 Preston Brown <pbrown@redhat.com>
- updates for our own release

* Sun Dec 20 1998 Duncan Haldane <f.d.m.haldane@cwix.com>
- adapted for the KDE-1.1 release

* Fri Dec 18 1998 Preston Brown <pbrown@redhat.com>
- updated file specification to pick up config files as such.








