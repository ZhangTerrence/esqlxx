#include "../include/grouping_variable.hpp"
#include "../include/utility.hpp"

#include <bits/ranges_algo.h>
#include <iostream>
#include <unordered_map>

esqlxx::grouping_variable::grouping_variable(int const i)
    : i_(i), aggregates_({}), predicate_(esqlxx::predicate(-1, ""))
{
}

void esqlxx::grouping_variable::add_aggregate(int const i, std::string const& aggregate)
{
    this->aggregates_.emplace_back(i, aggregate);
}

void esqlxx::grouping_variable::add_predicate(int const i, std::string const& predicate)
{
    this->predicate_ = esqlxx::predicate(i, predicate);
}

std::vector<esqlxx::grouping_variable> esqlxx::grouping_variable::get_grouping_variables(
    int const n,
    std::vector<std::string> const& grouping_variable_aggregates,
    std::vector<std::string> const& grouping_variable_predicates)
{
    std::unordered_map<int, esqlxx::grouping_variable> unique_grouping_variables;
    std::vector<esqlxx::grouping_variable> grouping_variables;

    // Adds grouping variable aggregates to their respective grouping variables
    for (int i = 0; i < grouping_variable_aggregates.size(); i++)
    {
        auto const& aggregate = grouping_variable_aggregates[i];
        auto const& tokens = esqlxx::utility::split(aggregate, '_');
        auto const ith_grouping_variable = std::stoi(&tokens[0].back()) - 1;

        if (!unique_grouping_variables.contains(ith_grouping_variable))
        {
            unique_grouping_variables.insert({ith_grouping_variable, esqlxx::grouping_variable{ith_grouping_variable}});
        }
        unique_grouping_variables.at(ith_grouping_variable).add_aggregate(i, aggregate);
    }

    // Adds grouping variable predicates to their respective grouping variables
    // Unlike grouping variable aggregates, there can only be one grouping variable predicate per grouping variable
    for (int ith_grouping_variable = 0; ith_grouping_variable < n; ith_grouping_variable++)
    {
        auto const& predicate = grouping_variable_predicates[ith_grouping_variable];

        if (!unique_grouping_variables.contains(ith_grouping_variable))
        {
            unique_grouping_variables.insert({ith_grouping_variable, esqlxx::grouping_variable{ith_grouping_variable}});
        }
        unique_grouping_variables.at(ith_grouping_variable).add_predicate(ith_grouping_variable, predicate);
    }

    // Converts to vector.
    for (auto const& [i, grouping_variable] : unique_grouping_variables)
    {
        grouping_variables.push_back(grouping_variable);
    }

    std::ranges::sort(grouping_variables, [](auto const& a, auto const& b)
    {
        return a.get_i() < b.get_i();
    });

    return grouping_variables;
}
