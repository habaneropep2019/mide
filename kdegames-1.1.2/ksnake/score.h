#ifndef SCORE_H
#define SCORE_H

#include <qdialog.h>
#include <qdatetm.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qgrpbox.h>
#include <qlined.h>
#include <qstring.h>
#include <qfile.h>

class ScoreBox : public QWidget
{
    Q_OBJECT
public:
    ScoreBox( QWidget *parent=0, const char *name=0, int current = -1);

    void setTitle( const char *);
    void setScore(int, int, const char *, const char *);

protected:
    void resizeEvent( QResizeEvent * );

private:
    QGroupBox   *box;
    QLabel      *label;

    QLabel *date[5];
    QLabel *points[5];
    QLabel *player[5];
};

class Score : public QObject
{
    Q_OBJECT
public:
    Score( QWidget *parent=0, const char *name=0);

public slots:
    void setScore(int);
    void display(int newHall, int newToday = -1);

private:

    struct {
	QDate date;
	int  points;
	char *player;
    } hall[5], today[5];


    void read();
    void write();

    QString formatDate(int, int, int);
    QString getPlayerName();
    QString playerName;
    QFile file;
};


#endif // SCORE_H
