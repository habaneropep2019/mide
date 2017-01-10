/* QwPublicList.cpp
 *
 * Warwick Allison 1/07/95 - Lists with public head/next.
 */

#ifndef PublicList_h
#define PublicList_h

#include <assert.h>
#include <stdlib.h>

template <class T>
class QwPublicList {
public:
    QwPublicList(const T& e, QwPublicList* n=0) : element(e), next(n) { }
    ~QwPublicList()
    {
        delete next;
    }

    void append(QwPublicList<T>* a); // O(n)
    int size() const;

    // Pre: sorted (see Sort below)
    void uniqueify(int compare(const T&, const T&));
    void uniqueify(); // Uses operator==

    QwPublicList<T>* copy() const;

    bool isOrdered(int compare(const T&, const T&)) const;
    bool allDistinct() const; // Pre: isOrdered()

    T element;
    QwPublicList<T>* next;
};

template <class T>
void sort(QwPublicList<T>*& list, int compare(void*, void*), bool ascending_not_descending);

template <class T>
void sort(QwPublicList<T>*& list, int compare(void*, void*))
{ sort(list, compare, TRUE); }

template <class T>
QwPublicList<T>* remove(T, QwPublicList<T>*& list); // O(n)

template <class T>
QwPublicList<T>* remove(QwPublicList<T>*, QwPublicList<T>*& list); // O(n)

template <class T>
void prepend(const T& e, QwPublicList<T>*&);


template <class T>
inline void prepend(const T& e, QwPublicList<T>*& l)
{
    l=new QwPublicList<T>(e, l);
}

#endif
