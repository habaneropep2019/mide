#ifndef _QDATELIST_H
#define _QDATELIST_H

#include <qdatetm.h>
#include <qlist.h>

#if defined(DEFAULT_TEMPLATECLASS)
typedef QListT<QDate>                    QDateListBase;
typedef QListIteratorT<QDate>            QDateListIterator;
#else
typedef Q_DECLARE(QListM,QDate)          QDateListBase;
typedef Q_DECLARE(QListIteratorM,QDate)  QDateListIterator;
#endif


class QDateList : public QDateListBase {
public:
  /**
   *This class is a QList<QDate> instance.  It has the ability to make 
   *  deep or shallow copies, and compareItems() is re-implemented. 
   */

  /**
   *Constructs an empty list of dates. Will make deep copies of all
   * inserted dates if deepCopies is TRUE, or uses shallow copies
   *if deepCopies is FALSE. 
   */
  QDateList(bool deepCopies = TRUE) { deep = deepCopies; };
  QDateList(const QDateList &list);
  /** 
   * Constructs a copy of list. 
   * If list has deep copies, this list will also get deep copies. Only
   * the pointers are copied (shallow copy) if the other list does not use
   * deep copies. 
   */
  virtual ~QDateList(void) { clear(); };
  /** 
   * copies the contents of one list to the other.  If the source list has 
   * deep copies, then deep copies will be made.  Otherwise, only the 
   * pointers will be copied. 
   */
  QDateList &operator=(const QDateList &list);

private:
  GCI newItem(GCI d) { return deep ? new QDate(*(QDate *) d) : (QDate *) d; }
  void deleteItem(GCI d) { if (deep) delete (QDate *) d; }
  inline int compareItems(GCI s1, GCI s2);
  bool  deep;
};

inline QDateList &QDateList::operator=( const QDateList &dateList )
{
    clear();
    deep = dateList.deep;
    QDateListBase::operator=(dateList);
    return *this;
}

inline QDateList::QDateList( const QDateList &dateList )
    : QDateListBase( dateList )
{
    deep = FALSE;
    operator=(dateList);
}

inline int QDateList::compareItems(GCI s1, GCI s2)
{
  if (*((QDate *) s1) == *((QDate *) s2)) 
    return 0;
  if (*((QDate *) s1) < *((QDate *) s2)) 
    return -1; 
  else 
    return 1;
}

#endif
