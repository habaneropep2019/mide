#ifndef KRN_SENDER_H
#define KRN_SENDER_H

#include <qobject.h>

#include "kmsender.h"
#include "NNTP.h"

class KRNSender: public KMSender
{
    Q_OBJECT
public:
    void setNNTP(NNTP *);
    bool send(KMMessage *aMsg, short sendNow=-1);
    bool sendQueued (const char *fname);
private:
    bool doSendNNTP(KMMessage *msg);
    bool sendNow(KMMessage *aMsg);
    bool queue(KMMessage *aMsg);
    NNTP *server;
};

#endif

