/*================================================================
 * sbk2cfg  --  extracts info from sf2/sbk font and constructs
 *		a TiMidity cfg file.  Greg Lee, lee@hawaii.edu, 5/98.
 * The code is adapted from "sbktext" by Takashi Iwai, which contained
 * the following notice:
 *================================================================
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
		fprintf(stderr, "usage: sbk2cfg SoundFontFile [textfile]\n");
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

	fprintf(fp, "\nsbk %s\n", argv[1]);

	exit(0);
}


static char *getname(p)
	char *p;
{
	int i;
	static char buf[21];
	strncpy(buf, p, 20);
	buf[20] = 0;
	for (i = 0; i < 20; i++) {
	  if (buf[i] == ' ') buf[i] = '_';
	  if (buf[i] == '#') buf[i] = '@';
	}
	return buf;
}


static void print_sbk(SFInfo *sf, FILE *fout)
{
    int i, bank, preset, lastpatch;
    int lastbank = -1;
    tpresethdr *ip;
    tinsthdr *tp;

    fprintf(fout, "#  ** SoundFont: %d %d\n", sf->version, sf->minorversion);
    fprintf(fout, "#  ** SampleData: %d %d\n", (int)sf->samplepos, (int)sf->samplesize);

    fprintf(fout, "#\n#  ** Presets: %d\n", sf->nrpresets-1);

    for (bank = 0; bank <= 128; bank++) {
      for (preset = 0; preset <= 128; preset++) {
      lastpatch = -1;
	for (i = 0, ip = sf->presethdr; i < sf->nrpresets-1; i++) {
	    int b, g, inst, sm_idx;
	    if (ip[i].bank != bank) continue;
	    if (ip[i].preset != preset) continue;
	    inst = -1;
	    for (b = ip[i].bagNdx; b < ip[i+1].bagNdx; b++) {
		for (g = sf->presetbag[b]; g < sf->presetbag[b+1]; g++)
		   if (sf->presetgen[g].oper == SF_instrument) {
			inst = sf->presetgen[g].amount;
			break;
		   }
	    }
	    if (inst < 0) continue;
	    tp = sf->insthdr;
	    if (bank != 128) {
		int realwaves = 0;
		if (bank != lastbank) {
			fprintf(fout, "\nbank %d sbk\n", bank);
			lastbank = bank;
		}
		for (b = tp[inst].bagNdx; b < tp[inst+1].bagNdx; b++) {
		    sm_idx = -1;
		    for (g = sf->instbag[b]; g < sf->instbag[b+1]; g++) {
			if (sf->instgen[g].oper == SF_sampleId) sm_idx = sf->instgen[g].amount;
		    }
		    if (sm_idx >= 0 && sf->sampleinfo[sm_idx].sampletype < 0x8000) realwaves++;
		}
		if (realwaves && ip[i].preset != lastpatch) {
		    fprintf(fout, "\t%3d %s\n", ip[i].preset, getname(ip[i].name));
		    lastpatch = ip[i].preset;
		}
	    }
	    else {
		int keynote, c, dpreset;
		fprintf(fout, "\ndrumset %d sbk\t\t#%s\n", ip[i].preset, getname(ip[i].name));

		for (dpreset = 0; dpreset < 128; dpreset++)
	        for (c = ip[i].bagNdx; c < ip[i+1].bagNdx; c++) {
		  inst = -1;
		  for (g = sf->presetbag[c]; g < sf->presetbag[c+1]; g++)
		   if (sf->presetgen[g].oper == SF_instrument) {
			inst = sf->presetgen[g].amount;
			break;
		   }
		  if (inst >= 0) for (b = tp[inst].bagNdx; b < tp[inst+1].bagNdx; b++) {
		    sm_idx = keynote = -1;
		    for (g = sf->instbag[b]; g < sf->instbag[b+1]; g++) {
			if (sf->instgen[g].oper == SF_sampleId) sm_idx = sf->instgen[g].amount;
			else if (sf->instgen[g].oper == SF_keyRange) keynote = sf->instgen[g].amount & 0xff;
		    }
		    if (sf->sampleinfo[sm_idx].sampletype >= 0x8000) continue;
		    if (keynote != dpreset) continue;
		    if (sm_idx >= 0 && keynote >= 0 && keynote != lastpatch) {
			   fprintf(fout, "\t%3d %s\n", keynote, getname( sf->samplenames[sm_idx].name ));
			   lastpatch = keynote;
		    }
		  }
	        }

	    }
	}
      }
    }
}
