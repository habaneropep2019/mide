/*

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   instrum.c 
   
   Code to load and unload GUS-compatible instrument patches.

*/

#include <stdio.h>

#if (defined(SUN) && defined(SYSV)) || defined(__WIN32__)
#include <string.h>
#else
#include <strings.h>
#endif

#if defined(__FreeBSD__) || defined (__WIN32__)
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include "config.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"
#include "output.h"
#include "controls.h"
#include "resample.h"
#include "tables.h"
#include "filter.h"

/* Some functions get aggravated if not even the standard banks are 
   available. */
#ifndef ADAGIO
static ToneBank standard_tonebank, standard_drumset;
#else /* ADAGIO */
static ToneBank standard_tonebank;
#endif /* ADAGIO */
ToneBank 
#ifndef ADAGIO
  *tonebank[MAXBANK]={&standard_tonebank},
  *drumset[MAXBANK]={&standard_drumset};
#else /* ADAGIO */
  *tonebank[1]={&standard_tonebank};
#endif /* ADAGIO */

/* This is a special instrument, used for all melodic programs */
Instrument *default_instrument=0;

/* This is only used for tracks that don't specify a program */
int default_program=DEFAULT_PROGRAM;

int antialiasing_allowed=0;

#ifdef FAST_DECAY
int fast_decay=1;
#else
int fast_decay=0;
#endif

static void free_instrument(Instrument *ip)
{
  Sample *sp;
  int i;
  if (!ip) return;
  for (i=0; i<ip->samples; i++)
    {
      sp=&(ip->sample[i]);
      free(sp->data);
    }
  free(ip->sample);
  for (i=0; i<ip->right_samples; i++)
    {
      sp=&(ip->right_sample[i]);
      free(sp->data);
    }
  if (ip->right_sample)
    free(ip->right_sample);
  free(ip);
}

#ifndef ADAGIO
static void free_bank(int dr, int b)
#else /* ADAGIO */
static void free_bank(int b)
#endif /* ADAGIO */
{
  int i;
#ifndef ADAGIO
  ToneBank *bank=((dr) ? drumset[b] : tonebank[b]);
  for (i=0; i<MAXPROG; i++)
#else /* ADAGIO */
  ToneBank *bank= tonebank[b];
  for (i=0; i<MAX_TONE_VOICES; i++)
#endif /* ADAGIO */
    if (bank->tone[i].instrument)
      {
	/* Not that this could ever happen, of course */
	if (bank->tone[i].instrument != MAGIC_LOAD_INSTRUMENT)
	  free_instrument(bank->tone[i].instrument);
	bank->tone[i].instrument=0;
      }
}

int32 convert_envelope_rate_attack(uint8 rate, uint8 fastness)
{
  int32 r;

  r=3-((rate>>6) & 0x3);
  r*=3;
  r = (int32)(rate & 0x3f) << r; /* 6.9 fixed point */

  /* 15.15 fixed point. */
  return (((r * 44100) / play_mode->rate) * control_ratio) 
    << 10;
#if 0
    << fastness;
#endif
}

int32 convert_envelope_rate(uint8 rate)
{
  int32 r;

  r=3-((rate>>6) & 0x3);
  r*=3;
  r = (int32)(rate & 0x3f) << r; /* 6.9 fixed point */

  /* 15.15 fixed point. */
  return (((r * 44100) / play_mode->rate) * control_ratio) 
    << ((fast_decay) ? 10 : 9);
}

