// The interface for komailclient class
// $Id: komailclient.h,v 1.1 1999/01/19 15:02:33 pbrown Exp $
#ifndef _KOMAILCLIENT_H
#define _KOMAILCLIENT_H

#define COMMA ","
#define XMAILER "X-Mailer: KOrganizer "
#define SUBJECT Meeting Notification

#include "kdpevent.h"
#include "calobject.h"

/**
 * A Class to maintain the Mailing Headers
 */
class MailMsgString 
{
 public:
    MailMsgString();
    virtual ~MailMsgString();
    void addAddressee(Attendee *);
    void addFrom( char *);
    /**
     * Method to build a plain text (text/plain) message body
     */
    void buildTextMsg(KDPEvent *);
    /**
     * This method returns the Headers in a QString object.
     */
    QString * getHeaders();
    /**
     * This method returns the body of a mail msg
     */
    QString * getBody() { return textString; }

    public slots:

 protected:
    int numberOfAddresses;
    QString * xMailer;
    QString * Addressee;
    QString * From;
    QString * Subject;
    QString * Headers;
    QString * textString;
};

class KoMailClient :  public QObject
{
    Q_OBJECT

public:
    KoMailClient(CalObject *cal);
    virtual ~KoMailClient();
    
public slots:
    void emailEvent(KDPEvent *,QWidget *);

protected:

    CalObject * calendar;
};

#endif
