// svec.h					emacs, this is a -*-c++-*- file
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef SVEC_H
#define SVEC_H

#include <stdlib.h>

template<class T>
class Svec
{
public:
    Svec(int max = 16);
    Svec(const Svec<T> &s);
    ~Svec();

    Svec<T> &operator=(const Svec<T> &s);
    int size() const { return used; };
    T &operator[](int index) {
	return (index < 0 || index >= used) ? accessref(index) : vect[index];};
    T operator[](int index) const {
	return (index < 0 || index >= used) ? access(index) : vect[index]; };
    void sort(int (*compare)(T *a, T *b));
    void add(T x) { if(++used > alloced) grow(); vect[used - 1] = x; };
    void remove(int index);
    void clear() { used = 0; };
    int allocated_size() const { return alloced; };

private:
    void grow() { vect = (T *)realloc(vect, (alloced *= 2) * sizeof(T)); };
    T &accessref(int index);
    T access(int index) const;

    T *vect;
    int alloced;		// # of entries allocated
    int used;			// # of entries actually used (size)
};

#endif	// SVEC_H