int32 convert_envelope_offset(uint8 offset)
{
  /* This is not too good... Can anyone tell me what these values mean?
     Are they GUS-style "exponential" volumes? And what does that mean? */

  /* 15.15 fixed point */
  return offset << (7+15);
}

 int32 convert_tremolo_sweep(uint8 sweep)
{
  if (!sweep)
    return 0;

  return
    ((control_ratio * SWEEP_TUNING) << SWEEP_SHIFT) /
      (play_mode->rate * sweep);
}

 int32 convert_vibrato_sweep(uint8 sweep, int32 vib_control_ratio)
{
  if (!sweep)
    return 0;

  return
    (int32) (FRSCALE((double) (vib_control_ratio) * SWEEP_TUNING, SWEEP_SHIFT)
	     / (double)(play_mode->rate * sweep));

  /* this was overflowing with seashore.pat

      ((vib_control_ratio * SWEEP_TUNING) << SWEEP_SHIFT) /
      (play_mode->rate * sweep); */
}

 int32 convert_tremolo_rate(uint8 rate)
{
  return
    ((SINE_CYCLE_LENGTH * control_ratio * rate) << RATE_SHIFT) /
      (TREMOLO_RATE_TUNING * play_mode->rate);
}

 int32 convert_vibrato_rate(uint8 rate)
{
  /* Return a suitable vibrato_control_ratio value */
  return
    (VIBRATO_RATE_TUNING * play_mode->rate) / 
      (rate * 2 * VIBRATO_SAMPLE_INCREMENTS);
}

static void reverse_data(int16 *sp, int32 ls, int32 le)
{
  int16 s, *ep=sp+le;
  sp+=ls;
  le-=ls;
  le/=2;
  while (le--)
    {
      s=*sp;
      *sp++=*ep;
      *ep--=s;
    }
}

/* 
   If panning or note_to_use != -1, it will be used for all samples,
   instead of the sample-specific values in the instrument file. 

   For note_to_use, any value <0 or >127 will be forced to 0.
 
   For other parameters, 1 means yes, 0 means no, other values are
   undefined.

   TODO: do reverse loops right */
static Instrument *load_instrument(char *name, int font_type, int percussion,
				   int panning, int amp, int note_to_use,
				   int strip_loop, int strip_envelope,
#ifndef ADAGIO
				   int strip_tail, int brightness, int harmoniccontent, int bank, int gm_num)
#else /* ADAGIO */
				   int strip_tail,
				   int gm_num, int tpgm, int reverb, int main_volume)
