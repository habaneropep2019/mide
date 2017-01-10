#ifndef __ANSTL__
#define __ANSTL_

template <class a,class b>
class pair {
public:
    
    a first;
    b second;

    pair(const a& x,const b& y) : first(x),second(y) { }
};

template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f)
{
    while (first != last) f(*first++);
    return f;
}

template <class InputIterator,class T>
InputIterator find(InputIterator first,
		   InputIterator last, const T& value)
{
    while(first!=last) {
	if(*first==value)
	    return first;
	else
	    first++;
    }
}

template <class InputIterator,class Predicate>
InputIterator find_if(InputIterator first,
		   InputIterator last, Predicate p)
{
    while(first!=last) {
	if(p(*first))
	    return first;
	else
	    first++;
    }
}


#endif


