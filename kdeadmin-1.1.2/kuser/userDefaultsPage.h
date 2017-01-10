/**********************************************************************

	--- Qt Architect generated file ---

	File: userDefaultsPage.h
	Last generated: Mon Oct 12 00:56:45 1998

 *********************************************************************/

#ifndef userDefaultsPage_included
#define userDefaultsPage_included

#include "userDefaultsPageData.h"

class userDefaultsPage : public userDefaultsPageData
{
    Q_OBJECT

public:

    userDefaultsPage
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~userDefaultsPage();

  const char *getShell() const;
  const char *getHomeBase() const;
  bool getCreateHomeDir() const;
  bool getCopySkel() const;
  bool getUserPrivateGroup() const;

  void setShell(const char *data);
  void setHomeBase(const char *data);
  void setCreateHomeDir(bool data);
  void setCopySkel(bool data);
  void setUserPrivateGroup(bool data);
};
#endif // userDefaultsPage_included
