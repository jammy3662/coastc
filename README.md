## Coastc (Coast Compiler)
Official compiler for the Coast programming language.

### What is Coast?
Coast (co-AST) is a compiled, general-purpose programming language with a modern design, clean development environment, and high-level compile-time features with zero runtime overhead. Coast uses compile-time reference counting to model object lifetimes, making it *completely free of* the most common and notorious memory bugs. Coast has **no dangling pointers** and **no memory leaks**, all guaranteed by the compiler itself. When you use memory in Coast, *it just works*.

Coast uses an agnostic intermediate representation that replaces the functionality of separating header and implementation files, hence the name: co-AST. Code in Coast is truly modular and composable.

Detailed information about Coast can be found on the [wiki page](https://github.com/jammy3662/coastc/wiki).