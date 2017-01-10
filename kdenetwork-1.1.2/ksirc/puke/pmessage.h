#ifndef PUKE_MESSAGE_H
#define PUKE_MESSAGE_H

typedef struct {
  unsigned int iHeader;   // Filled in durring in PukeController, do not set
  int iCommand;
  int iWinId;
  int iArg;
  int iTextSize; // Size of the text message that follows
  char *cArg;
} PukeMessage;

typedef struct {
  int fd;
  int iWinId;
} widgetId;

const uint iPukeHeader = 42U;

class errorInvalidSet {
public:
  errorInvalidSet(QObject *_from, const char *_to)
    : __from(_from), __to(_to)
  {
  }

  QObject *from() { return __from; }
  const char *to() { return __to; }
    
private:
  QObject *__from;
  const char *__to;

};


#endif
