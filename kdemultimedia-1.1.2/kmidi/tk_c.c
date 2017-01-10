/*================================================================
 *
 * The Tcl/Tk interface for Timidity
 * written by Takashi Iwai (iwai@dragon.mm.t.u-tokyo.ac.jp)
 *
 * Most of the following codes are derived from both motif_ctl.c
 * and motif_pipe.c.  The communication between Tk program and
 * timidity is established by a pipe stream as in Motif interface.
 * On the contrast to motif, the stdin and stdout are assigned
 * as pipe i/o in Tk interface.
 *
 *================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <tcl.h>
#include <tk.h>

#include "config.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"
#include "output.h"
#include "controls.h"


static void ctl_refresh(void);
static void ctl_total_time(int tt);
static void ctl_master_volume(int mv);
static void ctl_file_name(char *name);
static void ctl_current_time(int ct);
static void ctl_note(int v);
static void ctl_program(int ch, int val);
static void ctl_volume(int channel, int val);
static void ctl_expression(int channel, int val);
static void ctl_panning(int channel, int val);
static void ctl_sustain(int channel, int val);
static void ctl_pitch_bend(int channel, int val);
static void ctl_reset(void);
static int ctl_open(int using_stdin, int using_stdout);
static void ctl_close(void);
static int ctl_read(int32 *valp);
static int cmsg(int type, int verbosity_level, char *fmt, ...);
static void ctl_pass_playing_list(int number_of_files, char *list_of_files[]);
static int ctl_blocking_read(int32 *valp);

static void pipe_printf(char *fmt, ...);
static void pipe_puts(char *str);
static int pipe_gets(char *str, int maxlen);
static void pipe_open();
static void pipe_error(char *st);
static int pipe_read_ready();

static int AppInit(Tcl_Interp *interp);
static int ExitAll(ClientData clientData, Tcl_Interp *interp,
		       int argc, char *argv[]);
static int TraceCreate(ClientData clientData, Tcl_Interp *interp,
		       int argc, char *argv[]);
static int TraceUpdate(ClientData clientData, Tcl_Interp *interp,
		    int argc, char *argv[]);
static int TraceReset(ClientData clientData, Tcl_Interp *interp,
		    int argc, char *argv[]);
static void trace_volume(int ch, int val);
static void trace_panning(int ch, int val);
static void trace_prog_init(int ch);
static void trace_prog(int ch, int val);
static void trace_bank(int ch, int val);
static void trace_sustain(int ch, int val);

static void get_child(int sig);
static void shm_alloc(void);
static void shm_free(int sig);

static void start_panel(void);

#define MAX_MIDI_CHANNELS	16

typedef struct {
	int reset_panel;
	int multi_part;

	int32 last_time, cur_time;

	char v_flags[MAX_MIDI_CHANNELS];
	int16 cnote[MAX_MIDI_CHANNELS];
	int16 cvel[MAX_MIDI_CHANNELS];
	int16 ctotal[MAX_MIDI_CHANNELS];

	char c_flags[MAX_MIDI_CHANNELS];
	Channel channel[MAX_MIDI_CHANNELS];
} PanelInfo;


/**********************************************/

#define ctl tk_control_mode

ControlMode ctl= 
{
  "Tcl/Tk interface", 'k',
  1,0,0,
  ctl_open, ctl_pass_playing_list, ctl_close, ctl_read, cmsg,
  ctl_refresh, ctl_reset, ctl_file_name, ctl_total_time, ctl_current_time, 
  ctl_note, 
  ctl_master_volume, ctl_program, ctl_volume, 
  ctl_expression, ctl_panning, ctl_sustain, ctl_pitch_bend
};

#define FLAG_NOTE_OFF	1
#define FLAG_NOTE_ON	2

#define FLAG_BANK	1
#define FLAG_PROG	2
#define FLAG_PAN	4
#define FLAG_SUST	8

static PanelInfo *Panel;

static int shmid;	/* shared memory id */
static int child_killed = 0;

static int pipeAppli[2],pipePanel[2]; /* Pipe for communication with Tcl/Tk process   */
static int fpip_in, fpip_out;	/* in and out depends in which process we are */
static int child_pid;	               /* Pid for child process */


