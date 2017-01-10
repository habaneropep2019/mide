/*================================================================
 * SoundFont file extension
 *	written by Takashi Iwai <iwai@dragon.mm.t.u-tokyo.ac.jp>
 *================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "config.h"
#include "common.h"
#include "tables.h"
#include "instrum.h"
#include "playmidi.h"
#include "controls.h"
#include "sbk.h"
#include "sflayer.h"
#include "output.h"
#include "filter.h"
#include "resample.h"

/*----------------------------------------------------------------
 * compile flags
 *----------------------------------------------------------------*/

/*#define GREGSTEST*/

/*#define SF_CLOSE_EACH_FILE*/

/*#define SF_SUPPRESS_TREMOLO*/
#ifdef ADAGIO
#define SF_SUPPRESS_VIBRATO
#endif
/*#define SF_SUPPRESS_CUTOFF*/

/* sounds crappy -- 2mbgmgs.sf2 has them */
/* #define DO_LINKED_WAVES */

/* require cfg declarations of all banks and presets --gl */
#define STRICT_VOICE_DECL
/*----------------------------------------------------------------
 * local parameters
 *----------------------------------------------------------------*/

typedef struct _Layer {
	int16 val[PARM_SIZE];
	int8 set[PARM_SIZE];
} Layer;

typedef struct _SampleList {
	Sample v;
	struct _SampleList *next;
	int32 startsample, endsample;
	int32 cutoff_freq;
	float resonance;
} SampleList;

typedef struct _InstList {
	int bank, preset, keynote;
	int samples, rsamples;
	int order, already_loaded;
	char *fname;
	FILE *fd;
	SampleList *slist, *rslist;
	struct _InstList *next;
} InstList;

typedef struct SFInsts {
	char *fname;
	FILE *fd;
	uint16 version, minorversion;
	int32 samplepos, samplesize;
	InstList *instlist;
} SFInsts;

typedef struct _SFExclude {
	int bank, preset, keynote;
	struct _SFExclude *next;
} SFExclude;

typedef struct _SFOrder {
	int bank, preset, keynote;
	int order;
	struct _SFOrder *next;
} SFOrder;


/*----------------------------------------------------------------*/

/*static void free_sample(InstList *ip);*/
static int load_one_side(SFInsts *rec, SampleList *sp, int sample_count, Sample *base_sample, int amp, int brightness, int harmoniccontent);
static Instrument *load_from_file(SFInsts *rec, InstList *ip, int amp, int brightness, int harmoniccontent);
static int is_excluded(int bank, int preset, int keynote);
static void free_exclude(void);
static int is_ordered(int bank, int preset, int keynote);
static void free_order(void);
static void parse_preset(SFInsts *rec, SFInfo *sf, int preset, int order);
static void parse_gen(Layer *lay, tgenrec *gen);
static void parse_preset_layer(Layer *lay, SFInfo *sf, int idx);
/*static void merge_layer(Layer *dst, Layer *src);*/
static int search_inst(Layer *lay);
static void parse_inst(SFInsts *rec, Layer *pr_lay, SFInfo *sf, int preset, int inst, int order, int which);
static void parse_inst_layer(Layer *lay, SFInfo *sf, int idx);
static int search_sample(Layer *lay);
static void append_layer(Layer *dst, Layer *src, SFInfo *sf);
static void make_inst(SFInsts *rec, Layer *lay, SFInfo *sf, int pr_idx, int in_idx, int order, int which);
static int32 calc_root_pitch(Layer *lay, SFInfo *sf, SampleList *sp);
#ifndef SF_SUPPRESS_ENVELOPE
static void convert_volume_envelope(Layer *lay, SFInfo *sf, SampleList *sp);
#endif
static int32 to_offset(int offset);
static int32 calc_rate(int diff, int time);
static int32 to_msec(Layer *lay, SFInfo *sf, int index);
static float calc_volume(Layer *lay, SFInfo *sf);
static int32 calc_sustain(Layer *lay, SFInfo *sf);
#ifndef SF_SUPPRESS_TREMOLO
static void convert_tremolo(Layer *lay, SFInfo *sf, SampleList *sp);
#endif
#ifndef SF_SUPPRESS_VIBRATO
static void convert_vibrato(Layer *lay, SFInfo *sf, SampleList *sp);
#endif
#ifndef SF_SUPPRESS_CUTOFF
static void do_lowpass(Sample *sp, int32 freq, float resonance);
#endif
static void calc_cutoff(Layer *lay, SFInfo *sf, SampleList *sp);
static void calc_filterQ(Layer *lay, SFInfo *sf, SampleList *sp);

/*----------------------------------------------------------------*/

static SFInsts sfrec;
static SFExclude *sfexclude;
static SFOrder *sforder;

int cutoff_allowed = 0;
int command_cutoff_allowed = 1;


#ifdef GREGSTEST
static char *getname(p)
	char *p;
{
	static char buf[21];
	strncpy(buf, p, 20);
	buf[20] = 0;
	return buf;
}
#endif

static SFInfo sfinfo;

void init_soundfont(char *fname, int order, int oldbank, int newbank)
{
	/*static SFInfo sfinfo;*/
	int i;
	InstList *ip;

	ctl->cmsg(CMSG_INFO, VERB_NOISY, "init soundfonts `%s'", fname);
#ifdef ADAGIO
	play_mode->rate = setting_dsp_rate;  /*output_rate;*/
#endif
	control_ratio = play_mode->rate / CONTROLS_PER_SECOND;
	if(control_ratio<1)
		 control_ratio=1;
	else if (control_ratio > MAX_CONTROL_RATIO)
		 control_ratio=MAX_CONTROL_RATIO;

	if ((sfrec.fd = open_file(fname, 1, OF_VERBOSE)) == NULL) {
		ctl->cmsg(CMSG_ERROR, VERB_NORMAL,
			  "can't open soundfont file %s", fname);
		return;
	}
	sfrec.fname = strcpy((char*)safe_malloc(strlen(fname)+1), fname);
	load_sbk(sfrec.fd, &sfinfo);

	for (i = 0; i < sfinfo.nrpresets; i++) {
		int bank = sfinfo.presethdr[i].bank;
		int preset = sfinfo.presethdr[i].preset;
		if (oldbank != newbank) {
			if (newbank >= 256 && bank == 128 && preset == oldbank) {
				preset = sfinfo.presethdr[i].sub_preset = newbank - 256;
			}
			else if (bank == oldbank) bank = sfinfo.presethdr[i].sub_bank = newbank;
		}
		parse_preset(&sfrec, &sfinfo, i, order);
	}

	/* copy header info */
	sfrec.version = sfinfo.version;
	sfrec.minorversion = sfinfo.minorversion;
	sfrec.samplepos = sfinfo.samplepos;
	sfrec.samplesize = sfinfo.samplesize;

	free_sbk(&sfinfo);

	/* mark instruments as loaded so they won't be loaded again if we're re-called */
	for (ip = sfrec.instlist; ip; ip = ip->next) ip->already_loaded = 1;

#ifdef SF_CLOSE_EACH_FILE
/* mustn't do this: inst's now remember the open fd's --gl
	fclose(sfrec.fd);
	sfrec.fd = NULL;
*/
#endif
}

#if 0
static void free_sample(InstList *ip)
{
	SampleList *sp, *snext;
	for (sp = ip->slist; sp; sp = snext) {
		snext = sp->next;
		free(sp);
	}
	for (sp = ip->rslist; sp; sp = snext) {
		snext = sp->next;
		free(sp);
	}
	free(ip);
}
#endif

