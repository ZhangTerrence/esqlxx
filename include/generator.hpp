#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "grouping_variable.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace esqlxx
{
    class generator
    {
    public:
        generator(std::string connection_string, std::string table);

        void read_inputs();
        void print() const;

        void generate();

    private:
        std::vector<std::string> output_structure_;
        std::vector<std::string> grouping_attributes_;
        std::vector<esqlxx::grouping_variable> grouping_variables_;
        std::string having_clause_;

        std::string s_;
        std::string connection_string_;
        std::string table_;
        std::unordered_map<std::string, std::string> attributes_;

        void get_table_info();
        void generate_headers();
        void generate_struct();
        void generate_helpers();
        void generate_main();
        std::string generate_init_loop() const;
        std::string generate_hash_fn() const;
        void write_file() const;
    };
}

#endif
