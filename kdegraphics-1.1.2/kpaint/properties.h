#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <qtabdlg.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlined.h>
#include <qcombo.h>
#include <qbrush.h>
#include <qpen.h>
#include <qpixmap.h>

class fillPropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  fillPropertiesWidget(QWidget *parent);
  QBrush &getBrush();

public slots:
    void setFillColour();
private:
  QComboBox *patternBox;
  QLabel *patternLabel;
  QLabel *customPatternLabel;
  QLineEdit *customPatternField;
  QPushButton *browseButton;
  QPushButton *setFillColourButton;
  QColor fillColour;
  QPixmap *pattern;
  QBrush b;
};


class linePropertiesWidget : public QWidget
{
  Q_OBJECT
public:
  linePropertiesWidget(QWidget *parent);
  QPen &getPen();
public slots:
    void setLineColour();
private:
  QComboBox *styleBox;
  QLabel *styleLabel;
  QComboBox *widthBox;
  QLabel *widthLabel;
  QComboBox *endTypeBox;
  QLabel *endTypeLabel;
  QComboBox *joinStyleBox;
  QLabel *joinStyleLabel;
  QPushButton *setLineColourButton;
  QColor lineColour;
  QPen p;
};

class propertiesDialog  : public QTabDialog
{
  Q_OBJECT
public:
  propertiesDialog(int pages, QWidget *parent= 0, const char *name= 0);

  QBrush &getBrush();
  QPen &getPen();
  void setPages(int);

private:
  linePropertiesWidget *lineProps;
  fillPropertiesWidget *fillProps;
};

#endif // PROPERTIES_H
