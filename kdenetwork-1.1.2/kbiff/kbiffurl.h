/*
 * kbiffurl.h
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * $Id: kbiffurl.h,v 1.1.4.1 1999/06/04 02:25:11 granroth Exp $
 *
 */
#ifndef KBIFFURL_H
#define KBIFFURL_H

#include <kurl.h>

/**
 * This is a simple extension to the KURL class.  It adds the method
 * 'encodeURLStrict' which will also encode the '/' character.
 */
class KBiffURL : public KURL
{
public:
	/**
	 * Default constructor
	 */
	KBiffURL();

	/**
	 * Construct a KBiffURL object from _url
	 */
	KBiffURL(const QString& _url);

	/**
	 * Escapes some reserved characters within URLs (e.g. '#', '%').
	 *
	 * This is the same as the @ref encodeURL function with the addition
	 * that it encodes the '/' character.
	 */
	static void encodeURLStrict(QString& url);
};
#endif // KBIFFURL_H
