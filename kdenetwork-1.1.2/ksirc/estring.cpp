#include "estring.h"

EString::EString(const QString &s) {
  QString::operator=(s);
}

EString &EString::operator= (const QString &s)
{
  QString::operator=(s);
  return *this;
}

int EString::find ( char c, int index, bool cs) const throw(estringOutOfBounds){
  int ret = QString::find(c, index, cs);
  if(ret < 0)
    throw(estringOutOfBounds(*this, ret));
  return ret;
}
int EString::find ( const char * str, int index, bool cs ) const throw(estringOutOfBounds){
  int ret = QString::find(str, index, cs);
  if(ret < 0)
    throw(estringOutOfBounds(*this, ret));
  return ret;
}
int EString::find ( const QRegExp &exp, int index ) const throw(estringOutOfBounds){
  int ret = QString::find(exp, index);
  if(ret < 0)
    throw(estringOutOfBounds(*this, ret));
  return ret;
}

