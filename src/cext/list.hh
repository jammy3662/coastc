#ifndef LIST_HH
#define LIST_HH

#include "ints.hh"

//
// Opaque wrapper for any contiguous buffer
// with elements of a single type.
//

template
<typename T>

struct List
{
	T* ptr;
	fast count;
};

#endif
