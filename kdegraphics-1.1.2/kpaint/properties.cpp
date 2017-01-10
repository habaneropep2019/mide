#include <kdebug.h>
#include <qcolor.h>
#include <qtabdlg.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlined.h>
#include <qcombo.h>
#include <qlayout.h>
#include <stdio.h>
#include <kcolordlg.h>
#include <klocale.h>
#include <kapp.h>
#include "properties.h"

propertiesDialog::propertiesDialog(int pages,
				   QWidget *parent, const char *name)
  : QTabDialog(parent, name)
{
  // Create the pages
  lineProps= new linePropertiesWidget(this);
  addTab(lineProps, i18n("Line Properties"));
  fillProps= new fillPropertiesWidget(this);
  addTab(fillProps, i18n("Fill Properties"));

  setCancelButton( i18n( "Cancel" ) );
  setApplyButton( i18n( "Apply" ));
  setPages(pages);

}

QBrush &propertiesDialog::getBrush()
{
  return fillProps->getBrush();
}

QPen &propertiesDialog::getPen()
{
  return lineProps->getPen();
}

void propertiesDialog::setPages(int pages)
{
  // Adjust tabs to suit new tool
  if (pages & 1)
    lineProps->setEnabled(TRUE);
  else
    lineProps->setEnabled(FALSE);

  if (pages & 2)
    fillProps->setEnabled(TRUE);
  else
    fillProps->setEnabled(FALSE);
}


fillPropertiesWidget::fillPropertiesWidget(QWidget *parent)
  : QWidget(parent)
{
#if 0
  // Set the page size
  setMinimumSize(320,320);
  setMaximumSize(320,320);

  // Set the initial colour
  fillColour= red; 
  //
  // Now create the contents
  //

  patternLabel= new QLabel(i18n("Fill Pattern:"), this);
  patternLabel->move(40, 40);

  // Need to use an item that can display a sample of each one
  patternBox= new QComboBox(FALSE, this, 0);
  patternBox->resize(130, patternBox->height());
  patternBox->insertItem(i18n("None"));
  patternBox->insertItem(i18n("Solid"));
  patternBox->insertItem(i18n("Vertical Lines"));
  patternBox->insertItem(i18n("Horizontal Lines"));
  patternBox->insertItem(i18n("Grid"));
  patternBox->insertItem(i18n("Diagonal Lines //"));
  patternBox->insertItem(i18n("Diagonal Lines \\"));
  patternBox->insertItem(i18n("Diagonal Grid"));

  patternBox->insertItem("94%");
  patternBox->insertItem("88%");
  patternBox->insertItem("63%");
  patternBox->insertItem("50%");
  patternBox->insertItem("37%");
  patternBox->insertItem("12%");
  patternBox->insertItem("6%");

  patternBox->insertItem(i18n("Custom"));
  patternBox->move(150, 40);

  customPatternLabel= new QLabel(i18n("Pattern Filename:"), this);
  customPatternLabel->move(40, 100);

  customPatternField= new QLineEdit(this);
  customPatternField->move(150, 100);
  customPatternField->resize(130, customPatternField->height());

  browseButton= new QPushButton(i18n("Browse..."), this);
  browseButton->move(150, 150);

  setFillColourButton= new QPushButton(i18n("Set Fill Colour..."), this);
  setFillColourButton->move(40, 200);
  setFillColourButton->setEnabled(false);
  pattern= new QPixmap();

  connect(setFillColourButton, SIGNAL(clicked()),
	  this, SLOT(setFillColour()) );
#else
  // Set the initial colour
  fillColour= red; 

  //
  // Now create the contents
  //
  QVBoxLayout *vertical= new QVBoxLayout(this, 4);
  QGridLayout *grid= new QGridLayout(2, 2);
  vertical->addStretch();
  vertical->addLayout(grid);

  patternLabel= new QLabel(i18n("Fill Pattern:"), this);
  grid->addWidget(patternLabel, 0, 0);

  // Need to use an item that can display a sample of each one
  patternBox= new QComboBox(FALSE, this, 0);
  patternBox->insertItem(i18n("None"));
  patternBox->insertItem(i18n("Solid"));
  patternBox->insertItem(i18n("Vertical Lines"));
  patternBox->insertItem(i18n("Horizontal Lines"));
  patternBox->insertItem(i18n("Grid"));
  patternBox->insertItem(i18n("Diagonal Lines //"));
  patternBox->insertItem(i18n("Diagonal Lines \\"));
  patternBox->insertItem(i18n("Diagonal Grid"));
  patternBox->insertItem("94%");
  patternBox->insertItem("88%");
  patternBox->insertItem("63%");
  patternBox->insertItem("50%");
  patternBox->insertItem("37%");
  patternBox->insertItem("12%");
  patternBox->insertItem("6%");
  grid->addWidget(patternBox, 0, 1);

  //  patternBox->insertItem(i18n("Custom"));

  customPatternLabel= new QLabel(i18n("Pattern Filename:"), this);
  grid->addWidget(customPatternLabel, 1, 0);

  customPatternField= new QLineEdit(this);
  grid->addWidget(customPatternField, 1, 1);

  //  browseButton= new QPushButton(i18n("Browse..."), this);

  vertical->addStretch();

  pattern= new QPixmap();
  vertical->activate();
#endif
}