/***********************************************************************/
/* Put controls on the pipe                                            */
/***********************************************************************/

static int cmsg(int type, int verbosity_level, char *fmt, ...)
{
	char local[2048];
#define TOO_LONG	2000

	va_list ap;
	if ((type==CMSG_TEXT || type==CMSG_INFO || type==CMSG_WARNING) &&
	    ctl.verbosity<verbosity_level)
		return 0;

	va_start(ap, fmt);
	if (strlen(fmt) > TOO_LONG)
		fmt[TOO_LONG] = 0;
	if (!ctl.opened) {
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
	} else if (type == CMSG_ERROR) {
		int rc;
		int32 val;
		vsprintf(local, fmt, ap);
		pipe_printf("CERR %d", type);
		pipe_puts(local);
		while ((rc = ctl_blocking_read(&val)) != RC_NEXT)
			;
	} else {
		vsprintf(local, fmt, ap);
		pipe_printf("CMSG %d", type);
		pipe_puts(local);
	}
	va_end(ap);
	return 0;
}

static void ctl_refresh(void)
{
}

static void ctl_total_time(int tt)
{
	int centisecs=tt/(play_mode->rate/100);
	pipe_printf("TIME %d", centisecs);
}

static void ctl_master_volume(int mv)
{
	pipe_printf("MVOL %d", mv);
}

static void ctl_file_name(char *name)
{
	pipe_printf("FILE %s", name);
}

static void ctl_current_time(int ct)
{
	Panel->cur_time = ct / (play_mode->rate/100) / 100;  /* sec order */
}


static void ctl_channel_note(int ch, int note, int vel)
{
	if (vel == 0) {
		if (note == Panel->cnote[ch])
			Panel->v_flags[ch] = FLAG_NOTE_OFF;
		Panel->cvel[ch] = 0;
	} else if (vel > Panel->cvel[ch]) {
		Panel->cvel[ch] = vel;
		Panel->cnote[ch] = note;
		Panel->ctotal[ch] = vel * Panel->channel[ch].volume *
			Panel->channel[ch].expression / (127*127);
		Panel->v_flags[ch] = FLAG_NOTE_ON;
	}
}

static void ctl_note(int v)
{
	int ch, note, vel;

	if (!ctl.trace_playing) 
		return;

	ch = voice[v].channel;
	if (ch < 0 || ch >= MAX_MIDI_CHANNELS) return;

	note = voice[v].note;
	if (voice[v].status != VOICE_ON)
		vel = 0;
	else
		vel = voice[v].velocity;
	ctl_channel_note(ch, note, vel);
}

static void ctl_program(int ch, int val)
{
	if (!ctl.trace_playing) 
		return;
	if (ch < 0 || ch >= MAX_MIDI_CHANNELS) return;
	Panel->channel[ch].program = val;
	Panel->c_flags[ch] |= FLAG_PROG;
}

static void ctl_volume(int ch, int val)
{
	if (!ctl.trace_playing)
		return;
	Panel->channel[ch].volume = val;
	ctl_channel_note(ch, Panel->cnote[ch], Panel->cvel[ch]);
}

static void ctl_expression(int ch, int val)
{
	if (!ctl.trace_playing)
		return;
	Panel->channel[ch].expression = val;
	ctl_channel_note(ch, Panel->cnote[ch], Panel->cvel[ch]);
}

static void ctl_panning(int ch, int val)
{
	if (!ctl.trace_playing) 
		return;
	Panel->channel[ch].panning = val;
	Panel->c_flags[ch] |= FLAG_PAN;
}

static void ctl_sustain(int ch, int val)
{
	if (!ctl.trace_playing)
		return;
	Panel->channel[ch].sustain = val;
	Panel->c_flags[ch] |= FLAG_SUST;
}

static void ctl_pitch_bend(int channel, int val)
{
}

