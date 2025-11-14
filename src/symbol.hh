#ifndef SYMBOL_DOT_H
#define SYMBOL_DOT_H

#include <bit>
#include <sstream>
#include <unordered_map>
#include <vector>

//#include "token.hh"
#include "log.hh"

#include "optional.hh"
#include "ints.hh"

template <class T>
using Array = std::vector <T>;

template <class T>
using Map = std::unordered_map <T>;

struct Scope;

struct Tag
{
	Location location;
	char* comment;
};

struct Label: Tag
{
	Array <char*> names;
	bool global; // whether to start from the root of the symbol tree
	
	inline
	char* name () const {	return names.last(); } 
	
	Label () = default;
	Label (Location);
	Label (Location, char* first, bool global);
};

/*-------------------------------------
 | Refers to a symbol
 | by direct pointer or unresolved path
 |_____________________________________*/
struct Reference: Tag
{
	Label path;
	struct Symbol* ptr;
	
	Reference () = default;
	
	Reference (Label Path)
	{ path = Path; }
	
	Reference (struct Symbol* symbol)
	{ ptr = symbol; }
};

struct Template: Array<Reference>, Tag
{
	Template () = default;
};

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
		MACRO,    // literal token-level substitution
		
		// Stack / Locals //
		// -------------- //
		VARIABLE,   // storage class value (mutable, addressable)
		EXPRESSION, // temporary class value (immutable, non-addressable)
		
		// Scopes //
		// ------ //
		SCOPE,   // general container for symbols, and namespace
		            // (including structs, unions, enums, & stack frames)
		CONTROL, // scope attached to a conditional or loop
		
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
	
	Reference parent; // where symbol is defined
	Reference target; // desired path to symbol
};


struct Scope: Symbol
{
	// compile parameters attached to a scope
	Template generics;
	
	Array <Symbol*>
	members;
	
	Array <short>
	fields; // index into members
	
	Array <short>
	expressions;
	
	std::unordered_multimap <std::string, fast>*
	nametable; // index into members by name
	
	// for a given scope, there can only be one definition per operator/opcode
	std::unordered_map <fast, Operator*>*
	operators;
	
	Iterator*
	iterator;
	
	Array <Log> logs;
	
	fast insert (Symbol*); // returns index of insert within parent
	
	Symbol* find (Label path); // find in this scope or its nested scopes
	Symbol* lookup (Label path); // find in any accessible part of the symbol table
	
	Scope () = default;
	Scope (Location, Scope);
	Scope (Location);
};


struct Type: Tag
{
	enum DataType
	{
		//- Special types -//
		NONE	= -1,
		VOID = 0,
		NUMERIC,
		POINTER,
		ARRAY,
		FUNCTION,
		USER /* tuple, struct, union, or enum */,
		META,
		UNRESOLVED, // type defined elsewhere
	};
	
	enum Representation
	{ DEFAULT, SIGNED, UNSIGNED, REAL, IMAGINARY, COMPLEX };
	
	fast data;
	
	struct Qualifiers
	{
		ubyte
		locality: 1,
		staticness: 1,
		constness: 1,
		representation: 3;
	}
	qualifiers;
	
	struct Numeric;
	struct Pointer;
	struct Array;
	struct Function;
	struct Module;
	struct Meta;
};

struct Type::Numeric: Type
{
	fast longness;
	
	enum Scalar
	{ BIT, CHAR, BYTE, SHORT, INT, LONG,
		FLOAT, DOUBLE, };
	ubyte scalar: 3;
	
	Numeric () = default;
	Numeric (Location, fast scalar, fast longness = 0);
};

struct Type::Pointer: Type
{
	Type* underlying_type;
	
	struct Indirection
	{
		byte num_pointers;
	
		struct Pointer
		{ 
			char constness: 1, c_style: 1;
		}
		pointers [7];
	}
	indirection;
	
	Pointer (Location, Type*, ::Array <Indirection::Pointer>);
};

struct Type::Array: Type
{
	Type* contents;
	struct Expression* count;
};

struct Type::Function: Type
{
	Type* returned;
	Type* parameters;
	Scope* body;
};

struct Type::Module: Type
{
	Reference definition;
	
	Module (Location, Reference);
};

struct Type::Meta: Type
{
	Reference symbol;
	
	Meta (Location, Reference);
};

struct Expression: Symbol
{
	enum Opcode
	{
		#include "opcode.def"
	};
	
