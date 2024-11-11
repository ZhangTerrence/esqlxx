#include "include/generator.hpp"

int main()
{
    esqlxx::generator generator("postgresql://terrence:terrence@127.0.0.1:5432/cs562", "sales");
    generator.read_inputs();
    generator.print();
    generator.generate();
    return 0;
}
