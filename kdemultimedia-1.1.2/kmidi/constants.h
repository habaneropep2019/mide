#define RC_ERROR -1
#define RC_NONE 0
#define RC_QUIT 1
#define RC_NEXT 2
#define RC_PREVIOUS 3 /* Restart this song at beginning, or the previous
			 song if we're less than a second into this one. */
#define RC_FORWARD 4
#define RC_BACK 5
#define RC_JUMP 6
#define RC_TOGGLE_PAUSE 7 /* Pause/continue */
#define RC_RESTART 8 /* Restart song at beginning */

#define RC_PAUSE 9 /* Really pause playing */
#define RC_CONTINUE 10 /* Continue if paused */
#define RC_REALLY_PREVIOUS 11 /* Really go to the previous song */
#define RC_CHANGE_VOLUME 12
#define RC_LOAD_FILE 13		/* Load a new midifile */
#define RC_TUNE_END 14		/* The tune is over, play it again sam? */
#define RC_TRY_OPEN_DEVICE 15

#define CMSG_INFO	0
#define CMSG_WARNING	1
#define CMSG_ERROR	2
#define CMSG_FATAL	3
#define CMSG_TRACE	4
#define CMSG_TIME	5
#define CMSG_TOTAL	6
#define CMSG_FILE	7
#define CMSG_TEXT	8

#define VERB_NORMAL	0
#define VERB_VERBOSE	1
#define VERB_NOISY	2
#define VERB_DEBUG	3
#define VERB_DEBUG_SILLY	4

/* 
 * MESSAGE FROM KERNEL TO MOTIF
 */
#define REFRESH_MESSAGE 1
#define HELPMODE_MESSAGE 2
#define TOTALTIME_MESSAGE 3
#define MASTERVOL_MESSAGE 4
#define FILENAME_MESSAGE 5
#define CURTIME_MESSAGE 6
#define NOTE_MESSAGE 7
#define PROGRAM_MESSAGE 8
#define VOLUME_MESSAGE 9
#define EXPRESSION_MESSAGE 10
#define PANNING_MESSAGE 11
#define SUSTAIN_MESSAGE 12
#define PITCH_MESSAGE 13
#define RESET_MESSAGE 14
#define CLOSE_MESSAGE 15
#define CMSG_MESSAGE 16
#define FILE_LIST_MESSAGE 17
#define NEXT_FILE_MESSAGE 18
#define PREV_FILE_MESSAGE 19
#define TUNE_END_MESSAGE 20
#define DEVICE_OPEN 21
#define DEVICE_NOT_OPEN 22
/* 
 * MESSAGE ON THE PIPE FROM MOTIF TOWARD KERNEL
 */
#define MOTIF_CHANGE_VOLUME 1
#define MOTIF_CHANGE_LOCATOR 2
#define MOTIF_QUIT 3
#define MOTIF_PLAY_FILE 4
#define MOTIF_NEXT 5
#define MOTIF_PREV 6
#define MOTIF_RESTART 7
#define MOTIF_FWD 8
#define MOTIF_RWD 9
#define MOTIF_PAUSE 10
#define TRY_OPEN_DEVICE 11