#endif /* ADAGIO */
{
  Instrument *ip;
  Sample *sp;
  FILE *fp;
  uint8 tmp[1024];
  int i,j,noluck=0;
#ifdef PATCH_EXT_LIST
  static char *patch_ext[] = PATCH_EXT_LIST;
#endif
#ifdef ADAGIO
  int newmode;
  extern Instrument *load_fff_patch(int, int, int, int);
  extern Instrument *load_sbk_patch(int, int, int, int, int, int, int);
  extern int next_wave_prog;

  if (gm_num >= 0) {
  if ((ip = load_fff_patch(gm_num, tpgm, reverb, main_volume))) return(ip);
  if ((ip = load_sbk_patch(0, gm_num, tpgm, reverb, main_volume, brightness, harmoniccontent))) return(ip);
  }
#else
  extern Instrument *load_fff_patch(char *, int, int, int, int, int, int, int, int, int);
  extern Instrument *load_sbk_patch(int, char *, int, int, int, int, int, int, int, int, int, int, int);

  if (gm_num >= 0) {
      if (font_type == FONT_FFF && (ip = load_fff_patch(name, gm_num, bank, percussion,
			   panning, amp, note_to_use,
			   strip_loop, strip_envelope,
			   strip_tail))) return(ip);
	/* Substitute next if sbk fonts are not to require bank and preset
	   declarations -- also undefine STRICT_FONT_DECL in sndfont.c. --gl
	 */
      /*if ((!bank || font_type == FONT_SBK) && (ip = load_sbk_patch(0, name, gm_num, bank, percussion,*/
      if (font_type == FONT_SBK && (ip = load_sbk_patch(0, name, gm_num, bank, percussion,
			   panning, amp, note_to_use,
			   strip_loop, strip_envelope,
			   strip_tail, brightness, harmoniccontent))) return(ip);
  }
#endif

  if (!name) return 0;

  /* Open patch file */
  if (!(fp=open_file(name, 1, OF_NORMAL)))
    {
      noluck=1;
#ifdef PATCH_EXT_LIST
      /* Try with various extensions */
      for (i=0; patch_ext[i]; i++)
	{
	  if (strlen(name)+strlen(patch_ext[i])<1024)
	    {
	      strcpy(tmp, name);
	      strcat(tmp, patch_ext[i]);
	      if ((fp=open_file(tmp, 1, OF_NORMAL)))
		{
		  noluck=0;
		  break;
		}
	    }
	}
#endif
    }
  
  if (noluck)
    {
    #ifndef ADAGIO
      if (gm_num >= 0) {
  	if (font_type == FONT_SBK && (ip = load_sbk_patch(1, name, gm_num, bank, percussion,
			   panning, amp, note_to_use,
			   strip_loop, strip_envelope,
			   strip_tail, brightness, harmoniccontent))) return(ip);
      }
    #endif
      ctl->cmsg(CMSG_ERROR, VERB_NORMAL, 
		"Instrument `%s' can't be found.", name);
    #ifdef ADAGIO
      gus_voice[tpgm].volume = DOES_NOT_EXIST;
    #endif
  
      return 0;
    }
      
  ctl->cmsg(CMSG_INFO, VERB_NOISY, "Loading instrument %s", current_filename);

  /* Read some headers and do cursory sanity checks. There are loads
     of magic offsets. This could be rewritten... */

  if ((239 != fread(tmp, 1, 239, fp)) ||
      (memcmp(tmp, "GF1PATCH110\0ID#000002", 22) &&
       memcmp(tmp, "GF1PATCH100\0ID#000002", 22))) /* don't know what the
						      differences are */
    {
      ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "%s: not an instrument", name);
      return 0;
    }
  
  if (tmp[82] != 1 && tmp[82] != 0) /* instruments. To some patch makers, 
				       0 means 1 */
    {
      ctl->cmsg(CMSG_ERROR, VERB_NORMAL, 
	   "Can't handle patches with %d instruments", tmp[82]);
      return 0;
    }

  if (tmp[151] != 1 && tmp[151] != 0) /* layers. What's a layer? */
    {
      ctl->cmsg(CMSG_ERROR, VERB_NORMAL, 
	   "Can't handle instruments with %d layers", tmp[151]);
      return 0;
    }

#ifdef ADAGIO
  gus_voice[tpgm].loaded |= DSP_MASK;
  gus_voice[tpgm].prog = next_wave_prog++;
  newmode = gus_voice[tpgm].modes;
