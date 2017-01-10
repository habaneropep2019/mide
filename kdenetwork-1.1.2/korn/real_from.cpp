#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <qstring.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#define whitespace(c)    (c == ' ' || c == '\t')

#define skip_white(c)	 while(c && (*c) && whitespace(*c) ) c++
#define skip_nonwhite(c) while(c && (*c) && !whitespace(*c) ) c++

#define skip_token(buf) skip_nonwhite(buf); if(!*buf) return false; \
	skip_white(buf); if(!*buf) return false;

static char *month_name[13] = {
	"jan", "feb", "mar", "apr", "may", "jun",
	"jul", "aug", "sep", "oct", "nov", "dec", NULL
};

static char *day_name[8] = {
	"sun", "mon", "tue", "wed", "thu", "fri", "sat", 0
};

bool realfrom(const char *buffer)
{

	/*
	A valid from line will be in the following format:
	
	From <user> <weekday> <month> <day> <hr:min:sec> [TZ1 [TZ2]] <year>
*/

	int day;
	int i;
	int found;

	/* From */
	
	if(!buffer || !*buffer)
		return false;

	if (strncmp(buffer, "From ", 5))
		return false;

	buffer += 5;

	skip_white(buffer);

	/* <user> */
	if(*buffer == 0) return false;
	skip_token(buffer);

	/* <weekday> */
	found = 0;
	for (i = 0; day_name[i] != NULL; i++)
		found = found || (strnicmp(day_name[i], buffer, 3) == 0);

	if (!found)
		return false;
	
	skip_token(buffer);

	/* <month> */
	found = 0;
	for (i = 0; month_name[i] != NULL; i++)
		found = found || (strnicmp(month_name[i], buffer, 3) == 0);
	if (!found)
		return false;

	skip_token(buffer);

	/* <day> */
	if ( (day = atoi(buffer)) < 0 || day < 1 || day > 31)
		return false;

	return true;
}