static void ctl_reset(void)
{
	int i;

	pipe_printf("RSET %d", ctl.trace_playing);
	if (!ctl.trace_playing)
		return;
	for (i = 0; i < MAX_MIDI_CHANNELS; i++) {
		ctl_program(i, channel[i].program);
		ctl_volume(i, channel[i].volume);
		ctl_expression(i, channel[i].expression);
		ctl_panning(i, channel[i].panning);
		ctl_sustain(i, channel[i].sustain);
		ctl_pitch_bend(i, channel[i].pitchbend);
		ctl_channel_note(i, Panel->cnote[i], 0);
	}
}

/***********************************************************************/
/* OPEN THE CONNECTION                                                */
/***********************************************************************/
static int ctl_open(int using_stdin, int using_stdout)
{
	shm_alloc();
	pipe_open();

	if (child_pid == 0)
		start_panel();

	signal(SIGCHLD, get_child);
	signal(SIGTERM, shm_free);
	signal(SIGINT, shm_free);

	ctl.opened=1;
	return 0;
}

/* Tells the window to disapear */
static void ctl_close(void)
{
	if (ctl.opened) {
		kill(child_pid, SIGTERM);
		shm_free(100);
		ctl.opened=0;
	}
}


/* 
 * Read information coming from the window in a BLOCKING way
 */

/* commands are: PREV, NEXT, QUIT, STOP, LOAD, JUMP, VOLM */

static int ctl_blocking_read(int32 *valp)
{
	char buf[256], *tok, *arg;
	int rc;
	int new_volume;
	int new_centiseconds;

	rc = pipe_gets(buf, sizeof(buf)-1);
	tok = strtok(buf, " ");

	while (1)/* Loop after pause sleeping to treat other buttons! */
	{
		switch (*tok) {
		case 'V':
			if ((arg = strtok(NULL, " ")) != NULL) {
				new_volume = atoi(arg);
				*valp= new_volume - amplification ;
				return RC_CHANGE_VOLUME;
			}
			return RC_NONE;
		  
		case 'J':
			if ((arg = strtok(NULL, " ")) != NULL) {
				new_centiseconds = atoi(arg);
				*valp= new_centiseconds*(play_mode->rate / 100) ;
				return RC_JUMP;
			}
			return RC_NONE;
		  
		case 'Q':
			return RC_QUIT;
		
		case 'L':
			return RC_LOAD_FILE;		  
		  
		case 'N':
			return RC_NEXT;
		  
		case 'P':
			/*return RC_REALLY_PREVIOUS;*/
			return RC_PREVIOUS;
		  
		case 'R':
			return RC_RESTART;
		  
		case 'F':
			*valp=play_mode->rate;
			return RC_FORWARD;
		  
		case 'B':
			*valp=play_mode->rate;
			return RC_BACK;
		}
	  
	  
		if (*tok == 'S') {
			pipe_gets(buf, sizeof(buf)-1);
			tok = strtok(buf, " ");
			if (*tok == 'S')
				return RC_NONE; /* Resume where we stopped */
		}
		else {
			fprintf(stderr,"UNKNOWN RC_MESSAGE %s\n", tok);
			return RC_NONE;
		}
	}

}

/* 
 * Read information coming from the window in a non blocking way
 */
static int ctl_read(int32 *valp)
{
	int num;

	/* We don't wan't to lock on reading  */
	num=pipe_read_ready(); 

	if (num==0)
		return RC_NONE;
  
	return(ctl_blocking_read(valp));
}