void fillPropertiesWidget::setFillColour()
{
  QColor mycol;

 if (KColorDialog::getColor(mycol))
   fillColour= mycol;
}


QBrush &fillPropertiesWidget::getBrush()
{
  b.setColor(fillColour);

  switch (patternBox->currentItem()) {
  case 0:
    b.setStyle(NoBrush);
    break;
  case 1:
    b.setStyle(SolidPattern);
    break;
  case 2:
    b.setStyle(VerPattern);
    break;
  case 3:
    b.setStyle(HorPattern);
    break;
  case 4:
    b.setStyle(CrossPattern);
    break;
  case 5:
    b.setStyle(BDiagPattern);
    break;
  case 6:
    b.setStyle(FDiagPattern);
    break;
  case 7:
    b.setStyle(DiagCrossPattern);
    break;

    // Greys
  case 8:
    b.setStyle(Dense1Pattern);
    break;
  case 9:
    b.setStyle(Dense2Pattern);
    break;
  case 10:
    b.setStyle(Dense3Pattern);
    break;
  case 11:
    b.setStyle(Dense4Pattern);
    break;
  case 12:
    b.setStyle(Dense5Pattern);
    break;
  case 13:
    b.setStyle(Dense6Pattern);
    break;
  case 14:
    b.setStyle(Dense7Pattern);
    break;

    // Custom Pattern 
  case 15:
    pattern->load(customPatternField->text());
    b.setPixmap(*pattern); 
    break;

  default:
    b.setStyle(Dense7Pattern);
    break;
  }
  return b;
}

QPen &linePropertiesWidget::getPen()
{
  p.setColor(lineColour);
  p.setWidth(widthBox->currentItem()+1);

  switch (styleBox->currentItem()) {
  case 0:
    p.setStyle(SolidLine);
    break;
  case 1:
    p.setStyle(DashLine);
    break;
  case 2:
    p.setStyle(DotLine);
    break;
  case 3:
    p.setStyle(DashDotLine);
    break;
  case 4:
    p.setStyle(DashDotDotLine);
    break;
  case 5:
    p.setStyle(NoPen);
    break;
  default:
    p.setStyle(DotLine);
    break;
  }
  return p;
}

