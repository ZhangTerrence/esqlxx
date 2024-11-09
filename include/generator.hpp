#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "grouping_variable.hpp"

#include <string>
#include <vector>

namespace esqlxx
{
    class generator
    {
    public:
        generator() = default;

        void read_inputs();
        void print();

    private:
        std::vector<std::string> output_structure_;
        std::vector<std::string> grouping_attributes_;
        std::vector<esqlxx::grouping_variable> grouping_variables_;
        std::string having_clause_;
    };
}

#endif
