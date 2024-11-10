#include "../include/grouping_variable.hpp"
#include "../include/utility.hpp"

#include <iostream>
#include <unordered_map>

esqlxx::grouping_variable::grouping_variable(int const i)
    : i_(i), aggregate_fns_({}), predicates_{{}}
{
}

void esqlxx::grouping_variable::add_aggregate_fn(std::string const& aggregate_fn)
{
    this->aggregate_fns_.push_back(aggregate_fn);
}

void esqlxx::grouping_variable::add_predicate(std::string const& predicate)
{
    this->predicates_.emplace_back(predicate);
}

std::vector<esqlxx::grouping_variable> esqlxx::grouping_variable::get_grouping_variables(
    int const n,
    std::vector<std::string> const& grouping_variable_aggregates,
    std::vector<std::string> const& grouping_variable_predicates)
{
    std::unordered_map<int, esqlxx::grouping_variable> unique_grouping_variable;
    std::vector<esqlxx::grouping_variable> grouping_variables;

    for (const auto& aggregate : grouping_variable_aggregates)
    {
        auto const& tokens = esqlxx::utility::split(aggregate, '_');
        auto const i = std::stoi(&tokens[0].back());

        if (!unique_grouping_variable.contains(i))
        {
            unique_grouping_variable.insert({i, esqlxx::grouping_variable{i}});
        }
        unique_grouping_variable.at(i).add_aggregate_fn(aggregate);
    }

    for (const auto& predicate : grouping_variable_predicates)
    {
        auto const& tokens = esqlxx::utility::split(predicate, '.');
        auto const i = std::stoi(&tokens[0].back());

        if (!unique_grouping_variable.contains(i))
        {
            unique_grouping_variable.insert({i, esqlxx::grouping_variable{i}});
        }
        unique_grouping_variable.at(i).add_predicate(esqlxx::utility::to_cpp(tokens[1]));
    }

    for (auto const& [_, grouping_variable] : unique_grouping_variable)
    {
        grouping_variables.push_back(grouping_variable);
    }

    return grouping_variables;
}