#endif
  
  ip=safe_malloc(sizeof(Instrument));
  ip->type = INST_GUS;
  ip->samples = tmp[198];
  ip->sample = safe_malloc(sizeof(Sample) * ip->samples);
  ip->left_samples = ip->samples;
  ip->left_sample = ip->sample;
  ip->right_samples = 0;
  ip->right_sample = 0;

  for (i=0; i<ip->samples; i++)
    {

      uint8 fractions;
      int32 tmplong;
      uint16 tmpshort;
      uint8 tmpchar;

#define READ_CHAR(thing) \
      if (1 != fread(&tmpchar, 1, 1, fp)) goto fail; \
      thing = tmpchar;
#define READ_SHORT(thing) \
      if (1 != fread(&tmpshort, 2, 1, fp)) goto fail; \
      thing = LE_SHORT(tmpshort);
#define READ_LONG(thing) \
      if (1 != fread(&tmplong, 4, 1, fp)) goto fail; \
      thing = LE_LONG(tmplong);

      skip(fp, 7); /* Skip the wave name */

      if (1 != fread(&fractions, 1, 1, fp))
	{
	fail:
	  ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "Error reading sample %d", i);
	  for (j=0; j<i; j++)
	    free(ip->sample[j].data);
	  free(ip->sample);
	  free(ip);
	  return 0;
	}

      sp=&(ip->sample[i]);
      
      READ_LONG(sp->data_length);
      READ_LONG(sp->loop_start);
      READ_LONG(sp->loop_end);
      READ_SHORT(sp->sample_rate);
      READ_LONG(sp->low_freq);
      READ_LONG(sp->high_freq);
      READ_LONG(sp->root_freq);
      skip(fp, 2); /* Why have a "root frequency" and then "tuning"?? */
      
      READ_CHAR(tmp[0]);

      if (panning==-1)
	sp->panning = (tmp[0] * 8 + 4) & 0x7f;
      else
	sp->panning=(uint8)(panning & 0x7F);

      sp->resonance=0;
      sp->cutoff_freq=0;
      sp->reverberation=0;
      sp->chorusdepth=0;

      /* envelope, tremolo, and vibrato */
      if (18 != fread(tmp, 1, 18, fp)) goto fail; 

      if (!tmp[13] || !tmp[14])
	{
	  sp->tremolo_sweep_increment=
	    sp->tremolo_phase_increment=sp->tremolo_depth=0;
	  ctl->cmsg(CMSG_INFO, VERB_DEBUG, " * no tremolo");
	}
      else
	{
	  sp->tremolo_sweep_increment=convert_tremolo_sweep(tmp[12]);
	  sp->tremolo_phase_increment=convert_tremolo_rate(tmp[13]);
	  sp->tremolo_depth=tmp[14];
	  ctl->cmsg(CMSG_INFO, VERB_DEBUG,
	       " * tremolo: sweep %d, phase %d, depth %d",
	       sp->tremolo_sweep_increment, sp->tremolo_phase_increment,
	       sp->tremolo_depth);
	}

#ifdef ADAGIO
      if (1)
#else
      if (!tmp[16] || !tmp[17])
#endif
	{
	  sp->vibrato_sweep_increment=
	    sp->vibrato_control_ratio=sp->vibrato_depth=0;
	  ctl->cmsg(CMSG_INFO, VERB_DEBUG, " * no vibrato");
	}
      else
	{
	  sp->vibrato_control_ratio=convert_vibrato_rate(tmp[16]);
	  sp->vibrato_sweep_increment=
	    convert_vibrato_sweep(tmp[15], sp->vibrato_control_ratio);
	  sp->vibrato_depth=tmp[17];
	  ctl->cmsg(CMSG_INFO, VERB_DEBUG,
	       " * vibrato: sweep %d, ctl %d, depth %d",
	       sp->vibrato_sweep_increment, sp->vibrato_control_ratio,
	       sp->vibrato_depth);

	}

      READ_CHAR(sp->modes);
      READ_SHORT(sp->freq_center);
      READ_SHORT(sp->freq_scale);
      skip(fp, 36);
#if 0
      skip(fp, 40); /* skip the useless scale frequency, scale factor
		       (what's it mean?), and reserved space */
#endif

#ifdef ADAGIO
      if (sp->freq_scale == 1024) sp->freq_center = 60;
#ifndef STRIP_PERCUSSION
      strip_loop = strip_envelope = strip_tail = 0;
#endif

      if (!i) {
        gus_voice[tpgm].vibrato_sweep = tmp[15];
        gus_voice[tpgm].vibrato_rate = tmp[16];
        gus_voice[tpgm].vibrato_depth = sp->vibrato_depth;
        if (!newmode) gus_voice[tpgm].modes = sp->modes;
      }
      if (newmode) sp->modes = newmode;

      amp = gus_voice[tpgm].volume;

      if (gm_num > 127 && gus_voice[tpgm].fix_key) {
	sp->freq_scale = 0;
	note_to_use = sp->freq_center = gus_voice[tpgm].fix_key;
      }
      else if (!sp->freq_scale) gus_voice[tpgm].fix_key = sp->freq_center;

      if (gus_voice[tpgm].strip & LOOPS_FLAG) strip_loop = 1;
      if (gus_voice[tpgm].strip & ENVELOPE_FLAG) strip_envelope = 1;
      if (gus_voice[tpgm].strip & TAIL_FLAG) strip_tail = 1;
      if (gus_voice[tpgm].keep & LOOPS_FLAG) strip_loop = 0;
      if (gus_voice[tpgm].keep & ENVELOPE_FLAG) strip_envelope = 0;
      if (gus_voice[tpgm].keep & TAIL_FLAG) strip_tail = 0;
