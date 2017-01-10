#ifndef KEYS_H
#define KEYS_H

#include <qdialog.h>
#include <qlabel.h>
#include <qstring.h>

class Keys : public QDialog
{
    Q_OBJECT
public:
    Keys( QWidget *parent=0, const char *name=0 );
private slots:
    void butRight();
    void butLeft();
    void butUp();
    void butDown();

    void getKey(int);
    void defaults();
    void focusIn(QLabel *);
    void focusOut(QLabel *);

    void ok();

protected:
    void  keyPressEvent( QKeyEvent * );
private:

    void init();

    QLabel *labels[4];
    QLabel *lab;
};




#endif // KEYS_H
