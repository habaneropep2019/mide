# $Id: kde-snapshot.mk,v 1.6 1998/10/01 01:35:46 bieker Exp $

CATEGORIES=	kde
BUILD_DEPENDS=	autoconf:${PORTSDIR}/devel/autoconf \
		automake:${PORTSDIR}/devel/automake
INSTALL_ROOT=	${.CURDIR}/${PKGNAME}
LIB_DEPENDS=	gif.[23]:${PORTSDIR}/graphics/giflib \
		jpeg.9:${PORTSDIR}/graphics/jpeg \
		kdecore.[12]:${PORTSDIR}/x11/kdelibs \
		kdeui.[12]:${PORTSDIR}/x11/kdelibs \
		jscript.[12]:${PORTSDIR}/x11/kdelibs \
		khtmlw.[12]:${PORTSDIR}/x11/kdelibs \
		kfm.[12]:${PORTSDIR}/x11/kdelibs \
NO_MTREE=	yup
USE_GMAKE=	"without a doubt"
NO_WRKDIR=	"Cause, you've obviously got the source in the parent directory"
WRKSRC=		../
NO_EXTRACT=	"yes, please"
GNU_CONFIGURE=	yes
USE_QT=		yes
CONFIGURE_ARGS=	"--prefix=$(PREFIX)" \
		"--x-inc=$(X11BASE)/include" \
		"--x-lib=$(X11BASE)/lib" \
		"--with-install-root=$(INSTALL_ROOT)" \
		"--with-extra-includes=$(PREFIX)/include/giflib"
CONFIGURE_ENV=	CXXFLAGS="$(CFLAGS)" \
		install_root=$(INSTALL_ROOT) \
		INSTALL_SCRIPT="${INSTALL_SCRIPT}"

# Since there's nothing to fetch, we might as well use a dummy target
do-fetch:
		@true
# This should clean the KDE target pretty well
pre-clean:
		cd $(WRKSRC);$(GMAKE) clean   

# We need to go through Makefile.cvs before anything else.
pre-configure:
		cd $(WRKSRC);rm -f config.cache;$(GMAKE) -f Makefile.cvs
		rm -f $(PLIST)
post-install:
		${MAKE} PREFIX=${PREFIX} make-plist
		cp -Rp ${INSTALL_ROOT}/* /
		rm -rf ${INSTALL_ROOT}

# This should finally work somewhat decently now
make-plist:
		cd ${INSTALL_ROOT}/${PREFIX} && \
			find . -type f ! -name .|sed 's,^\./,,'|sort > $(PLIST)
		cd ${INSTALL_ROOT}/${PREFIX} && \
			find . -type l ! -name .|sed 's,^\./,,'|sort >> $(PLIST)
		cd ${INSTALL_ROOT}/${PREFIX} && \
			find . -type d ! -name .|sed 's,^\./,@dirrm ,'|sort -r>> $(PLIST)
		@echo "@exec /usr/bin/env OBJFORMAT=%%PORTOBJFORMAT%% /sbin/ldconfig -m %D/lib" >> PLIST
		@echo "@unexec /usr/bin/env OBJFORMAT=%%PORTOBJFORMAT%% /sbin/ldconfig -R" >> PLIST
