#ifndef OPTIONAL_H
#define OPTIONAL_H

#include "ints.hh"

template <typename T>
struct Opt
{
	T value;
	byte valid;
	
	Opt () = default;
	Opt (T Value) { value = Value, valid = true; }
	
	bool operator ! () const
	{ return not valid; }
	
	operator T () const { return value; }
};

#endif