#endif /* ADAGIO */

      /* Mark this as a fixed-pitch instrument if such a deed is desired. */
      if (note_to_use!=-1)
	sp->note_to_use=(uint8)(note_to_use);
      else
	sp->note_to_use=0;
      
      /* seashore.pat in the Midia patch set has no Sustain. I don't
         understand why, and fixing it by adding the Sustain flag to
         all looped patches probably breaks something else. We do it
         anyway. */
	 
      if (sp->modes & MODES_LOOPING) 
	sp->modes |= MODES_SUSTAIN;

      /* Strip any loops and envelopes we're permitted to */
      if ((strip_loop==1) && 
	  (sp->modes & (MODES_SUSTAIN | MODES_LOOPING | 
			MODES_PINGPONG | MODES_REVERSE)))
	{
	  ctl->cmsg(CMSG_INFO, VERB_DEBUG, " - Removing loop and/or sustain");
	  sp->modes &=~(MODES_SUSTAIN | MODES_LOOPING | 
			MODES_PINGPONG | MODES_REVERSE);
	}

      if (strip_envelope==1)
	{
	  if (sp->modes & MODES_ENVELOPE)
	    ctl->cmsg(CMSG_INFO, VERB_DEBUG, " - Removing envelope");
	  sp->modes &= ~MODES_ENVELOPE;
	}
      else if (strip_envelope != 0)
	{
	  /* Have to make a guess. */
	  if (!(sp->modes & (MODES_LOOPING | MODES_PINGPONG | MODES_REVERSE)))
	    {
	      /* No loop? Then what's there to sustain? No envelope needed
		 either... */
	      sp->modes &= ~(MODES_SUSTAIN|MODES_ENVELOPE);
	      ctl->cmsg(CMSG_INFO, VERB_DEBUG, 
			" - No loop, removing sustain and envelope");
	    }
	  else if (!memcmp(tmp, "??????", 6) || tmp[11] >= 100) 
	    {
	      /* Envelope rates all maxed out? Envelope end at a high "offset"?
		 That's a weird envelope. Take it out. */
	      sp->modes &= ~MODES_ENVELOPE;
	      ctl->cmsg(CMSG_INFO, VERB_DEBUG, 
			" - Weirdness, removing envelope");
	    }
#ifndef ADAGIO
	  else if (!(sp->modes & MODES_SUSTAIN))
	    {
	      /* No sustain? Then no envelope.  I don't know if this is
		 justified, but patches without sustain usually don't need the
		 envelope either... at least the Gravis ones. They're mostly
		 drums.  I think. */
	      sp->modes &= ~MODES_ENVELOPE;
	      ctl->cmsg(CMSG_INFO, VERB_DEBUG, 
			" - No sustain, removing envelope");
	    }
#endif
	}

	sp->attenuation = 0;
#ifdef ADAGIO

        for (j = 0; j < 6; j++) {
	    if (gus_voice[tpgm].keep & USEENV_FLAG) {
		tmp[6+j] = gus_voice[tpgm].envelope_offset[j];
		tmp[j] = gus_voice[tpgm].envelope_rate[j];
	    } else if (!i) {
		gus_voice[tpgm].envelope_offset[j] = tmp[6+j];
		gus_voice[tpgm].envelope_rate[j] = tmp[j];
	    }
	}

