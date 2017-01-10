// svec.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

// implement a stretchy vector class:
// An Svec<T> grows automatically (doubles when full), so that adding
// elements to the end has an amortized cost of O(1).

#include "svec.h"

template<class T>
Svec<T>::Svec(int max)
       : alloced(max), used(0)
{
    vect = (T *)malloc(max * sizeof(T));
}

template<class T>
Svec<T>::Svec(const Svec<T> &s)
{
    int n = s.size();
    vect = (T *)malloc(n * sizeof(T));
    alloced = used = n;
    for(int i = 0; i < n; i++)
	vect[i] = s[i];
}

template<class T>
Svec<T>::~Svec()
{
    free(vect);
}

template<class T>
Svec<T> &Svec<T>::operator=(const Svec<T> &s)
{
    if(this != &s) {
	if(alloced < s.size()) {
	    alloced = s.size();
	    vect = (T *)realloc(vect, alloced * sizeof(T));
	}
	for(int i = 0; i < s.size(); i++) {
	    vect[i] = s.vect[i];
	}
	used = s.size();
    }
    return *this;
}

template<class T>
T &Svec<T>::accessref(int index)
{
    if(index < 0)
	fatal("Svec: index out of range");
    while(index >= alloced) grow();
    if(index >= used) used = index + 1;
    return vect[index];
}

template<class T>
T Svec<T>::access(int index) const
{
    if(index < 0 || index >= used)
	fatal("Svec: index out of range");
    return vect[index];
}

template<class T>
void Svec<T>::remove(int index)
{
    if(index < 0 || index >= used)
	fatal("Svec: index out of range");
    for(int j = index; j < used - 1; j++)
	vect[j] = vect[j + 1];
    used--;
}

template<class T>
void Svec<T>::sort(int (*compare)(T *a, T *b))
{
    qsort(vect, used, sizeof(T), (int (*)(const void *, const void *))compare);
}