	Type* type;
	fast opcode;
	bool constant = false;
	
	struct Meta;
	struct Pair;
	struct Call;
	struct Ref;
	struct Literal;
	struct Unary;
	struct Binary;
	struct List;
};

struct Instance
{
	Array <Expression*> dimensionality;
	opt <Expression*> initializer;
	bool variadic;
};

struct Variable: Symbol, Instance
{
	Type* type;
	
	Variable () = default;
	Variable (Location, Reference name, Array <Expression*> dimensionality, Expression* initializer = 0);
	Variable (Location, Reference name, Expression* initializer = 0);
	Variable (Location, Variable);
};

struct Iterator;
struct Operator;

struct Iterator: Symbol
{
	struct Loop;
	struct Range;
	struct Custom;
};

struct Iterator::Loop: Iterator
{
	Symbol* init;
	Expression* condition;
	Symbol* on_continue;
	
	Loop (Location, Symbol* init, Expression* condition, Symbol* on_continue);
};

struct Iterator::Range: Iterator
{
	Expression* range;
	char* instance_name;
	
	Range (Location, char* instance_name, Expression* range);
	Range (Location, Expression* range);
};

struct Iterator::Custom: Iterator
{
	Reference container;
	char* instance_name;
	
	Custom (Location, char* instance_name, Reference);
	Custom (Location, Reference);
};

struct Module: Scope
{
	 enum Form { STRUCT, MODULE, UNION };
	 
	 Module (Location, fast form, Reference name, Scope);
	 Module (Location, Scope);
	 Module (Location, Module);
};

struct Enum: Scope
{
	Enum (Location);
	Enum (Location, Scope);
	Enum (Array <Variable>);
	Enum (Location, Reference name, Scope);
};

struct Conditional: Scope
{
	Expression* condition;
	opt <Scope> otherwise;
	
	Conditional (Location, fast, Expression*, Scope, opt <Scope>);
	Conditional (Location, fast, Expression*, Scope);
};

struct For: Scope
{
	Iterator* iterator;
	
	For (Location, Iterator*, Scope);
};

struct Function: Symbol
{
	Type* return_type;
	Scope parameters;
	opt <Scope*> body;
	
	Function () = default;
	Function (Location, Function);
	Function (Location, Type*, Reference name, Scope parameters);
	Function (Location, Type*, Reference name, Scope parameters, Scope body);
};

struct Operator: Function
{
	using Opcode = Expression::Opcode;
	
	Operator (Location, fast opcode, Scope parameters, Scope body);
};

struct Include: Symbol, Reference
{
	Include (Location, Reference);
};

struct Alias: Symbol, Reference
{
	Alias (Location, Reference name, Reference);
};

struct Marker: Symbol
{
	fast index;
	
	Marker (Location);
	Marker (Location, char* name);
};

struct Case: Symbol
{
	fast index;
	Expression* pattern;
	
	Case (Location, Expression*);
	Case (Location);
};

struct Jump: Symbol
{
	enum Destination { CONTINUE = Symbol::CONTINUE, BREAK = Symbol::BREAK, GOTO = Symbol::GOTO	}
	destination;
	
	Reference marker;
	
	Jump (Location, fast);
	Jump (Location, Reference);
};

struct Return: Symbol
{
	opt <Expression*> value;
	
	Return (Location, Expression*);
	Return (Location);
};

struct Expression::Meta: Expression
{
	enum Kind
	{ SIZEOF, COUNTOF, NAMEOF, STRINGOF }
	kind;
	
	Expression* expression;
	
	Meta (Location, fast kind, Expression*);
};

struct Expression::Pair: Expression
{
	Pair (Location, Expression*, Expression*);
};

struct Expression::Call: Expression
{
	Reference function;
	opt <Expression*> arguments;
	
	Call (Location, Reference, Expression*);
};

struct Expression::Ref: Expression, Reference
{
	Ref (Location, Reference);
};

struct Expression::Literal: Expression
{
	char* constant;
	
	Literal (Location, char*);
};

struct Expression::Unary: Expression
{
	Expression* operand;
	
	Unary (Location, fast opcode, Expression*);
};

struct Expression::Binary: Expression
{
	union
	{
		Expression* operands [2];
		struct {Expression *left, *right;};
	};
	
	Binary (Location, Expression*, fast opcode, Expression*);
};

struct Expression::List: Expression
{
	Array <Expression*> expressions;
	
	List (Location);
};

#endif
