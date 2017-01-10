#ifndef CONTROL_MESSAGE_H
#define CONTROL_MESSAGE_H

  // 001 is change channel
  // 002 is stop writting to mainw
  // 003 is resume updating mainw and flush buffer

#define CHANGE_CHANNEL 001
#define STOP_UPDATES   002
#define RESUME_UPDATES 003
#define REREAD_CONFIG  004
#define SET_LAG        005

#endif
