/*
 * kbiffurl.cpp
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the implementation of KBiffURL and
 *
 * $Id: kbiffurl.cpp,v 1.4.2.1 1999/06/04 02:25:10 granroth Exp $
 */
#include "kbiffurl.h"

KBiffURL::KBiffURL()
	: KURL()
{
}

KBiffURL::KBiffURL(const QString& _url)
	: KURL(_url)
{
}

void KBiffURL::encodeURLStrict(QString& _url)
{
	int old_length = _url.length();

	if (!old_length)
		return;

	// a worst case approximation
	char *new_url = new char[old_length * 3 + 1];
	int new_length = 0;

	for (int i = 0; i < old_length; i++)
	{
		static char *safe = "$-._!*(),"; /* RFC 1738 */

		char t = _url[i];

		if ( (( t >= 'A') && ( t <= 'Z')) ||
				(( t >= 'a') && ( t <= 'z')) ||
				(( t >= '0') && ( t <= '9')) ||
				(strchr(safe, t))
			)
		{
			new_url[new_length++] = _url[i];
		}
		else
		{
			new_url[new_length++] = '%';

			unsigned char c = ((unsigned char)_url[i]) / 16;
			c += (c > 9) ? ('A' - 10) : '0';
			new_url[new_length++] = c;

			c = ((unsigned char)_url[ i ]) % 16;
			c += (c > 9) ? ('A' - 10) : '0';
			new_url[new_length++] = c;
		}
	}

	new_url[new_length]=0;
	_url = new_url;
	delete [] new_url;
}
