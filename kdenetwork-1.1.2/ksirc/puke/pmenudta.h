#ifndef PMENUDATA_H
#define PMENUDATA_H

class PMenuData;

#include <qobject.h>
#include <qmenudta.h>
#include "pmessage.h"
#include "pframe.h"
#include "pobject.h"
#include "controller.h"

// Init and setup code
extern "C" {
PObject *createWidget(CreateArgs &ca);
}

/**
 * Little helper class here gives us access to needed info inside
 * QMenuData
 */

class PMenuDataHelper : public QMenuData
{
public:
  PMenuDataHelper(QMenuData &qmd) {
    memcpy(this, &qmd, sizeof(QMenuData));
  }
  int active() {
    return QMenuData::actItem;
  }
  int actItem;
};

/**
 * We're subclased off QMenuData so we can access it's internal proteted vars
 * We do not initialize NOR create it!!!!
 */

class PMenuData :  public PObject
{
  Q_OBJECT
public:
  PMenuData (PObject *_child);
  virtual ~PMenuData ();

  virtual void messageHandler(int fd, PukeMessage *pm);
  virtual bool menuMessageHandler(int fd, PukeMessage *pm);

//  virtual void setWidget(QMenuData *_qmd);
 // virtual QMenuData *widget();

  virtual int activeItem()
    {
      return PMenuDataHelper(*((QMenuData *) child->widget())).active();
    }

 private:
   PObject *child;
};

#endif
