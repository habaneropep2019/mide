/*================================================================
 * print the sbk/sf2 information
 *
 * Copyright (C) 1996,1997 Takashi Iwai
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *================================================================*/

#include <stdio.h>
#include <string.h>
#include "config.h"
#include "sbk.h"
#include "sflayer.h"

static SFInfo sfinfo;

static char *getname(char *p);
static void print_sbk(SFInfo *sf, FILE *fout);


void main(int argc, char **argv)
{
	FILE *fp;

	if (argc < 2) {
		fprintf(stderr, "usage: sf2text SoundFontFile [textfile]\n");
		exit(1);
	}

	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "can't open SoundFont file %s\n", argv[1]);
		exit(1);
	}

	load_sbk(fp, &sfinfo);
	fclose(fp);

	fp = stdout;
	if (argc >= 3 && (fp = fopen(argv[2], "w")) == NULL) {
		fprintf(stderr, "can't open text file %s\n", argv[2]);
		exit(1);
	}
	print_sbk(&sfinfo, fp);

	exit(0);
}


static char *getname(p)
	char *p;
{
	static char buf[21];
	strncpy(buf, p, 20);
	buf[20] = 0;
	return buf;
}


static void print_sbk(SFInfo *sf, FILE *fout)
{
	char buf[1000];
	int i, ninfos;
	tpresethdr *ip;
	tinsthdr *tp;
	tsampleinfo *hp;

	fprintf(fout, "** SoundFont: %d %d\n", sf->version, sf->minorversion);
	fprintf(fout, "** SampleData: %d %d\n", (int)sf->samplepos, (int)sf->samplesize);

	fprintf(fout, "\n** Presets: %d\n", sf->nrpresets-1);
	for (i = 0, ip = sf->presethdr; i < sf->nrpresets-1; ip++, i++) {
		int b;
		fprintf(fout, "[%s] preset=%d bank=%d layer=%d\n",
		       getname(ip->name), ip->preset, ip->bank,
		       ip[1].bagNdx - ip->bagNdx);
		for (b = ip->bagNdx; b < ip[1].bagNdx; b++) {
			int g;
			fprintf(fout, "    BAG %d\n", sf->presetbag[b+1] - sf->presetbag[b]);
			for (g = sf->presetbag[b]; g < sf->presetbag[b+1]; g++) {
				sbk_to_text(buf, sf->presetgen[g].oper,
					    sf->presetgen[g].amount, sf);
				fprintf(fout, "    %s\n", buf);
			}
		}
	}

	fprintf(fout, "\n** Instruments: %d\n", sf->nrinsts-1);
	for (i = 0, tp = sf->insthdr; i < sf->nrinsts-1; tp++, i++) {
		int b;
		fprintf(fout, "[%s] layer=%d\n", getname(tp->name), tp[1].bagNdx - tp->bagNdx);
		for (b = tp->bagNdx; b < tp[1].bagNdx; b++) {
			int g;
			fprintf(fout, "    BAG %d\n", sf->instbag[b+1] - sf->instbag[b]);
			for (g = sf->instbag[b]; g < sf->instbag[b+1]; g++) {
				sbk_to_text(buf, sf->instgen[g].oper,
					    sf->instgen[g].amount, sf);
				fprintf(fout, "    %s\n", buf);
			}
		}
	}

	if (sf->version == 1)
		ninfos = sf->nrinfos;
	else
		ninfos = sf->nrinfos - 1;
	fprintf(fout, "\n** SampleInfo: %d\n", ninfos);
	for (i = 0, hp = sf->sampleinfo; i < ninfos; hp++, i++) {
		fprintf(fout, "[%s] start=%x end=%x startloop=%x endloop=%x\n",
		       sf->samplenames[i].name, hp->startsample, hp->endsample,
		       hp->startloop, hp->endloop);
		if (sf->version > 1) {
			fprintf(fout, "    rate=%d, pitch=%d:%d, link=%d, type=%x\n",
			       hp->samplerate,
			       hp->originalPitch,
			       hp->pitchCorrection,
			       hp->samplelink,
			       hp->sampletype);
		}
	}
}
