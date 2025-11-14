#ifndef ARRAY_DOT_H
#define ARRAY_DOT_H

#include "ints.hh"

template <typename T = fast>
struct Array
{
	static const fast NOT_FOUND  = -1;
	
	static
	bool compare (T _1, T _2)
	{ return _1 == _2; }
	
	fast count, available;
	
	T* ptr;
	
	fast append (T next); // returns # of new elements allocated
	void allocate (fast num);
	fast expand (); // allocate 50% more space (returns # new elements)
	fast shrink (); // deallocate all unused elements (returns # unused elements)
	void clear (); // deallocate all memory
	
	fast find (T element, bool (* Compare)(T,T) = compare); // returns index of first matching element or NOT_FOUND
	
	T pop (); // remove last element
	
	T& operator [] (fast idx) const {return ptr[idx];};
	operator T* () const {return ptr;}
	
	T& first () const;
	T& last () const;
};

#include "array.cc.hh"

#endif
