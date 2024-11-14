#include "../include/grouping_variable.hpp"
#include "../include/utility.hpp"

#include <bits/ranges_algo.h>
#include <iostream>
#include <unordered_map>

esqlxx::grouping_variable::grouping_variable(int const i)
    : i_(i), aggregate_fns_({}), predicates_{{}}
{
}

void esqlxx::grouping_variable::add_aggregate_fn(int const i, std::string const& aggregate_fn)
{
    this->aggregate_fns_.emplace_back(i, aggregate_fn);
}

void esqlxx::grouping_variable::add_predicate(int const i, std::string const& predicate, std::unordered_map<std::string, bool> const& grouping_attributes)
{
    this->predicates_.emplace_back(i, predicate, grouping_attributes);
}

std::vector<esqlxx::grouping_variable> esqlxx::grouping_variable::get_grouping_variables(
    int const n,
    std::vector<std::string> const& grouping_variable_aggregates,
    std::vector<std::string> const& grouping_variable_predicates,
    std::vector<std::string> const& grouping_attributes)
{
    std::unordered_map<int, esqlxx::grouping_variable> unique_grouping_variable;
    std::vector<esqlxx::grouping_variable> grouping_variables;

    std::unordered_map<std::string, bool> gas;

    for (auto const& grouping_attribute : grouping_attributes)
    {
        gas[grouping_attribute] = true;
    }

    for (int i = 0; i < grouping_variable_aggregates.size(); i++)
    {
        auto const& aggregate = grouping_variable_aggregates[i];
        auto const& tokens = esqlxx::utility::split(aggregate, '_');
        auto const grouping_variable_i = std::stoi(&tokens[0].back()) - 1;

        if (!unique_grouping_variable.contains(grouping_variable_i))
        {
            unique_grouping_variable.insert({grouping_variable_i, esqlxx::grouping_variable{grouping_variable_i}});
        }
        unique_grouping_variable.at(grouping_variable_i).add_aggregate_fn(i, aggregate);
    }

    for (int i = 0; i < grouping_variable_predicates.size(); i++)
    {
        auto const& predicate = grouping_variable_predicates[i];
        auto const& tokens = esqlxx::utility::split(predicate, '.');
        auto const grouping_variable_i = std::stoi(&tokens[0].back()) - 1;

        if (!unique_grouping_variable.contains(i))
        {
            unique_grouping_variable.insert({grouping_variable_i, esqlxx::grouping_variable{i}});
        }
        unique_grouping_variable.at(grouping_variable_i).add_predicate(i, esqlxx::utility::to_cpp(predicate), gas);
    }

    for (auto const& [_, grouping_variable] : unique_grouping_variable)
    {
        grouping_variables.push_back(grouping_variable);
    }

    std::ranges::sort(grouping_variables, [](auto const& a, auto const& b)
    {
        return a.get_i() < b.get_i();
    });

    return grouping_variables;
}
