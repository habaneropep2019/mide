/*----------------------------------------------------------------
 * string <--> Emu parameter conversion
 *----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config.h"
#include "sbk.h"
#include "sflayer.h"


static char *gentypestr[] = {
	"startaddr",	/* 0 */
	"endaddr",	/* 1 */
	"startloop",	/* 2 */
	"endloop",	/* 3 */
	"startCoarse",	/* 4 */
	"modLfoToPitch",	/* 5 */
	"vibLfoToPitch",	/* 6 */
	"modEnvToPitch",	/* 7 */
	"initFilterFc",	/* 8 */
	"initFilterQ",	/* 9 */
	"modLfoToFilterFc",	/* 10 */
	"modEnvToFilterFc",	/* 11 */
	"endCoarse",	/* 12 */
	"modLfoToVol",	/* 13 */
	"unused1",	/* 14 */
	"chorusEffect",	/* 15 */
	"reverbEffect",	/* 16 */
	"pan",	/* 17 */
	"unused2",	/* 18 */
	"unused3",	/* 19 */
	"unused4",	/* 20 */
	"delayModLfo",	/* 21 */
	"freqModLfo",	/* 22 */
	"delayVibLfo",	/* 23 */
	"freqVibLfo",	/* 24 */
	"delayModEnv",	/* 25 */
	"attackModEnv",	/* 26 */
	"holdModEnv",	/* 27 */
	"decayModEnv",	/* 28 */
	"sustModEnv",	/* 29 */
	"relModEnv",	/* 30 */
	"keyToModEnvHold",	/* 31 */
	"keyToModEnvDecay",	/* 32 */
	"delayVolEnv",	/* 33 */
	"attackVolEnv",	/* 34 */
	"holdVolEnv",	/* 35 */
	"decayVolEnv",	/* 36 */
	"sustVolEnv",	/* 37 */
	"relVolEnv",	/* 38 */
	"keyToVolEnvHold",	/* 39 */
	"keyToVolEnvDecay",	/* 40 */
	"instrument",	/* 41 */
	"reserved1",	/* 42 */
	"keyRange",	/* 43 */
	"velRange",	/* 44 */
	"startloopCoarse",	/* 45 */
	"keynum",	/* 46 */
	"velocity",	/* 47 */
	"initAtten",	/* 48 */
	"reserved2",	/* 49 */
	"endloopCoarse",	/* 50 */
	"coarseTune",	/* 51 */
	"fineTune",	/* 52 */
	"sampleID",	/* 53 */
	"sampleModes",	/* 54 */
	"initialPitch", /* 55 */
	"scaleTuning",	/* 56 */
	"exclusiveClass",	/* 57 */
	"overrideRootKey",	/* 58 */
	"unused5",	/* 59 */
	"endOper",	/* 60 */
};


/*
 */

char *get_genstr(int type)
{
	if (type >= 0 && type <= 60)
		return gentypestr[type];
	else
		return "UNKNOWN";
}


void sbk_to_text(char *text, int type, int val, SFInfo *sf)
{
	char *name;

	name = get_genstr(type);
	switch (type) {
	case 43:
	case 44:
		sprintf(text, "%s %d:%d", name, val & 0xff, (val >> 8) & 0xff);
		break;
	case 41:
		sprintf(text, "%s %d:[%s]", name, val, sf->insthdr[val].name);
		break;
	case 53:
		sprintf(text, "%s %d:[%s]", name, val, sf->samplenames[val].name);
		break;
	default:
		sprintf(text, "%s %d", name, val);
		break;
	}
}

/*
 */

char *gettag(char *line)
{
	static char *lastp;
	char *p, *tag;

	if (line)
		lastp = line;

	for (p = lastp; *p && isspace(*p); p++)
		;

	if (*p == 0)
		return NULL;

	if (*p == '[') {
		p++;
		tag = p;
		for (; *p && *p != ']' && *p != '\n'; p++)
			;
	} else {
		tag = p;
		for (; *p && !isspace(*p); p++)
			;
	}
	if (*p)
		lastp = p + 1;
	else
		lastp = p;

	*p = 0;
	return tag;
}

int get_gentype(char *str)
{
	int k;
	for (k = 0; k < PARM_SIZE; k++) {
		if (strcmp(str, gentypestr[k]) == 0)
			return k;
	}
	return -1;
}

