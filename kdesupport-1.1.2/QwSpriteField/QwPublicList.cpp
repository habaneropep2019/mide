/*
 * Warwick Allison 1/07/95 - Lists with public head/next.
 */

#include "QwPublicList.h"

template <class T>
void QwPublicList<T>::append(QwPublicList<T>* a)
{
    QwPublicList<T>** cursor=&next;
    while ((*cursor)->next) cursor=&(*cursor)->next;
    *cursor=a;
}

template <class T>
void QwPublicList<T>::uniqueify(int compare(const T&, const T&))
{
    QwPublicList<T>* cursor=this;
    while (cursor) {
        while (cursor->next && compare(cursor->next->element,cursor->element)==0) {
            QwPublicList<T>* t=cursor->next;
            cursor->next=cursor->next->next;
            t->next=0;
            delete t;
        }
        cursor=cursor->next;
    }
}

template <class T>
void QwPublicList<T>::uniqueify()
{
    QwPublicList<T>* cursor=this;
    while (cursor) {
        while (cursor->next && cursor->next->element==cursor->element) {
            QwPublicList<T>* t=cursor->next;
            cursor->next=cursor->next->next;
            t->next=0;
            delete t;
        }
        cursor=cursor->next;
    }
}

template <class T>
int QwPublicList<T>::size() const
{
    if (!this) return 0;

    int count=1;

    for (QwPublicList<T>* cursor=next; cursor; cursor=cursor->next)
        count++;

    return count;
}

template <class T>
QwPublicList<T>* QwPublicList<T>::copy() const
{
    if (!this)
        return 0;
    else
        return new QwPublicList<T>(element,next->copy());
}

template<class T>
void PLqsort(QwPublicList<T>* array[], int l, int r, int compare(void*,void*), bool ascending_not_descending)
{
    if (r>l) {
        QwPublicList<T>* v=array[r];
        int i=l-1, j=r;
        for (;;) {
            if (ascending_not_descending) {
                while (compare(&array[++i]->element,&v->element)<0)
                    ;
                while (j>i && compare(&array[--j]->element,&v->element)>0)
                    ;
            } else {
                while (compare(&array[++i]->element,&v->element)>0)
                    ;
                while (j>i && compare(&array[--j]->element,&v->element)<0)
                    ;
            }

            if (i >= j)
                break;
            QwPublicList<T>* t=array[i];
            array[i]=array[j];
            array[j]=t;
        }
        QwPublicList<T>* t=array[i];
        array[i]=array[r];
        array[r]=t;

        PLqsort(array,l,i-1,compare,ascending_not_descending);
        PLqsort(array,i+1,r,compare,ascending_not_descending);
    }
}

template<class T>
void sort(QwPublicList<T>*& list, int compare(void*, void*), bool ascending_not_descending)
{
    // XXX YUCK!  Convert to array to sort with quicksort...
    // XXX    Better to use a list-based sort (merge?).

    int s=list->size();

    if (!s) return;

    QwPublicList<T>** array = new QwPublicList<T>*[s];
    QwPublicList<T>* cursor=list;
    int i;
    for (i=0; i<s; i++) {
        array[i]=cursor;
        cursor=cursor->next;
    }
    PLqsort(array,0,s-1,compare,ascending_not_descending);
    cursor=list=array[0];
    for (i=0; i<s-1; i++) {
        cursor=array[i];
        cursor->next=array[i+1];
    }
    cursor=array[s-1];
    cursor->next=0;
    delete [] array;
}

template<class T>
QwPublicList<T>* remove(T g, QwPublicList<T>*& list)
{
    QwPublicList<T>** cursor=&list;

    while ((*cursor) && (*cursor)->element != g) {
        cursor=&(*cursor)->next;
    }

    QwPublicList<T>* result=*cursor;

    if (result) {
        *cursor=result->next;
        result->next=0;
    }

    return result;
}

template<class T>
QwPublicList<T>* remove(QwPublicList<T>* e, QwPublicList<T>*& list)
{
    QwPublicList<T>** cursor=&list;

    while ((*cursor) && (*cursor) != e) {
        cursor=&(*cursor)->next;
    }

    QwPublicList<T>* result=*cursor;

    if (result) {
        *cursor=result->next;
        result->next=0;
    }

    return result;
}

template <class T>
bool QwPublicList<T>::isOrdered(int compare(const T&, const T&)) const
{
    const QwPublicList<T>* cursor=this;
    while (cursor) {
        if (cursor->next) {
            if (compare(cursor->element, cursor->next->element)>0) {
                return FALSE;
            }
        }
        cursor=cursor->next;
    }
    return TRUE;
}

template <class T>
bool QwPublicList<T>::allDistinct() const
{
    const QwPublicList<T>* cursor=this;
    while (cursor) {
        if (cursor->next) {
            if (cursor->element==cursor->next->element) {
                return FALSE;
            }
        }
        cursor=cursor->next;
    }
    return TRUE;
}