#ifdef REV_E_ADJUST
        if (reverb) {
	    int r = reverb;
	    int dec = tmp[2];
	    r = (127 - r) / 6;
	    if (r < 0) r = 0;
	    if (r > 28) r = 28;
	    r += dec & 0x3f;
	    if (r > 63) r = 63;
	    dec = (dec & 0xc0) | r;
	    tmp[2] = dec;
	    r = reverb;
	    if (r > 127) r = 127;
	    if (gm_num < 120) tmp[3] = (2<<6) | (12 - (r>>4));
	    else if (gm_num > 127) tmp[1] = (3<<6) | (63 - (r>>1));
        }
#endif

#ifdef VOL_E_ADJUST
#define VR_NUM 2
#define VR_DEN 3
      {
        unsigned voff, poff;
        for (j = 0; j < 6; j++) {
            voff = tmp[6+j];
            poff = 2 + main_volume + 63 + gus_voice[tpgm].volume / 2;
            voff = ((poff + VR_NUM*256) * voff + VR_DEN*128) / (VR_DEN*256);
            tmp[6+j] = voff;
        }
      }
#endif
#endif
/******debug*********
	if (gm_num==67)
	  for (j = 0; j < 6; j++) {
	printf("\t%d: rate %ld offset %ld\n", j, tmp[j], tmp[6+j]);
	  }
******debug*********/

      for (j=0; j<6; j++)
	{
	  sp->envelope_rate[j]=
	    (j<3)? convert_envelope_rate_attack(tmp[j], 11) : convert_envelope_rate(tmp[j]);
	  sp->envelope_offset[j]= 
	    convert_envelope_offset(tmp[6+j]);
	}

      /* Then read the sample data */
      sp->data = safe_malloc(sp->data_length);
      if (1 != fread(sp->data, sp->data_length, 1, fp))
	goto fail;

      if (!(sp->modes & MODES_16BIT)) /* convert to 16-bit data */
	{
	  int32 i=sp->data_length;
	  uint8 *cp=(uint8 *)(sp->data);
	  uint16 *tmp,*new;
	  tmp=new=safe_malloc(sp->data_length*2);
	  while (i--)
	    *tmp++ = (uint16)(*cp++) << 8;
	  cp=(uint8 *)(sp->data);
	  sp->data = (sample_t *)new;
	  free(cp);
	  sp->data_length *= 2;
	  sp->loop_start *= 2;
	  sp->loop_end *= 2;
	}
#ifndef LITTLE_ENDIAN
      else
	/* convert to machine byte order */
	{
	  int32 i=sp->data_length/2;
	  int16 *tmp=(int16 *)sp->data,s;
	  while (i--)
	    { 
	      s=LE_SHORT(*tmp);
	      *tmp++=s;
	    }
	}
#endif
      
      if (sp->modes & MODES_UNSIGNED) /* convert to signed data */
	{
	  int32 i=sp->data_length/2;
	  int16 *tmp=(int16 *)sp->data;
	  while (i--)
	    *tmp++ ^= 0x8000;
	}

      /* Reverse reverse loops and pass them off as normal loops */
      if (sp->modes & MODES_REVERSE)
	{
	  int32 t;
	  /* The GUS apparently plays reverse loops by reversing the
	     whole sample. We do the same because the GUS does not SUCK. */

	  ctl->cmsg(CMSG_WARNING, VERB_NORMAL, "Reverse loop in %s", name);
	  reverse_data((int16 *)sp->data, 0, sp->data_length/2);

	  t=sp->loop_start;
	  sp->loop_start=sp->data_length - sp->loop_end;
	  sp->loop_end=sp->data_length - t;

	  sp->modes &= ~MODES_REVERSE;
	  sp->modes |= MODES_LOOPING; /* just in case */
	}

      /* If necessary do some anti-aliasing filtering  */

      if (antialiasing_allowed)
	  antialiasing(sp,play_mode->rate);

