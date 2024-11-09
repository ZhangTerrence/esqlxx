#include <iostream>

#include "include/generator.hpp"

int main()
{
    esqlxx::generator generator;
    generator.read_inputs();
    generator.print();
    return 0;
}
