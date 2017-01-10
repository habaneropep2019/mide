#ifndef PROGRESS_H
#define PROGRESS_H

#include <qobject.h>
#include <kprogress.h>

class Progress : public KProgress
{
    Q_OBJECT
public:
    Progress( QWidget *parent=0, const char *name=0 );
public slots:
    void advance();
    void rewind();
signals:
    void restart();
};

#endif // PROGRESS_H
