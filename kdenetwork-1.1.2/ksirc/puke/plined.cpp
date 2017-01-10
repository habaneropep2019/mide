#include "plined.h"
#include <stdio.h>

PObject *createWidget(CreateArgs &ca)
{
  PLineEdit *pw = new("PLineEdit") PLineEdit(ca.parent);
  QLineEdit *le;
  if(ca.parent != 0 && ca.parent->widget()->isWidgetType() == TRUE)
    le = new("QLineEdit") QLineEdit((QWidget *) ca.parent->widget());
  else
    le = new("QLineEdit") QLineEdit();
  pw->setWidget(le);
  pw->setWidgetId(ca.pwI);
  return pw;
}


PLineEdit::PLineEdit(PObject *parent)
  : PWidget(parent)
{
  //  debug("PLineEdit PLineEdit called");
  lineedit = 0;
  setWidget(lineedit);
}

PLineEdit::~PLineEdit()
{
  //  debug("PLineEdit: in destructor");
  /*
  delete widget();     // Delete the frame
  lineedit=0;          // Set it to 0
  setWidget(lineedit); // Now set all widget() calls to 0.
  */
}

void PLineEdit::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_LINED_SET_MAXLENGTH:
    if(widget() == 0){
      debug("PLineEdit: No Widget set");
      return;
    }
    widget()->setMaxLength(pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->maxLength();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LINED_SET_ECHOMODE:
    if(widget() == 0){
      debug("PLineEdit: No Widget set");
      return;
    }
    widget()->setEchoMode((QLineEdit::EchoMode) pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = widget()->echoMode();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LINED_SET_TEXT:
    if(widget() == 0){
      debug("PLineEdit: No Widget set");
      return;
    }
    debug("PukeLine Edit: Got: %s", pm->cArg);
    widget()->setText(pm->cArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.iTextSize = strlen(widget()->text());
    pmRet.cArg = new char[strlen(widget()->text())+1];
    strcpy(pmRet.cArg, widget()->text());
    emit outputMessage(fd, &pmRet);
    delete[] pmRet.cArg;
    break;
  case PUKE_LINED_GET_TEXT:
    if(widget() == 0){
      debug("PLineEdit: No Widget set");
      return;
    }
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.iTextSize = strlen(widget()->text());
    pmRet.cArg = new char[strlen(widget()->text())+1];
    strcpy(pmRet.cArg, widget()->text());
    emit outputMessage(fd, &pmRet);
    delete[] pmRet.cArg;
    break;
  default:
    PWidget::messageHandler(fd, pm);
  }
}

void PLineEdit::setWidget(QObject *_le)
{
  if(_le != 0 && _le->inherits("QLineEdit") == FALSE)
    throw(errorInvalidSet(_le, className()));

  lineedit = (QLineEdit *) _le;
  if(lineedit != 0){
    connect(lineedit, SIGNAL(textChanged(const char *)),
	    this, SLOT(updateText(const char *)));
    connect(lineedit, SIGNAL(returnPressed()),
	    this, SLOT(returnPress()));
  }
  PWidget::setWidget(_le);

}


QLineEdit *PLineEdit::widget()
{
  return lineedit;
}

void PLineEdit::updateText(const char *text){
  PukeMessage pmRet;

  pmRet.iCommand = PUKE_LINED_GET_TEXT_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = 0;
  pmRet.iTextSize = strlen(text);
  pmRet.cArg = new char[strlen(text)+1];
  strcpy(pmRet.cArg, text);
  emit outputMessage(widgetIden().fd, &pmRet);
  delete[] pmRet.cArg;
}

void PLineEdit::returnPress() {
  PukeMessage pmRet;

  pmRet.iCommand = PUKE_LINED_RETURN_PRESSED_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = 0;
  pmRet.cArg = 0;
  emit outputMessage(widgetIden().fd, &pmRet);
}

#include "plined.moc"