static void ctl_pass_playing_list(int number_of_files, char *list_of_files[])
{
	int i=0;
	char local[1000];
	int command;
	int32 val;

	/* Pass the list to the interface */
	pipe_printf("LIST %d", number_of_files);
	for (i=0;i<number_of_files;i++)
		pipe_puts(list_of_files[i]);
    
	/* Ask the interface for a filename to play -> begin to play automatically */
	/*pipe_puts("NEXT");*/
	command = ctl_blocking_read(&val);

	/* Main Loop */
	for (;;)
	{ 
		if (command==RC_LOAD_FILE)
		{
			/* Read a LoadFile command */
			pipe_gets(local, sizeof(local)-1);
			command=play_midi_file(local);
		}
		else
		{
			if (command==RC_QUIT) {
				/* if really QUIT */
				pipe_gets(local, sizeof(local)-1);
				if (*local == 'Z')
					return;
				/* only stop playing..*/
			}
			else if (command==RC_ERROR)
				command=RC_TUNE_END; /* Launch next file */
			else if (command==RC_CHANGE_VOLUME) /* init volume */
				amplification += val;

			switch(command)
			{
			case RC_NEXT:
				pipe_puts("NEXT");
				break;
			case RC_REALLY_PREVIOUS:
				pipe_puts("PREV");
				break;
			case RC_TUNE_END:
				pipe_puts("TEND");
				break;
			case RC_RESTART:
				pipe_puts("RSTA");
				break;
			}
		    
			command = ctl_blocking_read(&val);
		}
	}
}


/* open pipe and fork child process */
static void pipe_open(void)
{
	int res;

	res = pipe(pipeAppli);
	if (res!=0) pipe_error("PIPE_APPLI CREATION");
    
	res = pipe(pipePanel);
	if (res!=0) pipe_error("PIPE_PANEL CREATION");
    
	if ((child_pid = fork()) == 0) {
		/*child*/
		close(pipePanel[1]); 
		close(pipeAppli[0]);
	    
		/* redirect to stdin/out */
		dup2(pipePanel[0], fileno(stdin));
		close(pipePanel[0]);
		dup2(pipeAppli[1], fileno(stdout));
		close(pipeAppli[1]);
	} else {
		close(pipePanel[0]);
		close(pipeAppli[1]);
    
		fpip_in= pipeAppli[0];
		fpip_out= pipePanel[1];
	}
}	    

static int pipe_read_ready()
{
    int num;
    ioctl(fpip_in,FIONREAD,&num); /* see how many chars in buffer. */
    return num;
}


/***********************************************************************/
/* PIPE COMUNICATION                                                   */
/***********************************************************************/

static void pipe_error(char *st)
{
    fprintf(stderr,"CONNECTION PROBLEM WITH TCL/TK PROCESS IN %s BECAUSE:%s\n",
	    st,
	    sys_errlist[errno]);
    exit(1);
}


static void pipe_printf(char *fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	pipe_puts(buf);
}

static void pipe_puts(char *str)
{
	int len;
	char lf = '\n';
	len = strlen(str);
	write(fpip_out, str, len);
	write(fpip_out, &lf, 1);
}


int pipe_gets(char *str, int maxlen)
{
/* blocking reading */
	char *p;
	int len;

	/* at least 5 letters (4+\n) command */
	len = 0;
	for (p = str; ; p++) {
		read(fpip_in, p, 1);
		if (*p == '\n')
			break;
		len++;
	}
	*p = 0;
	return len;
}


/*----------------------------------------------------------------
 * shared memory handling
 *----------------------------------------------------------------*/

static void get_child(int sig)
{
	child_killed = 1;
}

static void shm_alloc(void)
{
	shmid = shmget(IPC_PRIVATE, sizeof(PanelInfo),
		       IPC_CREAT|0600);
	if (shmid < 0) {
		fprintf(stderr, "can't allocate shared memory\n");
		exit(1);
	}
	Panel = (PanelInfo *)shmat(shmid, 0, 0);
	Panel->reset_panel = 0;
	Panel->multi_part = 0;
}

static void shm_free(int sig)
{ 
	kill(child_pid, SIGTERM);
	while (!child_killed)
		;
	shmctl(shmid, IPC_RMID,NULL);
	shmdt((char *)Panel);
	if (sig != 100)
		exit(0);
}

/*----------------------------------------------------------------
 * start Tk window panel
 *----------------------------------------------------------------*/

static void start_panel(void)
{
	char *argv[128];
	int argc;
    
	argc = 0;
	argv[argc++] = "-f";
	argv[argc++] = TKPROGPATH;

	if (ctl.trace_playing) {
		argv[argc++] = "-mode";
		argv[argc++] = "trace";
	}

	/* call Tk main routine */
	Tk_Main(argc, argv, AppInit);

	exit(0);
}


