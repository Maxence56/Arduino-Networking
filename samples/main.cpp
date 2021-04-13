#include <iostream>
#include "MyClass.h"
#include "ard_unique_ptr.h"
#include "ard_utils.h"


int main(){
    std::cout<< "Hello world!" << std::endl;
    int i = 23;
    int* p;  // p is a pointer to integer
    p = &i;  // now p points to i
    std::cout << "i=" << i << std::endl;
    i = 25;
    std::cout << "*P=" << *p << std::endl;
    
    MyClass mc;
    mc.set_k(3);
    mc.print_k();

    MyClass* mc_p = &mc;
    mc_p -> set_k(5);
    mc.print_k();

    ard_unique_ptr<MyClass> uptr_1(new MyClass);
    uptr_1 -> set_k(21);
    uptr_1 -> print_k();
    ard_unique_ptr<MyClass> uptr_2;
    uptr_2 = ard_move(uptr_1);
    // At this point, uptr_1 is not valid anymore and cannot be used (it has
    // "passed the token to uptr_2")

    // The following line does not compile
    // uptr_2 = uptr_1;
    uptr_2 -> print_k();
    // The following line causes a segmentation fault (memory access error)
    // uptr_1 -> set_k(33);

    return 0;
}