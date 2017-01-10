# New ports collection makefile for:   kdesupport
# Version required:    1.0
# Date created:        3 May 1998
# Whom:                Alex Zepeda <garbanzo@hooked.net>
#
# $Id: Makefile,v 1.10 1998/06/24 18:39:55 bieker Exp $
#
PKGNAME=	kdesupport-`date "+%y%m%d"`
CATEGORIES=	misc kde

MAINTAINER=	garbanzo@hooked.net

INSTALL_ROOT=	${.CURDIR}/${PKGNAME}
BUILD_DEPENDS=	moc:${PORTSDIR}/x11/qt \
		autoconf:${PORTSDIR}/devel/autoconf \
		automake:${PORTSDIR}/devel/automake \
		gmake:${PORTSDIR}/devel/gmake
LIB_DEPENDS=	qt\\.1\\.3[0-9]:${PORTSDIR}/x11/qt133 \
		gif\\.[23]\\.:${PORTSDIR}/graphics/giflib \
		jpeg\\.9\\.:${PORTSDIR}/graphics/jpeg

USE_GMAKE=	"without a doubt"
NO_WRKDIR=	"Cause, you've obviously got the source in the parent directory"
WRKSRC=		../
NO_EXTRACT=	"yes, please"
HAS_CONFIGURE=	yes
CONFIGURE_ARGS=	"--prefix=$(PREFIX)" \
		"--enable-shared" \
		"--disable-static" \
		"--without-libgdbm" \
		"--without-libjpeg" \
		"--without-libgif" \
		"--with-mimelib" \
		"--with-libsprite-field"

CONFIGURE_ENV=	CXXFLAGS="$(CFLAGS)" \
		INSTALL_SCRIPT="install -c -m 555"
INSTALL_TARGET=	prefix=$(INSTALL_ROOT)$(PREFIX) install

.include "kde-snapshot.mk"
.include <bsd.port.mk>
