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

        // Reads phi operator inputs and generates variables
        void read_inputs();

        // Prints read data for debugging purposes.
        void print() const;

        // Generates the C++ file.
        void generate(bool count_rows);

    private:
        int num_grouping_aggregates;
        std::vector<std::string> output_structure_;
        std::vector<std::string> grouping_attributes_;
        std::vector<esqlxx::grouping_variable> grouping_variables_;
        std::string having_clause_;

        std::string s_;
        std::string connection_string_;
        std::string table_;
        std::unordered_map<std::string, std::tuple<int, std::string>> attributes_;

        void get_table_info();
        void generate_headers();
        void generate_struct();
        void generate_helpers(bool count_rows);
        void generate_main();
        std::string generate_init_loop() const;
        std::string generate_scan_loops() const;
        std::string generate_hash_fn(bool init) const;
        static int generate_aggregate_init(esqlxx::aggregate const& aggregate);
        static std::string generate_aggregate_inc(esqlxx::aggregate const& aggregate);
        void write_file() const;
    };
}

#endif