void end_soundfont(void)
{
	InstList *ip, *next;
	FILE *still_open = NULL;
	char *still_not_free = NULL;

	while (1) {
	    for (ip = sfrec.instlist; ip; ip = next) {
		next = ip->next;
		if (!still_open && ip->fd) {
		    still_open = ip->fd;
		    still_not_free = ip->fname;
		}
		if (still_open && ip->fd == still_open) ip->fd = NULL;
	    }
	    if (still_open) {
		fclose(still_open);
		still_open = NULL;
		if (still_not_free) free(still_not_free);
	    }
	    else break;
	}

/** freeing samples done by free_instrument in instrum.c
	for (ip = sfrec.instlist; ip; ip = next) {
		next = ip->next;
		free_sample(ip);
	}
**/
	sfrec.instlist = NULL;

	free_exclude();
	free_order();
}

/*----------------------------------------------------------------
 * get converted instrument info and load the wave data from file
 *----------------------------------------------------------------*/

#ifdef ADAGIO
Instrument *load_sbk_patch(int order, int gm_num, int tpgm, int reverb, int main_volume) {
    extern int next_wave_prog;
    char *name;
    int percussion, amp=-1, keynote, strip_loop, strip_envelope, strip_tail, bank, newmode;
#else
Instrument *load_sbk_patch(int order, char *name, int gm_num, int bank, int percussion,
			   int panning, int amp, int keynote,
			   int strip_loop, int strip_envelope,
			   int strip_tail, int brightness, int harmoniccontent) {
#endif
	int preset;
	InstList *ip;
	Instrument *inst = NULL;

    preset = gm_num;
    if (gm_num >= 128) preset -= 128;

#ifdef ADAGIO
    if (!(gus_voice[tpgm].keep & SBK_FLAG)) return(0);
    bank = gus_voice[tpgm].bank & 0x7f;
    newmode = gus_voice[tpgm].modes;
    strip_loop = strip_envelope = strip_tail = amp = -1;
    percussion = (gm_num >= 128);
    name = gus_voice[tpgm].vname;
/*
    if (very_verbose) printf("load_sbk_patch(%s:order=%d,bank=%d,preset=%d,tpgm=%d,reverb=%d,main_volume=%d) keep 0x%02X\n",
	name,order,bank,preset,tpgm,reverb,main_volume, gus_voice[tpgm].keep);
*/
#endif

#ifndef ADAGIO
/* this isn't working */
    if (percussion && drumset[bank]) {
	Instrument *ipx = drumset[bank]->tone[preset].instrument;
	if (ipx && ipx != MAGIC_LOAD_INSTRUMENT) {
printf("but bank %d voice %d is already loaded\n", bank, preset);
		return ipx;
	}
    }
#endif

    if (percussion) {
	keynote = preset;
	preset = bank;
	bank = 128;
    }
    else keynote = -1;

    cutoff_allowed = command_cutoff_allowed;

/*
    if (command_cutoff_allowed) cutoff_allowed = !percussion;
    else cutoff_allowed = 0;
*/

	for (ip = sfrec.instlist; ip; ip = ip->next) {
		if (ip->bank == bank && ip->preset == preset &&
		    (keynote < 0 || keynote == ip->keynote) &&
		    ip->order == order)
			break;
	}
	if (ip && ip->samples) {
		sfrec.fname = ip->fname;
    		ctl->cmsg(CMSG_INFO, VERB_NOISY, "Loading %s%s %s[%d,%d] from %s.",
			(ip->rsamples)? "linked " : "",
			(percussion)? "drum":"instrument", name,
			(percussion)? keynote : preset, (percussion)? preset : bank, sfrec.fname);
		sfrec.fd = ip->fd;
		sfrec.fname = ip->fname;
		inst = load_from_file(&sfrec, ip, amp, brightness, harmoniccontent);
	}
	else if (order) ctl->cmsg(CMSG_INFO, VERB_NORMAL, "Can't find %s %s[%d,%d] in %s.",
			(percussion)? "drum":"instrument", name,
			(percussion)? keynote : preset, (percussion)? preset : bank, sfrec.fname);


#ifdef SF_CLOSE_EACH_FILE
/* mustn't do this --gl
	fclose(sfrec.fd);
	sfrec.fd = NULL;
*/
#endif

#ifdef ADAGIO
	if (inst) {
	    gus_voice[tpgm].loaded |= DSP_MASK;
	    gus_voice[tpgm].prog = next_wave_prog++;
	}
#endif

	return inst;
}


static Instrument *load_from_file(SFInsts *rec, InstList *ip, int amp, int brightness, int harmoniccontent)
{
	Instrument *inst;

/*
	ctl->cmsg(CMSG_INFO, VERB_NOISY, "Loading SF bank %d prg %d note %d from %s",
		  ip->bank, ip->preset, ip->keynote, rec->fname);
*/

	inst = safe_malloc(sizeof(Instrument));
	inst->type = INST_SF2;
	inst->samples = ip->samples;
	inst->sample = safe_malloc(sizeof(Sample)*ip->samples);

	inst->left_samples = inst->samples;
	inst->left_sample = inst->sample;
	inst->right_samples = ip->rsamples;

	if (ip->rsamples) inst->right_sample = safe_malloc(sizeof(Sample)*ip->rsamples);
	else inst->right_sample = 0;

	if (load_one_side(rec, ip->slist, ip->samples, inst->sample, amp, brightness, harmoniccontent) &&
	    load_one_side(rec, ip->rslist, ip->rsamples, inst->right_sample, amp, brightness, harmoniccontent))
		return inst;

	if (inst->right_sample) free(inst->right_sample);
	free(inst->sample);
	free(inst);
	return 0;
}


static int load_one_side(SFInsts *rec, SampleList *sp, int sample_count, Sample *base_sample, int amp,
	int brightness, int harmoniccontent)
{
	int i;

	for (i = 0; i < sample_count && sp; i++, sp = sp->next) {
		Sample *sample = base_sample + i;
#ifndef LITTLE_ENDIAN
		int32 j;
		int16 *tmp, s;
#endif
		memcpy(sample, &sp->v, sizeof(Sample));
		sample->data = safe_malloc(sp->endsample);
		if (fseek(rec->fd, sp->startsample, SEEK_SET)) {
			ctl->cmsg(CMSG_INFO, VERB_NORMAL, "Can't find sample in file!\n");
			return 0;
		}
		if (!fread(sample->data, sp->endsample, 1, rec->fd)) {
			ctl->cmsg(CMSG_INFO, VERB_NORMAL, "Can't read sample from file!\n");
			return 0;
		}
#ifndef LITTLE_ENDIAN
		tmp = (int16*)sample->data;
		for (j = 0; j < sp->endsample/2; j++) {
			s = LE_SHORT(*tmp);
			*tmp++ = s;
		}
#endif

	/* Note: _One_ thing that keeps this from working right is that,
	   apparently, zero-crossing points change so loops give clicks;
	   Another problem is that have to reload voices between songs.
	   The filter really ought to be in resample.c.
	*/
#define SF_SUPPRESS_DYN_CUTOFF
#ifndef SF_SUPPRESS_DYN_CUTOFF
		if (brightness >= 0 && brightness != 64) {
			int32 fq = sp->cutoff_freq;
#ifdef DYN_CUT_DEBUG
printf("cutoff from %ld", fq);
#endif
			if (!fq) fq = 8400;
			fq += (brightness - 64) * (fq / 40);
#ifdef DYN_CUT_DEBUG
printf(" to %ld\n", fq);
#endif
			if (fq < 349) fq = 349;
			else if (fq > 19912) fq = 19912;
			sp->cutoff_freq = fq;

		}
		if (harmoniccontent >= 0 && harmoniccontent != 64) {
#ifdef DYN_CUT_DEBUG
printf("resonance from %f", sp->resonance);
#endif
			sp->resonance = harmoniccontent / 256.0;
#ifdef DYN_CUT_DEBUG
printf(" to %f\n", sp->resonance);
#endif
		}
#else
	if (brightness+harmoniccontent > 1000) printf("gcc shush\n");
#endif
		/* do some filtering if necessary */
#ifndef SF_SUPPRESS_CUTOFF
		if (sp->cutoff_freq > 0 && cutoff_allowed) {
			/* restore the normal value */
			sample->data_length >>= FRACTION_BITS;
    		        if (!i) ctl->cmsg(CMSG_INFO, VERB_NOISY, "cutoff = %ld ; resonance = %g",
				sp->cutoff_freq, sp->resonance);
			do_lowpass(sample, sp->cutoff_freq, sp->resonance);
			/* convert again to the fractional value */
			sample->data_length <<= FRACTION_BITS;
		}
#endif

#ifdef ADJUST_SAMPLE_VOLUMES
      if (amp!=-1)
	sample->volume=(double)(amp) / 100.0;
      else
	{
	  /* Try to determine a volume scaling factor for the sample.
	     This is a very crude adjustment, but things sound more
	     balanced with it. Still, this should be a runtime option. */
	  int32 i=sp->endsample/2;
	  int16 maxamp=0,a;
	  int16 *tmp=(int16 *)sample->data;
	  while (i--)
	    {
	      a=*tmp++;
	      if (a<0) a=-a;
	      if (a>maxamp)
		maxamp=a;
	    }
	  sample->volume=32768.0 / (double)(maxamp);
	  ctl->cmsg(CMSG_INFO, VERB_DEBUG, " * volume comp: %f", sample->volume);
	}
#else
      if (amp!=-1)
	sample->volume=(double)(amp) / 100.0;
      else
	sample->volume=1.0;
#endif


		if (antialiasing_allowed) {
			/* restore the normal value */
			sample->data_length >>= FRACTION_BITS;
			antialiasing(sample, play_mode->rate);
			/* convert again to the fractional value */
			sample->data_length <<= FRACTION_BITS;
		}

		/* resample it if possible */
		if (sample->note_to_use && !(sample->modes & MODES_LOOPING))
			pre_resample(sample);

/*fprintf(stderr,"sample %d, note_to_use %d\n", i, sample->note_to_use);*/
#ifdef LOOKUP_HACK
		/* squash the 16-bit data into 8 bits. */
		{
			uint8 *gulp,*ulp;
			int16 *swp;
			int l = sample->data_length >> FRACTION_BITS;
			gulp = ulp = safe_malloc(l + 1);
			swp = (int16 *)sample->data;
			while (l--)
				*ulp++ = (*swp++ >> 8) & 0xFF;
			free(sample->data);
			sample->data=(sample_t *)gulp;
		}
#endif

/*
printf("loop start %ld, loop end %ld, len %d\n", sample->loop_start>>FRACTION_BITS, sample->loop_end>>FRACTION_BITS,
sample->data_length >> FRACTION_BITS);
*/
/*#define HANNU_CLICK_REMOVAL*/
#ifdef HANNU_CLICK_REMOVAL
    if ((sample->modes & MODES_LOOPING)) {
	int nls = sample->loop_start>>FRACTION_BITS;
	int nle = sample->loop_end>>FRACTION_BITS;
	int ipt = 0, ips = 0;
	char v1, v2;
	while (!ipt) {
		v1 = sample->data[nle-1];  v2 = sample->data[nle];
		if (v2==0) {
			if (v1 < 0) ipt = 1;
			else if (v2 > 0) ipt = 2;
		}
		else {
			if (v1 <= 0 && v2 > 0) ipt = 1;
			else if (v1 >= 0 && v2 < 0) ipt = 2;
		}
		if (!ipt) nle--;
		if (nle <= sample->loop_start) break;
	}
	if (ipt && nls > 0) while (!ips) {
		v1 = sample->data[nls-1];  v2 = sample->data[nls];
		if (v2==0) {
			if (ipt == 1 && v1 < 0) ips = 1;
			else if (ipt == 2 && v2 > 0) ips = 2;
		}
		else {
			if (ipt == 1 && v1 <= 0 && v2 > 0) ips = 1;
			else if (ipt == 2 && v1 >= 0 && v2 < 0) ips = 2;
		}
		if (!ips) nls--;
		if (nls < 1) break;
	}
	if (ipt && ips && ipt == ips && nle <= sample->loop_end) {
/*
printf("changing loop start from %ld to %d, loop end from %ld to %d\n",
sample->loop_start>>FRACTION_BITS, nls,
sample->loop_end>>FRACTION_BITS, nle);
*/
		sample->loop_start = nls<<FRACTION_BITS;
		sample->loop_end = nle<<FRACTION_BITS;
	}
    }
#endif
	}
	return 1;
}


/*----------------------------------------------------------------
 * excluded samples
 *----------------------------------------------------------------*/

void exclude_soundfont(int bank, int preset, int keynote)
{
	SFExclude *rec;
	rec = (SFExclude*)safe_malloc(sizeof(SFExclude));
	rec->bank = bank;
	rec->preset = preset;
	rec->keynote = keynote;
	rec->next = sfexclude;
	sfexclude = rec;
}

/* check the instrument is specified to be excluded */
static int is_excluded(int bank, int preset, int keynote)
{
	SFExclude *p;
	for (p = sfexclude; p; p = p->next) {
		if (p->bank == bank &&
		    (p->preset < 0 || p->preset == preset) &&
		    (p->keynote < 0 || p->keynote == keynote))
			return 1;
	}
	return 0;
}

/* free exclude list */
static void free_exclude(void)
{
	SFExclude *p, *next;
	for (p = sfexclude; p; p = next) {
		next = p->next;
		free(p);
	}
	sfexclude = NULL;
}


/*----------------------------------------------------------------
 * ordered samples
 *----------------------------------------------------------------*/

void order_soundfont(int bank, int preset, int keynote, int order)
{
	SFOrder *rec;
	rec = (SFOrder*)safe_malloc(sizeof(SFOrder));
	rec->bank = bank;
	rec->preset = preset;
	rec->keynote = keynote;
	rec->order = order;
	rec->next = sforder;
	sforder = rec;
}

/* check the instrument is specified to be ordered */
static int is_ordered(int bank, int preset, int keynote)
{
	SFOrder *p;
	for (p = sforder; p; p = p->next) {
		if (p->bank == bank &&
		    (p->preset < 0 || p->preset == preset) &&
		    (p->keynote < 0 || p->keynote == keynote))
			return p->order;
	}
	return -1;
}

/* free order list */
static void free_order(void)
{
	SFOrder *p, *next;
	for (p = sforder; p; p = next) {
		next = p->next;
		free(p);
	}
	sforder = NULL;
}

/*----------------------------------------------------------------
 * parse a preset
 *----------------------------------------------------------------*/

static void parse_preset(SFInsts *rec, SFInfo *sf, int preset, int order)
{
	int from_ndx, to_ndx;
	Layer lay, glay;
	int i, inst, inum;

	from_ndx = sf->presethdr[preset].bagNdx;
	to_ndx = sf->presethdr[preset+1].bagNdx;

#ifdef GREGSTEST
if (to_ndx - from_ndx > 1) {
printf("Preset #%d (%s) has %d instruments.\n", preset,
 getname(sf->presethdr[preset].name), to_ndx - from_ndx);
}
#endif

	memset(&glay, 0, sizeof(glay));
	for (i = from_ndx, inum = 0; i < to_ndx; i++) {
		memset(&lay, 0, sizeof(Layer));
		parse_preset_layer(&lay, sf, i);
		inst = search_inst(&lay);
		if (inst < 0) /* global layer */
			memcpy(&glay, &lay, sizeof(Layer));
		else {
			append_layer(&lay, &glay, sf);
			parse_inst(rec, &lay, sf, preset, inst, order, inum);
			inum++;
		}
	}
}

/* map a generator operation to the layer structure */
static void parse_gen(Layer *lay, tgenrec *gen)
{
	lay->set[gen->oper] = 1;
	lay->val[gen->oper] = gen->amount;
}

/* parse preset generator layers */
static void parse_preset_layer(Layer *lay, SFInfo *sf, int idx)
{
	int i;
	for (i = sf->presetbag[idx]; i < sf->presetbag[idx+1]; i++)
		parse_gen(lay, sf->presetgen + i);
}

#if 0
/* merge two layers; never overrides on the destination */
static void merge_layer(Layer *dst, Layer *src)
{
	int i;
	for (i = 0; i < PARM_SIZE; i++) {
		if (src->set[i] && !dst->set[i]) {
			dst->val[i] = src->val[i];
			dst->set[i] = 1;
		}
	}
}
#endif

/* search instrument id from the layer */
static int search_inst(Layer *lay)
{
	if (lay->set[SF_instrument])
		return lay->val[SF_instrument];
	else
		return -1;
}

/* parse an instrument */
static void parse_inst(SFInsts *rec, Layer *pr_lay, SFInfo *sf, int preset, int inst, int order, int which)
{
	int from_ndx, to_ndx;
	int i, sample;
	Layer lay, glay;

	from_ndx = sf->insthdr[inst].bagNdx;
	to_ndx = sf->insthdr[inst+1].bagNdx;

	memcpy(&glay, pr_lay, sizeof(Layer));
	for (i = from_ndx; i < to_ndx; i++) {
		memset(&lay, 0, sizeof(Layer));
		parse_inst_layer(&lay, sf, i);
		sample = search_sample(&lay);
		if (sample < 0) /* global layer */
			append_layer(&glay, &lay, sf);
		else {
			append_layer(&lay, &glay, sf);
			make_inst(rec, &lay, sf, preset, inst, order, which);
		}
	}
}

/* parse instrument generator layers */
static void parse_inst_layer(Layer *lay, SFInfo *sf, int idx)
{
	int i;
	for (i = sf->instbag[idx]; i < sf->instbag[idx+1]; i++)
		parse_gen(lay, sf->instgen + i);
}

/* search a sample id from instrument layers */
static int search_sample(Layer *lay)
{
	if (lay->set[SF_sampleId])
		return lay->val[SF_sampleId];
	else
		return -1;
}


/* two (high/low) 8 bit values in 16 bit parameter */
#define LO_VAL(val)	((val) & 0xff)
#define HI_VAL(val)	(((val) >> 8) & 0xff)
#define SET_LO(vp,val)	((vp) = ((vp) & 0xff00) | (val))
#define SET_HI(vp,val)	((vp) = ((vp) & 0xff) | ((val) << 8))

/* append two layers; parameters are added to the original value */
static void append_layer(Layer *dst, Layer *src, SFInfo *sf)
{
	int i;
	for (i = 0; i < PARM_SIZE; i++) {
		if (src->set[i]) {
			if (sf->version == 1 && i == SF_instVol)
				dst->val[i] = (src->val[i] * 127) / 127;
			else if (i == SF_keyRange || i == SF_velRange) {
				/* high limit */
				if (HI_VAL(dst->val[i]) > HI_VAL(src->val[i]))
					SET_HI(dst->val[i], HI_VAL(src->val[i]));
				/* low limit */
				if (LO_VAL(dst->val[i]) < LO_VAL(src->val[i]))
					SET_LO(dst->val[i], LO_VAL(src->val[i]));
			} else
				dst->val[i] += src->val[i];
			dst->set[i] = 1;
		}
	}
}

/* convert layer info to timidity instrument strucutre */
static void make_inst(SFInsts *rec, Layer *lay, SFInfo *sf, int pr_idx, int in_idx, int order, int which)
{
	int banknum = sf->presethdr[pr_idx].bank;
	int preset = sf->presethdr[pr_idx].preset;
	int sub_banknum = sf->presethdr[pr_idx].sub_bank;
	int sub_preset = sf->presethdr[pr_idx].sub_preset;
	int keynote, n_order, program, truebank, note_to_use;
	int strip_loop = 0, strip_envelope = 0, strip_tail = 0, panning = 0;
#ifndef ADAGIO
	ToneBank *bank=0;
	char **namep;
#endif
	InstList *ip;
	tsampleinfo *sample;
	SampleList *sp;
#ifdef DO_LINKED_WAVES
	int linked_wave;
#endif

	if (banknum == 128) which = 0;
	/* more than 2 notes in chord?  sorry -- no can do */
	if (which > 1) return;

	sample = &sf->sampleinfo[lay->val[SF_sampleId]];
#ifdef DO_LINKED_WAVES
	linked_wave = sample->samplelink;
	if (linked_wave && linked_wave < sfinfo.nrinfos) {
		if (which == -1) sample = &sf->sampleinfo[linked_wave - 1];
		else make_inst(rec, lay, sf, pr_idx, in_idx, order, -1);
	}
#endif
	if (sample->sampletype & 0x8000) /* is ROM sample? */
		return;

	/* set bank/preset name */
	if (banknum == 128) {
		truebank = sub_preset;
		program = keynote = LO_VAL(lay->val[SF_keyRange]);
#ifndef ADAGIO
#ifndef STRICT_VOICE_DECL
		if (!drumset[preset]) {
			int i;
			drumset[preset] = safe_malloc(sizeof(ToneBank));
			memset(drumset[preset], 0, sizeof(ToneBank));
			bank = drumset[preset];
			for (i = 0; i < 128; i++) {
			    bank->tone[i].font_type = FONT_SBK;
  			    bank->tone[i].note=bank->tone[i].amp=bank->tone[i].pan=
	  			bank->tone[i].strip_loop=bank->tone[i].strip_envelope=
	    			bank->tone[i].strip_tail=-1;
			}
		}
#endif
		if (drumset[truebank]) {
			bank = drumset[truebank];
		}
#endif
	} else {
		keynote = -1;
		truebank = sub_banknum;
		program = preset;
#ifndef ADAGIO
#ifndef STRICT_VOICE_DECL
		if (!tonebank[banknum]) {
			int i;
			tonebank[banknum] = safe_malloc(sizeof(ToneBank));
			memset(tonebank[banknum], 0, sizeof(ToneBank));
			bank = tonebank[banknum];
			for (i = 0; i < 128; i++) {
			    bank->tone[i].font_type = FONT_SBK;
  			    bank->tone[i].note=bank->tone[i].amp=bank->tone[i].pan=
	  			bank->tone[i].strip_loop=bank->tone[i].strip_envelope=
	    			bank->tone[i].strip_tail=-1;
			}
		}
#endif
		if (tonebank[truebank]) {
			bank = tonebank[truebank];
		}
#endif
	}

#ifdef GREGSTEST
printf("Adding keynote #%d preset %d to %s (%d), bank %d (=? %d).\n", keynote, preset,
 getname(sf->insthdr[in_idx].name), in_idx, banknum, truebank);
#endif


#ifndef ADAGIO
	if (!bank) return;
	namep = &bank->tone[program].name;
#ifdef GREGSTEST
if (*namep) printf("cfg name is %s\n", *namep);
else printf("NO CFG NAME!\n");
#endif
#ifdef STRICT_VOICE_DECL
	if (*namep == 0) return;
#endif

#ifdef STRICT_VOICE_DECL
	if (bank->tone[program].font_type != FONT_SBK) return;
#else
	if (truebank && bank->tone[program].font_type != FONT_SBK) return;
#endif
	panning = bank->tone[program].pan;
	strip_loop = bank->tone[program].strip_loop;
	strip_envelope = bank->tone[program].strip_envelope;
	strip_tail = bank->tone[program].strip_tail;
	note_to_use = bank->tone[program].note;
	/*if (!strip_envelope) strip_envelope = (banknum == 128);*/
#endif

	if (is_excluded(banknum, preset, keynote))
		return;
	if ((n_order = is_ordered(banknum, preset, keynote)) >= 0)
		order = n_order;

#ifndef ADAGIO
#if 0
	if (*namep == NULL || strlen(*namep) < 2) {
		if (*namep) free(*namep);
		*namep = safe_malloc(21);
		if (banknum == 128)
			memcpy(*namep, sf->samplenames[lay->val[SF_sampleId]].name, 20);
		else memcpy(*namep, sf->insthdr[in_idx].name, 20);
		(*namep)[20] = 0;
	}
#endif
#endif

	/* search current instrument list */
	for (ip = rec->instlist; ip; ip = ip->next) {
		if (ip->bank == sub_banknum && ip->preset == sub_preset &&
		    (keynote < 0 || keynote == ip->keynote))
			break;
	}
	/* don't append sample when instrument completely specified */
	if (ip && ip->already_loaded) return;
	if (ip == NULL) {
		ip = (InstList*)safe_malloc(sizeof(InstList));
		ip->bank = sub_banknum;
		ip->preset = sub_preset;
		ip->keynote = keynote;
		ip->order = order;
		ip->already_loaded = 0;
		ip->samples = 0;
		ip->rsamples = 0;
		ip->slist = NULL;
		ip->rslist = NULL;
		ip->fd = rec->fd;
		ip->fname = rec->fname;
		ip->next = rec->instlist;
		rec->instlist = ip;
	}

	/* add a sample */
	sp = (SampleList*)safe_malloc(sizeof(SampleList));
	if (!which) {
		sp->next = ip->slist;
		ip->slist = sp;
		ip->samples++;
	}
	else {
		sp->next = ip->rslist;
		ip->rslist = sp;
		ip->rsamples++;
	}

	/* set sample position */
	sp->startsample = ((short)lay->val[SF_startAddrsHi] * 32768)
		+ (short)lay->val[SF_startAddrs]
		+ sample->startsample;
	sp->endsample = ((short)lay->val[SF_endAddrsHi] * 32768)
		+ (short)lay->val[SF_endAddrs]
		+ sample->endsample - sp->startsample;


	/* set loop position */
	sp->v.loop_start = ((short)lay->val[SF_startloopAddrsHi] * 32768)
		+ (short)lay->val[SF_startloopAddrs]
		+ sample->startloop - sp->startsample;
	sp->v.loop_end = ((short)lay->val[SF_endloopAddrsHi] * 32768)
		+ (short)lay->val[SF_endloopAddrs]
		+ sample->endloop - sp->startsample;
	sp->v.data_length = sp->endsample;
#if 0
/* debug loop point calculation */
{
	int32 xloop_start = sample->startloop - sp->startsample;
	int32 xloop_end = sample->endloop - sp->startsample;
	int yloop_start = (lay->val[SF_startloopAddrsHi] << 15) + lay->val[SF_startloopAddrs];
	int yloop_end = (lay->val[SF_endloopAddrsHi] << 15) + lay->val[SF_endloopAddrs];
	xloop_start += yloop_start;
	xloop_end += yloop_end;
	if (xloop_start > xloop_end) {
	fprintf(stderr,"start %ld > end %ld\n", xloop_start, xloop_end);
	fprintf(stderr,"\tstart %ld = orig %lu + yloop_start %d\n", xloop_start,
		sp->v.loop_start, yloop_start);
	fprintf(stderr,"\t\tyloop_start 0x%x = high 0x%x + low 0x%x\n", yloop_start,
		(lay->val[SF_startloopAddrsHi] << 15), lay->val[SF_startloopAddrs]);
	fprintf(stderr,"\t\tend %ld = orig %lu + yloop_end %d\n", xloop_end,
		sp->v.loop_end, yloop_end);
	fprintf(stderr,"\tlen %lu, startsample %lu sample startloop %lu endsample %lu\n",
		sample->endsample - sample->startsample,
		sample->startsample, sample->startloop, sample->endsample);
	}
	if (xloop_end > sp->v.data_length) {
	fprintf(stderr,"end %ld > length %lu\n", xloop_end, sp->v.data_length);
	fprintf(stderr,"\tend %ld = orig %lu + yloop_end %d\n", xloop_end,
		sp->v.loop_end, yloop_end);
	fprintf(stderr,"\t\tyloop_end 0x%x = high 0x%x + low 0x%x\n", yloop_end,
		(lay->val[SF_endloopAddrsHi] << 15), lay->val[SF_endloopAddrs]);
	fprintf(stderr,"\tlen %lu, startsample %lu sample endloop %lu endsample %lu\n",
		sample->endsample - sample->startsample,
		sample->startsample, sample->endloop, sample->endsample);
	fprintf(stderr,"\t\tstart offset = high 0x%x + low 0x%x\n",
		(lay->val[SF_startAddrsHi] << 15), lay->val[SF_startAddrs]);
	fprintf(stderr,"\t\tend offset = high 0x%x + low 0x%x\n",
		(lay->val[SF_endAddrsHi] << 15), lay->val[SF_endAddrs]);
	}
}
#endif

	if (sp->v.loop_start < 0) {
		ctl->cmsg(CMSG_INFO, VERB_DEBUG, " - Negative loop pointer: removing loop");
		strip_loop = 1;
	}
	if (sp->v.loop_start > sp->v.loop_end) {
		ctl->cmsg(CMSG_INFO, VERB_DEBUG, " - Illegal loop position: removing loop");
		strip_loop = 1;
	}
	if (sp->v.loop_end > sp->v.data_length) {
		ctl->cmsg(CMSG_INFO, VERB_DEBUG, " - Illegal loop end or data size: adjusting loop");
		if (sp->v.loop_end - sp->v.data_length > 4)
			sp->v.loop_end = sp->v.data_length - 4;
		else sp->v.loop_end = sp->v.data_length;
	}
#if 0
if (strip_loop == 1) {
	fprintf(stderr, "orig start sample %lu, endsample %lu\n", sample->startsample, sample->endsample);
	fprintf(stderr, "SF_startAddr %d, SF_endAddr %d\n",
		((short)lay->val[SF_startAddrsHi] * 32768) + (short)lay->val[SF_startAddrs],
		((short)lay->val[SF_endAddrsHi] * 32768) + (short)lay->val[SF_endAddrs] );
	fprintf(stderr, "start sample %lu, length %lu\n", sp->startsample, sp->endsample);
	fprintf(stderr, "orig loop start %lu, loop end %lu\n", sample->startloop, sample->endloop);
	fprintf(stderr, "SF_startloopAddr %d, SF_endloopAddr %d\n",
		((short)lay->val[SF_startloopAddrsHi] * 32768) + (short)lay->val[SF_startloopAddrs],
		((short)lay->val[SF_endloopAddrsHi] * 32768) + (short)lay->val[SF_endloopAddrs] );
	fprintf(stderr, "loop start %lu, loop end %lu\n", sp->v.loop_start, sp->v.loop_end);
}
#endif

	sp->v.sample_rate = sample->samplerate;
	if (lay->set[SF_keyRange]) {
		sp->v.low_freq = freq_table[LO_VAL(lay->val[SF_keyRange])];
		sp->v.high_freq = freq_table[HI_VAL(lay->val[SF_keyRange])];
	} else {
		sp->v.low_freq = freq_table[0];
		sp->v.high_freq = freq_table[127];
	}

	/* scale tuning: 0  - 100 */
	sp->v.scale_tuning = 100;
	if (lay->set[SF_scaleTuning]) {
		if (sf->version == 1)
			sp->v.scale_tuning = lay->val[SF_scaleTuning] ? 50 : 100;
		else
			sp->v.scale_tuning = lay->val[SF_scaleTuning];
	}
/** --gl **/
	sp->v.freq_scale = 1024;
	sp->v.freq_center = 60;
	sp->v.attenuation = 0;
/**  **/

	/* root pitch */
	sp->v.root_freq = calc_root_pitch(lay, sf, sp);

	sp->v.modes = MODES_16BIT;

	/* volume envelope & total volume */
	sp->v.volume = calc_volume(lay,sf);
	if (lay->val[SF_sampleFlags] == 1 || lay->val[SF_sampleFlags] == 3) {
		sp->v.modes |= MODES_LOOPING|MODES_SUSTAIN;
#ifndef SF_SUPPRESS_ENVELOPE
		convert_volume_envelope(lay, sf, sp);
#endif
		if (lay->val[SF_sampleFlags] == 3)
			/* strip the tail */
			sp->v.data_length = sp->v.loop_end + 1;
	}
	if (strip_tail == 1) sp->v.data_length = sp->v.loop_end + 1;

	/*if (banknum == 128) sp->v.modes |= MODES_FAST_RELEASE;*/

      /* Strip any loops and envelopes we're permitted to */
      if ((strip_loop==1) && 
	  (sp->v.modes & (MODES_SUSTAIN | MODES_LOOPING | 
			MODES_PINGPONG | MODES_REVERSE)))
	{
	  ctl->cmsg(CMSG_INFO, VERB_DEBUG, " - Removing loop and/or sustain");
	  sp->v.modes &=~(MODES_SUSTAIN | MODES_LOOPING | 
			MODES_PINGPONG | MODES_REVERSE);
	}
      if (strip_envelope==1)
	{
	  if (sp->v.modes & MODES_ENVELOPE)
	    ctl->cmsg(CMSG_INFO, VERB_DEBUG, " - Removing envelope");
	  sp->v.modes &= ~MODES_ENVELOPE;
	}


	/* panning position: 0 to 127 */
	if (panning != -1) sp->v.panning=(uint8)(panning & 0x7F);
	else if (lay->set[SF_panEffectsSend]) {
		if (sf->version == 1)
			sp->v.panning = (int8)lay->val[SF_panEffectsSend];
		else
			sp->v.panning = (int8)(((int)lay->val[SF_panEffectsSend] + 500) * 127 / 1000);
	}
	else sp->v.panning = 64;

	if (lay->set[SF_chorusEffectsSend]) {
		if (sf->version == 1)
			sp->v.chorusdepth = (int8)lay->val[SF_chorusEffectsSend];
		else
			sp->v.chorusdepth = (int8)((int)lay->val[SF_chorusEffectsSend] * 127 / 1000);
	}
	else sp->v.chorusdepth = 0;

	if (lay->set[SF_reverbEffectsSend]) {
		if (sf->version == 1)
			sp->v.reverberation = (int8)lay->val[SF_reverbEffectsSend];
		else
			sp->v.reverberation = (int8)((int)lay->val[SF_reverbEffectsSend] * 127 / 1000);
	}
	else sp->v.reverberation = 0;

	/* tremolo & vibrato */
	sp->v.tremolo_sweep_increment = 0;
	sp->v.tremolo_phase_increment = 0;
	sp->v.tremolo_depth = 0;
#ifndef SF_SUPPRESS_TREMOLO
	convert_tremolo(lay, sf, sp);
#endif
	sp->v.vibrato_sweep_increment = 0;
	sp->v.vibrato_control_ratio = 0;
	sp->v.vibrato_depth = 0;
#ifndef SF_SUPPRESS_VIBRATO
	convert_vibrato(lay, sf, sp);
#endif

	/* set note to use for drum voices */
	if (note_to_use!=-1)
		sp->v.note_to_use = (uint8)note_to_use;
	else if (banknum == 128)
		sp->v.note_to_use = keynote;
	else
		sp->v.note_to_use = 0;

	/* convert to fractional samples */
	sp->v.data_length <<= FRACTION_BITS;
	sp->v.loop_start <<= FRACTION_BITS;
	sp->v.loop_end <<= FRACTION_BITS;

	/* point to the file position */
	sp->startsample = sp->startsample * 2 + sf->samplepos;
	sp->endsample *= 2;

	/* set cutoff frequency */
	if (lay->set[SF_initialFilterFc] || lay->set[SF_env1ToFilterFc])
		calc_cutoff(lay, sf, sp);
	else sp->cutoff_freq = 0;
	if (lay->set[SF_initialFilterQ])
		calc_filterQ(lay, sf, sp);
	sp->v.cutoff_freq = sp->cutoff_freq;
	sp->v.resonance = sp->resonance;
}

/* calculate root pitch */
static int32 calc_root_pitch(Layer *lay, SFInfo *sf, SampleList *sp)
{
	int32 root, tune;
	tsampleinfo *sample;

	sample = &sf->sampleinfo[lay->val[SF_sampleId]];

	root = sample->originalPitch;
	tune = sample->pitchCorrection;
	if (sf->version == 1) {
		if (lay->set[SF_samplePitch]) {
			root = lay->val[SF_samplePitch] / 100;
			tune = -lay->val[SF_samplePitch] % 100;
			if (tune <= -50) {
				root++;
				tune = 100 + tune;
			}
			if (sp->v.scale_tuning == 50)
				tune /= 2;
		}
		/* orverride root key */
		if (lay->set[SF_rootKey])
			root += lay->val[SF_rootKey] - 60;
		/* tuning */
		tune += lay->val[SF_coarseTune] * sp->v.scale_tuning +
			lay->val[SF_fineTune] * sp->v.scale_tuning / 100;
	} else {
		/* orverride root key */
		if (lay->set[SF_rootKey])
			root = lay->val[SF_rootKey];
		/* tuning */
		tune += lay->val[SF_coarseTune] * 100
			+ lay->val[SF_fineTune];
	}
	/* it's too high.. */
	if (lay->set[SF_keyRange] &&
	    root >= HI_VAL(lay->val[SF_keyRange]) + 60)
		root -= 60;

	while (tune <= -100) {
		root++;
		tune += 100;
	}
	while (tune > 0) {
		root--;
		tune -= 100;
	}
	return (int32)((double)freq_table[root] * bend_fine[(-tune*255)/100]);
}

/*#define EXAMINE_SOME_ENVELOPES*/
/*----------------------------------------------------------------
 * convert volume envelope
 *----------------------------------------------------------------*/

#ifndef SF_SUPPRESS_ENVELOPE
static void convert_volume_envelope(Layer *lay, SFInfo *sf, SampleList *sp)
{
	int32 sustain = calc_sustain(lay, sf);
	/*int delay = to_msec(lay, sf, SF_delayEnv2);*/
	int32 attack = to_msec(lay, sf, SF_attackEnv2);
	int32 hold = to_msec(lay, sf, SF_holdEnv2);
	int32 decay = to_msec(lay, sf, SF_decayEnv2);
	int32 release = to_msec(lay, sf, SF_releaseEnv2);
#ifdef EXAMINE_SOME_ENVELOPES
	static int no_shown = 0;
if (!no_shown) {
	printf("sustainEnv2 %d delayEnv2 %d attackEnv2 %d holdEnv2 %d decayEnv2 %d releaseEnv2 %d\n",
	lay->val[SF_sustainEnv2],
	lay->val[SF_delayEnv2],
	lay->val[SF_attackEnv2],
	lay->val[SF_holdEnv2],
	lay->val[SF_decayEnv2],
	lay->val[SF_releaseEnv2] );
	printf("play_mode->rate = %u; control_ratio = %u\n\n", play_mode->rate, control_ratio);
	printf("sustain %ld attack %ld hold %ld decay %ld release %ld\n", sustain, attack, hold, decay, release);
}
#endif

	sp->v.envelope_offset[0] = to_offset(255);
if (fast_decay)
	sp->v.envelope_rate[0] = calc_rate(255, attack) * 2;
else
	sp->v.envelope_rate[0] = calc_rate(255, attack) * 4;

	sp->v.envelope_offset[1] = to_offset(250);
	sp->v.envelope_rate[1] = calc_rate(5, hold);
	sp->v.envelope_offset[2] = to_offset(sustain);
	sp->v.envelope_rate[2] = calc_rate(250 - sustain, decay);
	sp->v.envelope_offset[3] = to_offset(5);
	sp->v.envelope_rate[3] = calc_rate(255, release);
	sp->v.envelope_offset[4] = to_offset(4);
	sp->v.envelope_rate[4] = to_offset(200);
	sp->v.envelope_offset[5] = to_offset(4);
	sp->v.envelope_rate[5] = to_offset(200);

	sp->v.modes |= MODES_ENVELOPE;
#ifdef EXAMINE_SOME_ENVELOPES
if (no_shown < 6) {
	no_shown++;
	printf(" attack: off %ld  rate %ld\n",
		sp->v.envelope_offset[0], sp->v.envelope_rate[0]);
	printf("   hold: off %ld  rate %ld\n",
		sp->v.envelope_offset[1], sp->v.envelope_rate[1]);
	printf("sustain: off %ld  rate %ld\n",
		sp->v.envelope_offset[2], sp->v.envelope_rate[2]);
	printf("release: off %ld  rate %ld\n",
		sp->v.envelope_offset[3], sp->v.envelope_rate[3]);
	printf("  decay: off %ld  rate %ld\n",
		sp->v.envelope_offset[4], sp->v.envelope_rate[4]);
	printf("    die: off %ld  rate %ld\n",
		sp->v.envelope_offset[5], sp->v.envelope_rate[5]);
}
#endif
}
#endif

/* convert from 8bit value to fractional offset (15.15) */
static int32 to_offset(int offset)
{
	return (int32)offset << (7+15);
}

/* calculate ramp rate in fractional unit;
 * diff = 8bit, time = msec
 */
static int32 calc_rate(int diff, int time)
{
	int32 rate;

	if (time < 6) time = 6;
	if (diff == 0) diff = 255;
	diff <<= (7+15);
	rate = (diff / play_mode->rate) * control_ratio;
	rate = rate * 1000 / time;
	if (fast_decay) rate *= 2;

	return rate;
}


#define TO_MSEC(tcents) (int32)(1000 * pow(2.0, (double)(tcents) / 1200.0))
#define TO_MHZ(abscents) (int32)(8176.0 * pow(2.0,(double)(abscents)/1200.0))
#define TO_HZ(abscents) (int32)(8.176 * pow(2.0,(double)(abscents)/1200.0))
#define TO_LINEAR(centibel) pow(10.0, -(double)(centibel)/200.0)
#define TO_VOLUME(centibel) (uint8)(255 * (1.0 - (centibel) / (1200.0 * log10(2.0))));

/* convert the value to milisecs */
static int32 to_msec(Layer *lay, SFInfo *sf, int index)
{
	int16 value;
	if (! lay->set[index])
		return 6;  /* 6msec minimum */
	value = lay->val[index];
	if (sf->version == 1)
		return value;
	else
		return TO_MSEC(value);
}

/* convert peak volume to linear volume (0-255) */
static float calc_volume(Layer *lay, SFInfo *sf)
{
	if (sf->version == 1)
		return (float)(lay->val[SF_instVol] * 2) / 255.0;
	else
		return TO_LINEAR((double)lay->val[SF_instVol] / 10.0);
}

/* convert sustain volume to linear volume */
static int32 calc_sustain(Layer *lay, SFInfo *sf)
{
	int32 level;
	if (!lay->set[SF_sustainEnv2])
		return 250;
	level = lay->val[SF_sustainEnv2];
	if (sf->version == 1) {
		if (level < 96)
			level = 1000 * (96 - level) / 96;
		else
			return 0;
	}
	return TO_VOLUME(level);
}


#ifndef SF_SUPPRESS_TREMOLO
/*----------------------------------------------------------------
 * tremolo (LFO1) conversion
 *----------------------------------------------------------------*/

static void convert_tremolo(Layer *lay, SFInfo *sf, SampleList *sp)
{
	extern int32 convert_tremolo_rate(uint8 rate);
	int32 level, freq;

	if (!lay->set[SF_lfo1ToVolume])
		return;

	level = lay->val[SF_lfo1ToVolume];
	if (sf->version == 1)
		level = (120 * level) / 64;  /* to centibel */
	/* centibel to linear */
	/*sp->v.tremolo_depth = TO_LINEAR(level);*/

	if (level < 0) sp->v.tremolo_depth = -level;
	else if (level < 20) sp->v.tremolo_depth = 20;
	else sp->v.tremolo_depth = level;

	/* frequency in mHz */
	if (! lay->set[SF_freqLfo1]) {
		if (sf->version == 1)
			freq = TO_MHZ(-725);
		else
			freq = 0;
	} else {
		freq = lay->val[SF_freqLfo1];
		if (freq > 0 && sf->version == 1)
			freq = (int)(3986.0 * log10((double)freq) - 7925.0);
		freq = TO_MHZ(freq);
	}
	/* convert mHz to sine table increment; 1024<<rate_shift=1wave */
	/*sp->v.tremolo_phase_increment = (freq * 1024) << RATE_SHIFT;*/

	freq /= 40;
	if (freq < 5) return;
	if (freq > 255) freq = 255;
	sp->v.tremolo_phase_increment = convert_tremolo_rate((uint8)freq);
	sp->v.tremolo_sweep_increment = 41;
}
#endif


#ifndef SF_SUPPRESS_VIBRATO
/*----------------------------------------------------------------
 * vibrato (LFO2) conversion
 * (note: my changes to Takashi's code are unprincipled --gl)
 *----------------------------------------------------------------*/

static void convert_vibrato(Layer *lay, SFInfo *sf, SampleList *sp)
{
	int32 shift=0, freq=0;

	if (lay->set[SF_lfo2ToPitch]) {
		shift = lay->set[SF_lfo2ToPitch];
		if (lay->set[SF_freqLfo2]) freq = lay->val[SF_freqLfo2];
	}
	else if (lay->set[SF_lfo1ToPitch]) {
		shift = lay->set[SF_lfo1ToPitch];
		if (lay->set[SF_freqLfo1]) freq = lay->val[SF_freqLfo1];
	}

	if (!shift) return;
	if (!freq) return;

	/* pitch shift in cents (= 1/100 semitone) */
	if (sf->version == 1)
		shift = (1200 * shift / 64 + 1) / 2;

	/* cents to linear; 400cents = 256 */
	/*sp->v.vibrato_depth = (int8)((int32)shift * 256 / 400);*/
	sp->v.vibrato_depth = shift * 400 / 64;

	/* frequency in mHz */
	if (!freq) {
		if (sf->version == 1)
			freq = TO_HZ(-725);
		else
			freq = 0;
	} else {
		if (sf->version == 1)
			freq = (int)(3986.0 * log10((double)freq) - 7925.0);
		else freq = TO_HZ(freq);
	}
	/* convert mHz to control ratio */
	sp->v.vibrato_control_ratio = freq *
		(VIBRATO_RATE_TUNING * play_mode->rate) /
		(2 * 1000 * VIBRATO_SAMPLE_INCREMENTS);

	sp->v.vibrato_sweep_increment = 74;
}
#endif


/* calculate cutoff/resonance frequency */
static void calc_cutoff(Layer *lay, SFInfo *sf, SampleList *sp)
{
	int16 val;
	if (! lay->set[SF_initialFilterFc]) {
		val = 13500;
	} else {
		val = lay->val[SF_initialFilterFc];
		if (sf->version == 1) {
			if (val == 127)
				val = 14400;
			else if (val > 0)
				val = 50 * val + 4366;
		}
	}
	if (lay->set[SF_env1ToFilterFc]) {
		val += lay->val[SF_env1ToFilterFc];
	}
	if (val < 6500 || val >= 13500)
		sp->cutoff_freq = 0;
	else
		sp->cutoff_freq = TO_HZ(val);
}

static void calc_filterQ(Layer *lay, SFInfo *sf, SampleList *sp)
{
	int16 val = lay->val[SF_initialFilterQ];
	if (sf->version == 1)
		val = val * 3 / 2; /* to centibels */
	sp->resonance = pow(10.0, (double)val / 2.0 / 200.0) - 1;
	if (sp->resonance < 0)
		sp->resonance = 0;
}


#ifndef SF_SUPPRESS_CUTOFF
/*----------------------------------------------------------------
 * low-pass filter:
 * 	y(n) = A * x(n) + B * y(n-1)
 * 	A = 2.0 * pi * center
 * 	B = exp(-A / frequency)
 *----------------------------------------------------------------
 * resonance filter:
 *	y(n) = a * x(n) - b * y(n-1) - c * y(n-2)
 *	c = exp(-2 * pi * width / rate)
 *	b = -4 * c / (1+c) * cos(2 * pi * center / rate)
 *	a = sqt(1-b*b/(4 * c)) * (1-c)
 *----------------------------------------------------------------*/

#ifdef LOOKUP_HACK
#define MAX_DATAVAL 127
#define MIN_DATAVAL -128
#else
#define MAX_DATAVAL 32767
#define MIN_DATAVAL -32768
#endif

#define USE_BUTTERWORTH

#ifdef USE_BUTTERWORTH
static void do_lowpass_w_res(Sample *sp, int32 freq, float resonance)
#else
static void do_lowpass(Sample *sp, int32 freq, float resonance)
#endif
{
	double A, B, C;
	sample_t *buf, pv1, pv2;
	int32 i;

	if (freq > sp->sample_rate * 2) {
		ctl->cmsg(CMSG_ERROR, VERB_NORMAL,
			  "Lowpass: center must be < data rate*2");
		return;
	}
	A = 2.0 * M_PI * freq * 2.5 / sp->sample_rate;
	B = exp(-A / sp->sample_rate);
/*
	A *= 0.8;
	B *= 0.8;
*/
	C = 0;

	/* */
	if (resonance) {
		double a, b, c;
		int32 width;
		width = freq / 5;
		c = exp(-2.0 * M_PI * width / sp->sample_rate);
		b = -4.0 * c / (1+c) * cos(2.0 * M_PI * freq / sp->sample_rate);
		a = sqrt(1 - b * b / (4 * c)) * (1 - c);
		b = -b; c = -c;

		A += a * resonance;
		B += b;
		C = c;
	}
	/* */

/* some clipping w. 0.30, 0.25, 0.20, 0.15 */
	A *= 0.10;
	B *= 0.10;
	C *= 0.10;

	pv1 = 0;
	pv2 = 0;
	buf = sp->data;
	for (i = 0; i < sp->data_length; i++) {
		sample_t l = *buf;
		double d = A * l + B * pv1 + C * pv2;
		if (d > MAX_DATAVAL)
			d = MAX_DATAVAL;
		else if (d < MIN_DATAVAL)
			d = MIN_DATAVAL;
		pv2 = pv1;
		pv1 = *buf++ = (sample_t)d;
	}
}

#ifdef USE_BUTTERWORTH
static void do_lowpass(Sample *sp, int32 freq, float resonance)
{
	double C, a0, a1, a2, b0, b1;
	sample_t *buf = sp->data;
	double oldin=0, lastin=0, oldout=0, lastout=0;
	int i, clips = 0;
	double maxin=0, minin=0, inputgain=0.95;

	if (resonance) do_lowpass_w_res(sp, freq, resonance);

	if (freq > 12000) freq = 12000;

	if (freq > sp->sample_rate * 2) {
		ctl->cmsg(CMSG_ERROR, VERB_NORMAL,
			  "Lowpass: center must be < data rate*2");
		return;
	}

	C = 1.0 / tan(M_PI * freq / sp->sample_rate);

	a0 = 1.0 / (1.0 + sqrt(2.0) * C + C * C);
	a1 = 2.0 * a0;
	a2 = a0;

	b0 = 2 * (1.0 - C * C) * a0;
	b1 = (1.0 - sqrt(2.0) * C + C * C) * a0;

	if (a1 > 2.0) {
		inputgain = 1 / a1;
	}
	for (i = 0; i < sp->data_length; i++) {
		sample_t samp = *buf;
		double outsamp, insamp;
		insamp = samp * inputgain;
		outsamp = a0 * insamp + a1 * lastin + a2 * oldin - b0 * lastout - b1 * oldout;
		if (outsamp >maxin) maxin = outsamp;
		if (outsamp <minin) minin = outsamp;
		lastout = outsamp;
		if (outsamp > MAX_DATAVAL) {
			outsamp = MAX_DATAVAL;
			clips++;
		}
		else if (outsamp < MIN_DATAVAL) {
			outsamp = MIN_DATAVAL;
			clips++;
		}
		*buf++ = (sample_t)outsamp;
		oldin = lastin;
		lastin = insamp;
		oldout = lastout;
	}
#ifdef BUTTERWORTH_DEBUG
if (resonance)
printf("\tbandwith %f centerfreq %f f=%ld : a0=%f a1=%f a2=%f b0=%f b1=%f\n", bandwidth, centerfreq, freq,
	 a0,a1,a2,b0,b1);
/*
else
printf("\tgain %f , f=%ld : a0=%f a1=%f a2=%f b0=%f b1=%f\n", inputgain, freq, a0,a1,a2,b0,b1);
*/
	if (clips) {
    		ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "%d clips: maxout %f, minout %f : a0=%f a1=%f a2=%f b0=%f b1=%f",
 clips, maxin, minin, a0,a1,a2,b0,b1);
		/*if (clips>60) exit(1);*/
	}
#endif
}
#endif

#endif