linePropertiesWidget::linePropertiesWidget(QWidget *parent)
  : QWidget(parent)
{
#if 0
  setMinimumSize(320,320);
  setMaximumSize(320,320);

  lineColour= blue;
   
  //
  // Now create the contents
  //

  styleLabel= new QLabel(i18n("Line Style:"), this);
  styleLabel->move(40, 40);

  // These should have a picture
  styleBox= new QComboBox(FALSE, this, 0);
  styleBox->insertItem(i18n("Solid"));
  styleBox->insertItem(i18n("Dashed"));
  styleBox->insertItem(i18n("Dotted"));
  styleBox->insertItem(i18n("Dash Dot"));
  styleBox->insertItem(i18n("Dash Dot Dot"));
  styleBox->insertItem(i18n("None"));
  styleBox->move(150, 40);

  
  widthLabel= new QLabel(i18n("Line Width:"), this);
  widthLabel->move(40, 100);

  widthBox= new QComboBox(TRUE, this, 0);
  widthBox->insertItem("1");
  widthBox->insertItem("2");
  widthBox->insertItem("3");
  widthBox->insertItem("4");
  widthBox->move(150, 100);

  endTypeLabel= new QLabel(i18n("End style:"), this);
  endTypeLabel->setEnabled(FALSE);
  endTypeLabel->move(40, 150);

  endTypeBox= new QComboBox(FALSE, this, 0);
  endTypeBox->insertItem(i18n("Not Yet"));
  //  endTypeBox->insertItem(i18n("Rounded"));
  //  endTypeBox->insertItem(i18n("Arrow"));
  //  endTypeBox->insertItem(i18n("Flat"));
  endTypeBox->setEnabled(FALSE);
  endTypeBox->move(150, 150);

  joinStyleLabel= new QLabel(i18n("Join style:"), this);
  joinStyleLabel->setEnabled(FALSE);
  joinStyleLabel->move(40, 200);

  joinStyleBox= new QComboBox(FALSE, this, 0);
  joinStyleBox->setEnabled(FALSE);
  joinStyleBox->insertItem(i18n("Not Yet"));
  //  joinStyleBox->insertItem(i18n("Rounded"));
  //  joinStyleBox->insertItem(i18n("Straight"));
  joinStyleBox->move(150, 200);


  setLineColourButton= new QPushButton(i18n("Set Line Colour..."), this);
  setLineColourButton->move(40, 240);
  connect(setLineColourButton, SIGNAL(clicked()), this, SLOT(setLineColour()) );
  setLineColourButton->setEnabled(false);
#else
  lineColour= blue;

  QVBoxLayout *vertical= new QVBoxLayout(this, 4);

  //  vertical->addStretch();
  QGridLayout *grid= new QGridLayout(4, 2);
  vertical->addLayout(grid);

  styleLabel= new QLabel(i18n("Line Style:"), this);
  grid->addWidget(styleLabel, 0, 0);

  styleBox= new QComboBox(FALSE, this, 0);
  styleBox->insertItem(i18n("Solid"));
  styleBox->insertItem(i18n("Dashed"));
  styleBox->insertItem(i18n("Dotted"));
  styleBox->insertItem(i18n("Dash Dot"));
  styleBox->insertItem(i18n("Dash Dot Dot"));
  styleBox->insertItem(i18n("None"));
  grid->addWidget(styleBox, 0, 1);

  widthLabel= new QLabel(i18n("Line Width:"), this);
  grid->addWidget(widthLabel, 1, 0);

  widthBox= new QComboBox(TRUE, this, 0);
  widthBox->insertItem("1");
  widthBox->insertItem("2");
  widthBox->insertItem("3");
  widthBox->insertItem("4");
  grid->addWidget(widthBox, 1, 1);

  endTypeLabel= new QLabel(i18n("End style:"), this);
  grid->addWidget(endTypeLabel, 2, 0);

  endTypeBox= new QComboBox(FALSE, this, 0);
  endTypeBox->insertItem(i18n("Not Yet"));
  grid->addWidget(endTypeBox, 2, 1);

  joinStyleLabel= new QLabel(i18n("Join style:"), this);
  grid->addWidget(joinStyleLabel, 3, 0);

  joinStyleBox= new QComboBox(FALSE, this, 0);
  joinStyleBox->insertItem(i18n("Not Yet"));
  grid->addWidget(joinStyleBox, 3, 1);
  joinStyleBox->setEnabled(FALSE);

  //  vertical->addStretch();

  vertical->activate();
#endif
}

void linePropertiesWidget::setLineColour()
{
  QColor mycol;

 if (KColorDialog::getColor(mycol))
   lineColour= mycol;
}

#include "properties.moc"
