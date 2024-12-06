#ifndef GROUPING_VARIABLE_HPP
#define GROUPING_VARIABLE_HPP

#include "aggregate.hpp"
#include "predicate.hpp"

#include <string>
#include <vector>

namespace esqlxx
{
    class grouping_variable
    {
    public:
        explicit grouping_variable(int i);

        void add_aggregate(int i, std::string const& aggregate);

        void add_predicate(int i, std::string const& predicate);

        // Initializes all the grouping variables with all of their respective aggregates and predicates
        static std::vector<esqlxx::grouping_variable> get_grouping_variables(
            int n,
            std::vector<std::string> const& grouping_variable_aggregates,
            std::vector<std::string> const& grouping_variable_predicates);

        // Gets the grouping variable number.
        [[nodiscard]] int get_i() const { return this->i_; }

        // Gets all the aggregates.
        [[nodiscard]] std::vector<esqlxx::aggregate> get_aggregates() const { return this->aggregates_; }

        // Gets all the predicates.
        [[nodiscard]] esqlxx::predicate get_predicate() const { return this->predicate_; }

    private:
        int i_;
        std::vector<esqlxx::aggregate> aggregates_;
        esqlxx::predicate predicate_;
    };
}

#endif
