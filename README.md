# VMTSwap
A stealthy and minimalist c++ library for shadow VMT (Virtual Method Table) hooking in x86 and x64 architectures.

## Features
- Support for both x86/x64
- Single file (header) library
- Stealthy

## What is the VMT?
The **Virtual Method Table (VMT)** is a mechanism used in the C++ compiler to handle [dynamic dispatch](https://en.wikipedia.org/wiki/Dynamic_dispatch), or as we know it, runtime polymorphism. Think of a class containing virtual functions; how do we tell *which* implementation to invoke? Its base method, or an override? 

The **MSVC** compiler solves this by inserting a hidden member variable known as the **Virtual Pointer (VPTR)**. This ptr points to a table of function pointers (our VMT), which is then used to dynamially resolve and invoke the correct function implementation at runtime. 

## What does VMTSwap do?
VMTSwap takes advantage of this behavior to intercept or modify vfunction calls. 

Unlike traditional VMT hooking, this method doesn't modify the original VMT; instead, it inserts a shadow (or copy) vtable in its place then swaps the instance's VPTR.

## Example usage
See [example_x64.cpp](examples/example_x64.cpp)