#include <iostream>
#include "../vmtswap.hpp"

std::unique_ptr<VMTSwap> g_vmtSwap;

class example {
public:
	// RTTI example[0000] -- vtable idx 0
	virtual void foo(int x) {
		printf("[example] foo(x = %d)\n", x);
	}
};

using fooFn = void(__stdcall*)(int);
void __stdcall foo_hook(int x)
{
	printf("{HOOKED} foo(x = %d)\n", x);

	fooFn fooOrig = g_vmtSwap->get_orig_func<fooFn>(0);
	fooOrig(x);
}

int main(int argc, char** argv)
{
	// spawn new instance of target obj
	example* exampleInst = new example();

	// hook the virtual function at index 0 (foo)
	g_vmtSwap = std::make_unique<VMTSwap>(exampleInst);
	g_vmtSwap->hook(0, &foo_hook);

	// function call
	exampleInst->foo(1337);

	// restore original foo
	g_vmtSwap->unhook(0);
}