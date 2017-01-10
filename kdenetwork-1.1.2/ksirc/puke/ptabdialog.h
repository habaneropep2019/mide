#ifndef PTABDIALOG_H
#define PTABDIALOG_H

class PTabDialog;

#include <qtabdlg.h>
#include "pmessage.h"
#include "pwidget.h"

extern "C" {
PObject *createWidget(CreateArgs &ca);
}


class PTabDialog : public PWidget
{
  Q_OBJECT
 public:
  PTabDialog(QObject *parent = 0);
  virtual ~PTabDialog();

  /**
   * Handles messages from dsirc
   * PObject can't get messages so return an error
   */
  virtual void messageHandler(int fd, PukeMessage *pm);

  /**
   * Sets the current opbect
   */
  virtual void setWidget(QObject *tb);
  
  /**
   * Returns the current object
   */
  virtual QTabDialog *widget();

 private:

  QTabDialog *tab;

};

#endif