#ifdef ADJUST_SAMPLE_VOLUMES
      if (amp!=-1)
	sp->volume=(double)(amp) / 100.0;
      else
	{
	  /* Try to determine a volume scaling factor for the sample.
	     This is a very crude adjustment, but things sound more
	     balanced with it. Still, this should be a runtime option. */
	  int32 i=sp->data_length/2;
	  int16 maxamp=0,a;
	  int16 *tmp=(int16 *)sp->data;
	  while (i--)
	    {
	      a=*tmp++;
	      if (a<0) a=-a;
	      if (a>maxamp)
		maxamp=a;
	    }
	  sp->volume=32768.0 / (double)(maxamp);
	  ctl->cmsg(CMSG_INFO, VERB_DEBUG, " * volume comp: %f", sp->volume);
	}
#else
      if (amp!=-1)
	sp->volume=(double)(amp) / 100.0;
      else
	sp->volume=1.0;
#endif

      sp->data_length /= 2; /* These are in bytes. Convert into samples. */
      sp->loop_start /= 2;
      sp->loop_end /= 2;

      /* Then fractional samples */
      sp->data_length <<= FRACTION_BITS;
      sp->loop_start <<= FRACTION_BITS;
      sp->loop_end <<= FRACTION_BITS;

      /* Adjust for fractional loop points. This is a guess. Does anyone
	 know what "fractions" really stands for? */
      sp->loop_start |=
	(fractions & 0x0F) << (FRACTION_BITS-4);
      sp->loop_end |=
	((fractions>>4) & 0x0F) << (FRACTION_BITS-4);

      /* If this instrument will always be played on the same note,
	 and it's not looped, we can resample it now. */
      if (sp->note_to_use && !(sp->modes & MODES_LOOPING))
	pre_resample(sp);

#ifdef LOOKUP_HACK
      /* Squash the 16-bit data into 8 bits. */
      {
	uint8 *gulp,*ulp;
	int16 *swp;
	int l=sp->data_length >> FRACTION_BITS;
	gulp=ulp=safe_malloc(l+1);
	swp=(int16 *)sp->data;
	while(l--)
	  *ulp++ = (*swp++ >> 8) & 0xFF;
	free(sp->data);
	sp->data=(sample_t *)gulp;
      }
#endif
      
      if (strip_tail==1)
	{
	  /* Let's not really, just say we did. */
	  ctl->cmsg(CMSG_INFO, VERB_DEBUG, " - Stripping tail");
	  sp->data_length = sp->loop_end;
	}
    }

  close_file(fp);
  return ip;
}

