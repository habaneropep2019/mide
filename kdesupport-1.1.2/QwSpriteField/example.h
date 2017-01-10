#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <qpopmenu.h>
#include <qintdict.h>
#include "QwSpriteField.h"

class MyPopupMenu : public QPopupMenu {
    Q_OBJECT
public:
    MyPopupMenu();
    ~MyPopupMenu();

    int insertCheckItem(const char* text, bool* var, int accel=0);
    int insertRadioItem(const char* text, int* var, int value, int accel=0);

signals:
    void variableChanged(bool*);
    void variableChanged(int*);

public slots:
    void checkCheck(int id);

private:
    static QIntDict<bool> bools;
    static QIntDict<int> ints;
    static QIntDict<int> values;

private slots:
    void dummy();
};

class MySpriteField : public QwImageSpriteField {
public:
    MySpriteField(const char* imagefile, int w, int h, int chunksize=16, int maxclusters=200);

    void resize(int width, int height);
    void drawForeground(QPainter& painter, const QRect& r);

private:
    QRect textarea;
};

class Example : public QWidget {
    Q_OBJECT

public:
    Example(bool scrbars, MySpriteField& sfield, QWidget* parent=0,
	const char* name=0, WFlags f=0);

signals:
    void status(const char*);
    void refreshSlaves();

public slots:
    void refresh();
    void makeSlave();
    void makePolygon();
    void makeEllipse();
    void makeRectangle();

protected:
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent*);
    void timerEvent(QTimerEvent*);

private:
    void setCount(int);
    MySpriteField& field;
    QWidget* view;
    QwRealMobileSprite** sprite;
    QwSpritePixmapSequence* images;
    QwTextSprite textsprite;
    QwSprite *drawnsprite;
    QwPolygon *polygon;
    QPoint mpos;
    bool scrollbars;
    int count;
};

#endif
