/**********************************************************************

	--- Qt Architect generated file ---

	File: ksprogress.h
	Last generated: Thu Dec 18 08:01:39 1997

 *********************************************************************/

#ifndef KSProgress_included
#define KSProgress_included

#include "ksprogressdata.h"

class KSProgress : public ksprogressData
{
    Q_OBJECT

public:

    KSProgress
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~KSProgress();

    virtual void setID(QString);
    virtual void setRange(int minValue, int maxValue);
    virtual void setTopText(QString text);
    virtual void setBotText(QString text);

public slots:
    virtual void setValue(int value);
    virtual void cancelPressed();

signals: 
    void cancel();
    void cancel(QString);

private:
    QString id;
    bool canceled;

};
#endif // KSProgress_included
