// Howdy!
//
// Thank you for your interest in FrogSlayer and taking the time to apply.
//
// This file is a coding exercise. Please follow the instructions explicitly -
// you may only write code where specified. We've included reference material
// with information to produce a working solution. This is not a trick problem;
// it is solvable within the given constraints.
//
// It's okay if you can't get a fully working solution; just submit what you
// have. We're more interested in seeing how well you can learn and use
// something new with limited direction.
//
// Instructions:
//
// Only writing code where indicated (the bodies of the `extract_x`,
// `extract_y`, `call_foo`, and `call_bar` functions), make this program output
// the values of:
//
//   * `thing->x`
//   * `thing->y`
//   * `thing->foo()`
//   * `thing->bar()`
//
// Some useful references:
//
// https://en.wikipedia.org/wiki/Virtual_method_table
// http://www.openrce.org/articles/full_view/23
// https://en.wikipedia.org/wiki/Pointer_(computer_programming)#C_arrays
// https://en.wikipedia.org/wiki/Pointer_(computer_programming)#Function_pointer


#include <cstdio>

class Thing
{
private:
	int x;
	int y;
	virtual int foo()
	{
		return x+y;
	}
	virtual int bar()
	{
		return x*y;
	}
public:
	Thing(){
		x = 2;
		y = 10;
	}
};

//MEM LAYOUT
// 0:   vtable
// 4:   int x
// 8:   int y
// 12:  char test //removed test data member

//MEM LAYOUT for Thing in G++ compiler in MinGW GCC version 6.3.0 (output from cmd 'g++ -fdump-class-hierarchy layout.cpp):
// Class _iobuf
//    size=32 align=4
//    base size=32 base align=4
// _iobuf (0x0x84d4460) 0

// Vtable for Thing
// Thing::_ZTV5Thing: 4u entries
// 0     (int (*)(...))0                
// 4     (int (*)(...))(& _ZTI5Thing)  
// 8     (int (*)(...))Thing::foo
// 12    (int (*)(...))Thing::bar

// Class Thing
//    size=12 align=4
//    base size=12 base align=4
// Thing (0x0x84d4738) 0
//     vptr=((& Thing::_ZTV5Thing) + 8u)

int extract_x(void *thing)
{
    // --- Begin your code ---
    // generic/void pointer is passed in
    // here, thing param is a pointer to the mem address of thing that was declared in main() 
    // not a typed pointer, but we know we want to access an int so we'll cast it
    // doing this because from our reference material about C arrays we know we can access the
    // vtable/mem layout using pointers
    int* ptr = (int*)thing;  //cast
    //access memory via pointer
    //move over to access fist value
    return  *(ptr + 1); //wrapping in *() dereferences to access the val from that memory location
    // --- End your code   ---
}

int extract_y(void *thing)
{
    // --- Begin your code ---
    // can  follow what I did in  extract_x and adjust the mem offset to get to y's val
    // the number added to the pointer is offsetting the pointer by the size of n ints (n*4 bytes)
    int* ptr = (int*)thing;
    return *(ptr + 2); 
    // --- End your code   ---
}

// char extract_test(void *thing)
// {
//     //doing this to see if pointer must be cast to an int, or if it has to match
//     //the value that we want to pull out of memory
//     //char* ptr = (char*)thing;
//     // NOPE! pointer needs to be cast as an int pointer to correctly be adjusted and
//     // point to the correct location in memory
//     int* ptr = (int*)thing; // this works to pull a char I put after int y in class
//     return *(ptr + 3);
// }

int call_foo(void *thing)
{
    // --- Begin your code ---
    // in reference to Visual C++ compiler: "To call a virtual method the compiler first needs to fetch the function address from the _vftable_ 
    // and then call the function at that address same way as a simple method (i.e. passing _this_ pointer as an implicit parameter)". Should be the same for GCC
    
    // making the pointer of type Thing and using thingPtr->foo causes "error: cannot convert 'Thing::foo' from type 'int (Thing::)()' to type 'int (*)()'"
    // how to get it to int (*)()?
    
    //typedef int (*fp)();  
    //Thing* thingPtr = (Thing*)thing;
    //fp funcPtr = (fp)thing;     // create Thing pointer so we can get Thing vtable 
    //int (*fp)();          // define function pointer that takes no params (mirrors foo)
    //fp = thingPtr + 3              // assign Thing pointer's foo to the function pointer
    //return (*fp)();            // execute function pointer

    // the general direction of the above code ended with me getting to the function, but not by pulling from mem because I was still getting 
    // encapsulation errors

    //pretty sure the Calling Conventions and Class Methods -> virtual method section of OpenRCE article is key
    //need to figure out how to get vftable pointer
    // ;pC->A_virt2()
    // ;esi = pC
    // mov eax, [esi]  //fetch virtual table pointer
    // mov ecx, esi
    // call [eax+4]  //call second virtual method

    //couldn't really interpret the code mentioned above (maybe is specific to Visual C++?), but did follow what it directed
    //the vftable address is stored first in the mem layout of the class, so we can make a pointer to that adresss and then dereference to 
    //get at the pointer to the vftable itself
    // so we're pointing to a pointer (VTP - virtual table pointer) mentioned in https://en.wikipedia.org/wiki/Virtual_method_table

    int* ptr = (int*)thing;         //make pointer an int so we can adjust it
    //ptr is now value of first address in thing's mem layout (address of vftable)
    ptr=(int *)*ptr;                //this casts and assigns the VTP to my pointer
    int (*foo)();                   //create function pointer that matches Thing::foo()'s signature
    foo = (int (*)())*ptr;          //cast and assign value at pointer location to function pointer
    return (*foo)();                //call first method in vftable

    // --- End your code   ---
}

int call_bar(void* thing)
{
    // --- Begin your code ---
    // 'void* thing' should behave the same as 'void *thing' or even 'void * thing'
    // let's repeat what I did for call_foo() and adjust the location of the pointer
    // to call bar()
    int* ptr = (int*)thing;         //make pointer
    ptr=(int *)*ptr;                //this gets the vftable at address of first method
    ptr += 1;                       // adjust location of pointer by 4 bytes to get address of second method
    int (*bar)();
    bar = (int(*)())*ptr;
    return (*bar)();                // call second method in vftable

    // --- End your code   ---
}

int main()
{
	Thing thing;
    std::printf("%d %d %d %d\n",
                extract_x(&thing),
                extract_y(&thing),
                call_foo(&thing),
                call_bar(&thing));

    //OUTPUT: '2 10 12 20'

    // std::printf("%d %d %c %d %d\n",
    //             extract_x(&thing),
    //             extract_y(&thing),
    //             extract_test(&thing),
    //             call_foo(&thing),
    //             call_bar(&thing));
    //OUTPUT w/extract_test: '2 10 B 12 20'
	return 0;
}
