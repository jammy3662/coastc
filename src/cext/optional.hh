#ifndef OPTIONAL_H
#define OPTIONAL_H

#include "ints.hh"

template <typename T>
struct Opt: T
{
	byte valid;
	
	Opt () = default;
	Opt (T Value) { *this = Value; valid = true; }
	
	operator bool () const
	{ return valid; }
};

#endif
