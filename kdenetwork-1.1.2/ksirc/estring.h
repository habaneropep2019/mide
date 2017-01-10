#ifndef ESTRING_H
#define ESTRING_H

#include <qstring.h>
#include <kstring.h>

class estringOutOfBounds {
public:
  QString str;
  int index;

  estringOutOfBounds(QString _str, int _index)
    {
      str = _str;
    }
};

class EString : public QString
  {
  public:
    // Setup a few copy operators to be compatible with qstring
    EString() : QString()
      {
      }
    EString (const QString &s);
    EString &operator= (const QString &s);
    // We'll just iherite most things, include constructors, etc
    int find (char c, int index=0, bool cs=TRUE) const throw(estringOutOfBounds);
    int find (const char * str, int index=0, bool cs=TRUE ) const throw(estringOutOfBounds);
    int find (const QRegExp &, int index=0 ) const throw(estringOutOfBounds);
  };

#endif

