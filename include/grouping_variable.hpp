#ifndef GROUPING_VARIABLE_HPP
#define GROUPING_VARIABLE_HPP

#include "aggregate_fn.hpp"
#include "predicate.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace esqlxx
{
    class grouping_variable
    {
    public:
        explicit grouping_variable(int i);

        void add_aggregate_fn(int i, std::string const& aggregate_fn);

        void add_predicate(int i, std::string const& predicate, std::unordered_map<std::string, bool> const& grouping_attributes);

        static std::vector<esqlxx::grouping_variable> get_grouping_variables(
            int n,
            std::vector<std::string> const& grouping_variable_aggregates,
            std::vector<std::string> const& grouping_variable_predicates,
            std::vector<std::string> const& grouping_attributes);

        [[nodiscard]] int get_i() const { return this->i_; }

        [[nodiscard]] std::vector<esqlxx::aggregate_fn> get_aggregate_fns() const { return this->aggregate_fns_; }

        [[nodiscard]] std::vector<esqlxx::predicate> get_predicates() const { return this->predicates_; }

    private:
        int i_;
        std::vector<esqlxx::aggregate_fn> aggregate_fns_;
        std::vector<esqlxx::predicate> predicates_;
    };
}

#endif
