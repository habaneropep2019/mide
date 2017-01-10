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

   instrum.h

   */

typedef struct {
  int32
    loop_start, loop_end, data_length,
    sample_rate, low_freq, high_freq, root_freq;

  int8
    root_tune, fine_tune;

  int32
    envelope_rate[6], envelope_offset[6];
  float
    volume, resonance;
  sample_t *data;
  int32 
    tremolo_sweep_increment, tremolo_phase_increment, 
    vibrato_sweep_increment, vibrato_control_ratio,
    cutoff_freq;
  uint8
    reverberation, chorusdepth,
    tremolo_depth, vibrato_depth,
    modes;
  int8
    panning, note_to_use;
  int16
    scale_tuning;
  uint8
    attenuation, freq_center;
  int32
    freq_scale;
} Sample;

/* Bits in modes: */
#define MODES_16BIT	(1<<0)
#define MODES_UNSIGNED	(1<<1)
#define MODES_LOOPING	(1<<2)
#define MODES_PINGPONG	(1<<3)
#define MODES_REVERSE	(1<<4)
#define MODES_SUSTAIN	(1<<5)
#define MODES_ENVELOPE	(1<<6)
#define MODES_FAST_RELEASE	(1<<7)

#define INST_GUS	0
#define INST_SF2	1

typedef struct {
  int type;
  int samples;
  Sample *sample;
  int left_samples;
  Sample *left_sample;
  int right_samples;
  Sample *right_sample;
} Instrument;

#define FONT_NORMAL 0
#define FONT_FFF    1
#define FONT_SBK    2

typedef struct {
  char *name;
  Instrument *instrument;
  int font_type;
#ifndef ADAGIO
  int note, amp, pan, strip_loop, strip_envelope, strip_tail, brightness, harmoniccontent;
#else /* ADAGIO */
  int note, amp, pan, strip_loop, strip_envelope, strip_tail,
	 gm_num, tpgm, reverb, main_volume;
#endif /* ADAGIO */
} ToneBankElement;

/* A hack to delay instrument loading until after reading the
   entire MIDI file. */
#define MAGIC_LOAD_INSTRUMENT ((Instrument *)(-1))
#define MAXPROG 128
#define MAXBANK 129
#define SFXBANK (MAXBANK-1)

typedef struct {
#ifndef ADAGIO
  ToneBankElement tone[MAXPROG];
#else /* ADAGIO */
  ToneBankElement tone[MAX_TONE_VOICES];
#endif /* ADAGIO */
} ToneBank;

extern char *sf_file;
#ifndef ADAGIO
extern ToneBank *tonebank[], *drumset[];
#else /* ADAGIO */
extern ToneBank *tonebank[];
#endif /* ADAGIO */

extern Instrument *default_instrument;
extern int default_program;
extern int antialiasing_allowed;
extern int fast_decay;
extern int free_instruments_afterwards;
extern int cutoff_allowed;

#define SPECIAL_PROGRAM -1

extern int load_missing_instruments(void);
extern void free_instruments(void);
extern void end_soundfont(void);
extern int set_default_instrument(char *name);

extern void init_soundfont(char *fname, int order, int oldbank, int newbank);
Instrument *load_soundfont(int order, int bank, int preset, int keynote);
void exclude_soundfont(int bank, int preset, int keynote);
void order_soundfont(int bank, int preset, int keynote, int order);
