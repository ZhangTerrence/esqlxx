#include "../include/generator.hpp"
#include "../include/utility.hpp"

#include <iostream>

void esqlxx::generator::read_inputs()
{
    std::string inputs[6];

    for (auto& i : inputs)
    {
        std::string input;
        std::getline(std::cin, input);
        i = input;
    }

    std::vector<std::string> const output_structure = esqlxx::utility::split(inputs[0], ',');
    int const n = std::stoi(inputs[1]);
    std::vector<std::string> const grouping_attributes = esqlxx::utility::split(inputs[2], ',');
    std::vector<std::string> const grouping_variable_aggregates = esqlxx::utility::split(inputs[3], ',');
    std::vector<std::string> const grouping_variable_predicates = esqlxx::utility::split(inputs[4], ',');
    std::string const having_clause = inputs[5];

    this->output_structure_ = output_structure;
    this->grouping_attributes_ = grouping_attributes;
    this->grouping_variables_ = esqlxx::grouping_variable::get_grouping_variables(
        n, grouping_variable_aggregates, grouping_variable_predicates);
    this->having_clause_ = esqlxx::utility::to_cpp(having_clause);


}

void esqlxx::generator::print()
{
    std::cout << "Output structure: ";
    for (const auto& e : this->output_structure_)
    {
        std::cout << e << " ";
    }
    std::cout << "\n";
    std::cout << "Grouping attributes: ";
    for (const auto& e : this->grouping_attributes_)
    {
        std::cout << e << " ";
    }
    std::cout << "\n";
    std::cout << "Grouping variables: \n";
    for (const auto& e : this->grouping_variables_)
    {
        std::cout << e.get_i() << " ";
        std::cout << "Aggregate Functions: ";
        for (auto const& fn : e.get_aggregate_fns())
        {
            std::cout << fn << " ";
        }
        std::cout << "Predicates: ";
        for (auto const& predicate : e.get_predicates())
        {
            std::cout << predicate.string() << " ";
        }
        std::cout << "\n";
    }
    std::cout << "Having clause: " << this->having_clause_;
}