/*----------------------------------------------------------------
 * initialize Tcl application
 *----------------------------------------------------------------*/

static Tcl_Interp *my_interp;

static int AppInit(Tcl_Interp *interp)
{
	my_interp = interp;

	if (Tcl_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
	}
	if (Tk_Init(interp) == TCL_ERROR) {
		return TCL_ERROR;
	}

	Tcl_CreateCommand(interp, "TraceCreate", TraceCreate,
			  (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
	Tcl_CreateCommand(interp, "TraceUpdate", TraceUpdate,
			  (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
	Tcl_CreateCommand(interp, "TraceReset", TraceReset,
			  (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
	Tcl_CreateCommand(interp, "ExitAll", ExitAll,
			  (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
	Tcl_CreateCommand(interp, "TraceUpdate", TraceUpdate,
			  (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
	return TCL_OK;
}

static int ExitAll(ClientData clientData, Tcl_Interp *interp,
		   int argc, char *argv[])
{
	/* window is killed; kill the parent process, too */
	kill(getppid(), SIGTERM);
	for (;;)
		sleep(1000);
}

/* evaluate Tcl script */
static char *v_eval(char *fmt, ...)
{
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	Tcl_Eval(my_interp, buf);
	va_end(ap);
	return my_interp->result;
}

static char *v_get2(char *v1, char *v2)
{
	return Tcl_GetVar2(my_interp, v1, v2, 0);
}


/*----------------------------------------------------------------
 * update Tcl timer / trace window
 *----------------------------------------------------------------*/

#define FRAME_WIN	".body.trace"
#define CANVAS_WIN	FRAME_WIN ".c"

#define BAR_WID 20
#define BAR_HGT 130
#define WIN_WID (BAR_WID * 16)
#define WIN_HGT (BAR_HGT + 11 + 17)
#define BAR_HALF_HGT (WIN_HGT / 2 - 11 - 17)


static int TraceCreate(ClientData clientData, Tcl_Interp *interp,
		       int argc, char *argv[])
{
	int i;
	v_eval("frame %s -bg black", FRAME_WIN);
	v_eval("canvas %s -width %d -height %d -bd 0 -bg black "
	       "-highlightthickness 0",
	       CANVAS_WIN, WIN_WID, WIN_HGT);
	v_eval("pack %s -side top -fill x", CANVAS_WIN);
	for (i = 0; i < 32; i++) {
		char *color;
		v_eval("%s create text 0 0 -anchor n -fill white -text 00 "
		       "-tags prog%d", CANVAS_WIN, i);
		v_eval("%s create poly 0 0 0 0 0 0 -fill yellow -tags pos%d",
		       CANVAS_WIN, i);
		color = (i == 9 || i == 25) ? "red" : "green";
		v_eval("%s create rect 0 0 0 0 -fill %s -tags bar%d "
		       "-outline \"\"", CANVAS_WIN, color, i);
	}
	v_eval("set Stat(TimerId) -1");
	v_eval("TraceReset");
	return TCL_OK;
}

static void trace_bank(int ch, int val)
{
	v_eval("%s itemconfigure bar%d -fill %s",
	       CANVAS_WIN, ch,
	       (val == 128 ? "red" : "green"));
}

static void trace_prog(int ch, int val)
{
	v_eval("%s itemconfigure prog%d -text %02X",
	       CANVAS_WIN, ch, val);
}

static void trace_sustain(int ch, int val)
{
	v_eval("%s itemconfigure prog%d -fill %s",
	       CANVAS_WIN, ch,
	       (val == 127 ? "green" : "white"));
}

static void trace_prog_init(int ch)
{
	int item, yofs, bar, x, y;

	item = ch;
	yofs = 0;
	bar = Panel->multi_part ? BAR_HALF_HGT : BAR_HGT;
	if (ch >= 16) {
		ch -= 16;
		yofs = WIN_HGT / 2;
		if (!Panel->multi_part)
			yofs = -500;
	}
	x = ch * BAR_WID + BAR_WID/2;
	y = bar + 11 + yofs;
	v_eval("%s coords prog%d %d %d", CANVAS_WIN, item, x, y);
}

static void trace_volume(int ch, int val)
{
	int item, bar, yofs, x1, y1, x2, y2;
	item = ch;
	yofs = 0;
	bar = Panel->multi_part ? BAR_HALF_HGT : BAR_HGT;
	if (ch >= 16) {
		yofs = WIN_HGT / 2;
		ch -= 16;
		if (!Panel->multi_part)
			yofs = -500;
	}
	x1 = ch * BAR_WID;
	y1 = bar - 1 + yofs;
	x2 = x1 + BAR_WID - 1;
	y2 = y1 - bar * val / 127;
	v_eval("%s coords bar%d %d %d %d %d", CANVAS_WIN,
	       item, x1, y1, x2, y2);
}

static void trace_panning(int ch, int val)
{
	int item, bar, yofs;
	int x, ap, bp;
	if (val < 0) {
		v_eval("%s coords pos%d -1 0 -1 0 -1 0", CANVAS_WIN, ch);
		return;
	}

	item = ch;
	yofs = 0;
	bar = Panel->multi_part ? BAR_HALF_HGT : BAR_HGT;
	if (ch >= 16) {
		yofs = WIN_HGT / 2;
		ch -= 16;
		if (!Panel->multi_part)
			yofs = -500;
	}

	x = BAR_WID * ch;
	ap = BAR_WID * val / 127;
	bp = BAR_WID - ap - 1;
	v_eval("%s coords pos%d %d %d %d %d %d %d", CANVAS_WIN, item,
	       ap + x, bar + 5 + yofs,
	       bp + x, bar + 1 + yofs,
	       bp + x, bar + 9 + yofs);
}

static int TraceReset(ClientData clientData, Tcl_Interp *interp,
			   int argc, char *argv[])
{
	int i;
	for (i = 0; i < 32; i++) {
		trace_volume(i, 0);
		trace_panning(i, -1);
		trace_prog_init(i);
		trace_prog(i, 0);
		trace_sustain(i, 0);
		Panel->ctotal[i] = 0;
		Panel->cvel[i] = 0;
		Panel->v_flags[i] = 0;
		Panel->c_flags[i] = 0;
	}

	return TCL_OK;
}



#define DELTA_VEL	32

static void update_notes(void)
{
	int i, imax;
	imax = Panel->multi_part ? 32 : 16;
	for (i = 0; i < imax; i++) {
		if (Panel->v_flags[i]) {
			if (Panel->v_flags[i] == FLAG_NOTE_OFF) {
				Panel->ctotal[i] -= DELTA_VEL;
				if (Panel->ctotal[i] <= 0) {
					Panel->ctotal[i] = 0;
					Panel->v_flags[i] = 0;
				}
			} else {
				Panel->v_flags[i] = 0;
			}
			trace_volume(i, Panel->ctotal[i]);
		}

		if (Panel->c_flags[i]) {
			if (Panel->c_flags[i] & FLAG_PAN)
				trace_panning(i, Panel->channel[i].panning);
			if (Panel->c_flags[i] & FLAG_BANK)
				trace_bank(i, Panel->channel[i].bank);
			if (Panel->c_flags[i] & FLAG_PROG)
				trace_prog(i, Panel->channel[i].program);
			if (Panel->c_flags[i] & FLAG_SUST)
				trace_sustain(i, Panel->channel[i].sustain);
			Panel->c_flags[i] = 0;
		}
	}
}

static int TraceUpdate(ClientData clientData, Tcl_Interp *interp,
		    int argc, char *argv[])
{
	char *playing = v_get2("Stat", "Playing");
	if (playing && *playing != '0') {
		if (Panel->reset_panel) {
			v_eval("TraceReset");
			Panel->reset_panel = 0;
		}
		if (Panel->last_time != Panel->cur_time) {
			v_eval("SetTime %d", Panel->cur_time);
			Panel->last_time = Panel->cur_time;
		}
		if (ctl.trace_playing)
			update_notes();
	}
	v_eval("set Stat(TimerId) [after 50 TraceUpdate]");
	return TCL_OK;
}