#ifndef ADAGIO
static int fill_bank(int dr, int b)
#else /* ADAGIO */
static int fill_bank(int b)
#endif /* ADAGIO */
{
#ifndef ADAGIO
  int i, errors=0;
  ToneBank *bank=((dr) ? drumset[b] : tonebank[b]);
#else /* ADAGIO */
  int i, errors=0, dr;
  ToneBank *bank=tonebank[b];
#endif /* ADAGIO */
  if (!bank)
    {
      ctl->cmsg(CMSG_ERROR, VERB_NORMAL, 
	#ifndef ADAGIO
	   "Huh. Tried to load instruments in non-existent %s %d",
	   (dr) ? "drumset" : "tone bank", b);
	#else /* ADAGIO */
	   "Huh. Tried to load instruments in non-existent tone bank %d", b);
	#endif /* ADAGIO */
      return 0;
    }
  #ifndef ADAGIO
  for (i=0; i<MAXPROG; i++)
  #else /* ADAGIO */
  for (i=0; i<MAX_TONE_VOICES; i++)
  #endif /* ADAGIO */
    {
      #ifdef ADAGIO
      dr = (bank->tone[i].gm_num - 128);
      if (dr < 0) dr = 0;
      #endif /* ADAGIO */
      if (bank->tone[i].instrument==MAGIC_LOAD_INSTRUMENT)
	{
	  if (!(bank->tone[i].name))
	    {
	      ctl->cmsg(CMSG_WARNING, (b!=0) ? VERB_VERBOSE : VERB_NORMAL,
		   "No instrument mapped to %s %d, program %d%s",
		   (dr)? "drum set" : "tone bank", b, i, 
		   (b!=0) ? "" : " - this instrument will not be heard");
	      if (b!=0)
		{
		  /* Mark the corresponding instrument in the default
		     bank / drumset for loading (if it isn't already) */
		#ifndef ADAGIO
		  if (!dr)
		    {
		#endif /* not ADAGIO */
		      if (!(standard_tonebank.tone[i].instrument))
			standard_tonebank.tone[i].instrument=
			  MAGIC_LOAD_INSTRUMENT;
		#ifndef ADAGIO
		    }
		  else
		    {
		      if (!(standard_drumset.tone[i].instrument))
			standard_drumset.tone[i].instrument=
			  MAGIC_LOAD_INSTRUMENT;
		    }
		#endif /* not ADAGIO */
		}
	      bank->tone[i].instrument=0;
	      errors++;
	    }
	  else if (!(bank->tone[i].instrument=
		     load_instrument(bank->tone[i].name, 
				     bank->tone[i].font_type,
				     (dr) ? 1 : 0,
				     bank->tone[i].pan,
				     bank->tone[i].amp,
				     (bank->tone[i].note!=-1) ? 
				     bank->tone[i].note :
				#ifndef ADAGIO
				     ((dr) ? i : -1),
				#else /* ADAGIO */
				     ((dr) ? dr : -1),
				#endif /* ADAGIO */
				     (bank->tone[i].strip_loop!=-1) ?
				     bank->tone[i].strip_loop :
				     ((dr) ? 1 : -1),
				     (bank->tone[i].strip_envelope != -1) ? 
				     bank->tone[i].strip_envelope :
				     ((dr) ? 1 : -1),
				#ifndef ADAGIO
				     bank->tone[i].strip_tail,
				     bank->tone[i].brightness,
				     bank->tone[i].harmoniccontent,
				     b,
				     ((dr) ? i + 128 : i) )))
				#else /* ADAGIO */
				     bank->tone[i].strip_tail,
				     bank->tone[i].gm_num,
				     bank->tone[i].tpgm,
				     bank->tone[i].reverb,
				     bank->tone[i].main_volume )))
				#endif /* ADAGIO */
	    {
	      ctl->cmsg(CMSG_ERROR, VERB_NORMAL, 
		   "Couldn't load instrument %s (%s %d, program %d)",
		   bank->tone[i].name,
		   (dr)? "drum set" : "tone bank", b, i);
	      errors++;
	    }
	}
    }
  return errors;
}

int load_missing_instruments(void)
{
#ifdef ADAGIO
  int errors=0;
  errors+=fill_bank(0);
#else
  int i=MAXBANK,errors=0;
  while (i--)
    {
      if (tonebank[i])
	errors+=fill_bank(0,i);
      if (drumset[i])
	errors+=fill_bank(1,i);
    }
#endif
  return errors;
}

void free_instruments(void)
{
  int i=MAXBANK;
  while(i--)
    {
      if (tonebank[i])
    #ifndef ADAGIO
	free_bank(0,i);
      if (drumset[i])
	free_bank(1,i);
    #else /* ADAGIO */
	free_bank(i);
    #endif /* ADAGIO */
    }
}

int set_default_instrument(char *name)
{
  Instrument *ip;
#ifndef ADAGIO
  if (!(ip=load_instrument(name, FONT_NORMAL, 0, -1, -1, -1, 0, 0, 0, -1, -1, 0, -1)))
#else /* ADAGIO */
  if (!(ip=load_instrument(name, FONT_NORMAL, 0, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0)))
#endif /* ADAGIO */
    return -1;
  if (default_instrument)
    free_instrument(default_instrument);
  default_instrument=ip;
  default_program=SPECIAL_PROGRAM;
  return 0;
}
