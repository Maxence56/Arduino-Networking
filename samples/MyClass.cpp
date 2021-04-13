#include "MyClass.h"
#include <iostream>

void MyClass::set_k(int a_k){
    k = a_k;
}

void MyClass::print_k(){
    std::cout << "The value of k is: " << k << std::endl;
}