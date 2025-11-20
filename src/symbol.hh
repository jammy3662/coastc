#ifndef SYMBOL_DOT_H
#define SYMBOL_DOT_H

#include <bit>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "optional.hh"
#include "list.hh"
#include "ints.hh"

#define map std::unordered_map
#define multimap std::unordered_multimap
#define array std::vector

//----------------------------------------------------------------
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
//----------------------------------------------------------------

struct Name
{
	array <char*> path;
	bool global; // whether to start from the root of the symbol tree
	
	inline
	char* base () const { return path.back(); }
};

/*-------------------------------------
 | Refers to a symbol
 | by direct pointer or unresolved path
 |_____________________________________*/
template <typename T = Symbol>
struct Ref
{
	Name path;
	T* ptr;
	
	Ref () = default;
	
	Ref (Name Path)
	{ path = Path; }
	
	Ref (T* symbol)
	{ ptr = symbol; }
};
using Reference = Ref <>;

struct      Scope;
struct        Log;
struct   Operator;
struct Expression;
struct   Iterator;
struct       Type;

//----------------------------------------------------------------
// Symbol
//----------------------------------------------------------------

struct Symbol
{
	struct qualifiers
	{
		unsigned byte
		Local  : 1, // hidden from outer scopes
		Static : 1, // only one instance of symbol
		Extern : 1, // allocate in outer scope, not enclosing scope
		Inline :	1; // copy definition directly into its scope
	}
	qualifiers;
	
	enum kind: least
	{
		NONE = 0,   // error / invalid state
		UNRESOLVED, // symbol with unrecognized target path
		
		// Namespacing //
		// ----------- //
		ALIAS,    // local name for an existing symbol
		INCLUDE,  // non-destructive merge with a different symbol
		
		// Scopes //
		// ------ //
		MODULE,
		STRUCT,
		UNION,
		ENUM,
		STACK_FRAME,
		FRAME = STACK_FRAME,
		
		// Controls //
		// -------- //
		IF,
		WHILE,
		DO_WHILE,
		FOR,
		
		// Stack / Locals //
		// -------------- //
		VARIABLE,   // storage class value (mutable, addressable)
		EXPRESSION, // temporary class value (immutable, non-addressable)
		
		// Definitions //
		// ----------- //
		FUNCTION,  // function or method
		OPERATOR,  // arithmetic evaluation for given type(s)
		ITERATOR,  // sequential accessor for a container
		ALLOCATOR, // memory management handler
		
		// Markers //
		// ------- //
		MARKER, CASE, DEFAULT,
		
		// Jumps //
		// ----- //
		GOTO, CONTINUE, BREAK,
		RETURN,
	}
	kind;
	
	Ref <Scope> parent; // where symbol is defined
	Ref <Scope> target; // desired path to parent
	
	union
	{ Reference alias, source; };
};

//----------------------------------------------------------------
// Containers
//----------------------------------------------------------------

struct Scope: Symbol
{
	array <Symbol*>
	symbols;
	
	multimap <std::string, fast>
	nametable;
	
	using Opcode = fast;
	
	map <Opcode, Operator*>
	operators;
	
	array <Log>
	logs; // notes, warnings, & errors
};

struct Conditional: Scope
{
	Expression* condition;
};

struct If: Conditional
{
	Scope* skip; // else block
};

struct While: Conditional
{
	bool do_first; // true for do while
};

struct For: Scope
{
	Iterator* iterator;
};

//----------------------------------------------------------------
// Data
//----------------------------------------------------------------

struct Variable: Symbol
{
	Type* type;
	
	Ref <Expression> initializer;
};

struct Expression: Symbol
{
	enum opcode
	{
		#include "opcode.def"
	}
	opcode;
	
	union
	{
		Expression*         operand [2];
		Reference           reference;
		array <Expression*> list;
		char*               literal;
	};
	
	Type type () const;
	bool constant () const;
};

//----------------------------------------------------------------
// Definitions
//----------------------------------------------------------------

struct Function: Scope
{
	Type* result; // return type
	Type* parameters;
};

struct Operator: Function
{
	enum Expression::opcode
	opcode;
};

struct Allocator: Scope
{
	Function
	alloc, // requested size -> allocated buffer
	size, // address -> buffer size (0 for externally managed)
	free; // address -> deallocated buffer
};

struct Iterator: Symbol 
{
	enum iteration { LOOP, RANGE, FIELD }
	iteration;
	
	struct Loop;
	struct Range;
	struct Field;
};

struct Iterator::Loop: Iterator
{
	Variable    subject;
	Expression* condition;
	Symbol*     onloop;
	
	Scope*      body;
};

struct Iterator::Range: Iterator, Expression {};

struct Iterator::Field: Iterator, Ref <Variable> {};

//----------------------------------------------------------------
// Program flow
//----------------------------------------------------------------

struct Case: Symbol
{
	Expression* pattern; // null -> default case
};

struct Goto: Symbol
{
	least jump_index;
};

struct Return: Symbol
{
	Opt <Expression> value;
};

//----------------------------------------------------------------
// Type system
//----------------------------------------------------------------

struct Type
{
	enum data
	{
		NONE = -1, VOID = 0,
		NUMBER,
		BUFFER,
		FUNCTION,
		TUPLE,
		STRUCTURE,
	}
	data;
	
	struct Number;
	struct Buffer;
	struct Function;
	struct Tuple;
	struct Structure;
};

struct Type::Number: Type
{
	enum pattern
	{
		DEFAULT, SIGNED, UNSIGNED,
		REAL, IMAGINARY, COMPLEX,
	}
	pattern;
	
	enum type
	{
		BIT, BYTE, CHAR,
		INT, SHORT, LONG,
		FLOAT, DOUBLE,
	}
	type;
};

struct Type::Buffer: Type
{
	Type* base;
};

struct Type::Function: Type
{
	Type result;
	Type subject;
	Type parameters;
};

struct Type::Tuple: Type
{
	array <Type*> fields;
};

struct Type::Structure: Type
{
	Ref <Scope> body;
	
	Tuple tuple () const;
};

//----------------------------------------------------------------
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
//----------------------------------------------------------------

fast insert (Symbol*, Scope); // returns index of insert within parent

Symbol* find (Name path, Scope); // find in this scope or its nested scopes
Symbol* lookup (Name path, Scope); // find in any accessible part of the symbol table

#endif
