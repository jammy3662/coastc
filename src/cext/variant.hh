#ifndef VARIANT_HH
#define VARIANT_HH

#include "ints.hh"

struct Variant
{
	void* ptr = 0;
	
	Variant () = default;
	
	template <typename T>
	Variant (T value)
	{
		if (ptr isnt 0) throw "Variant value erased by assignment of value";
		
		ptr = new T (value);
	}
	
	bool operator ! ()
	{ return ptr is 0; }
	
	template <typename T>
	operator T& ()
	{
		if (no ptr) ptr = new T;
		
		return *((T*)ptr);
	}
	
	template <typename T>
	T& operator ()()
	{
		return (T&) *this;
	}
};

#endif
