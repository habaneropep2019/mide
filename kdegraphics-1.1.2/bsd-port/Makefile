# New ports collection makefile for:   kdegraphics
# Version required:    1.0
# Date created:        18 June 1998
# Whom:                Alex Zepeda <garbanzo@hooked.net>
#
# $Id: Makefile,v 1.4 1998/06/30 20:06:02 garbanzo Exp $
#
PKGNAME=	kdegraphics-`date "+%y%m%d"`
CATEGORIES=	graphics kde

MAINTAINER=	garbanzo@hooked.net

BUILD_DEPENDS=	moc:${PORTSDIR}/x11/qt \
		autoconf:${PORTSDIR}/devel/autoconf \
		automake:${PORTSDIR}/devel/automake
INSTALL_ROOT=	${.CURDIR}/${PKGNAME}
LIB_DEPENDS=	qt\\.1\\.3[0-9]:${PORTSDIR}/x11/qt133 \
		gif\\.[23]\\.:${PORTSDIR}/graphics/giflib \
		jpeg\\.9\\.:${PORTSDIR}/graphics/jpeg \
		kdecore\\.1\\.[0-9]:${PORTSDIR}/x11/kdelibs \
		kdeui\\.1\\.[0-9]:${PORTSDIR}/x11/kdelibs \
		jscript\\.1\\.[0-9]:${PORTSDIR}/x11/kdelibs \
		khtmlw\\.1\\.[0-9]:${PORTSDIR}/x11/kdelibs \
		kfm\\.1\\.[0-9]:${PORTSDIR}/x11/kdelibs \
		mediatool\\.1\.[0-9]:${PORTSDIR}/x11/kdelibs
NO_MTREE=	yup
USE_GMAKE=	"without a doubt"
NO_WRKDIR=	"Cause, you've obviously got the source in the parent directory"
WRKSRC=		../
NO_EXTRACT=	"yes, please"
HAS_CONFIGURE=	yes
CONFIGURE_ARGS=	"--prefix=$(PREFIX)" \
		"--x-inc=$(X11BASE)/include" \
		"--x-lib=$(X11BASE)/lib" \
		"--with-install-root=$(INSTALL_ROOT)" \
		"--with-extra-includes=$(PREFIX)/include/giflib"

.include "kde-snapshot.mk"
.include <bsd.port.mk>
