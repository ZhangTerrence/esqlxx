#ifndef AGGREGATE_FN_HPP
#define AGGREGATE_FN_HPP

#include <string>

namespace esqlxx
{
    enum AggregateType
    {
        Count,
        Sum,
        Average,
        Min,
        Max
    };

    class aggregate
    {
    public:
        aggregate(int i, std::string const& aggregate);

        // Gets the raw string of the aggregate.
        [[nodiscard]] std::string get_string() const { return this->string_; }

        // Gets the relative position of the aggregate's declaration with respect to all the other aggregates. Used for keeping track of the aggregate's n.
        [[nodiscard]] int get_i() const { return this->i_; }

        // Gets the grouping variable that this aggregate belongs to.
        [[nodiscard]] int get_ith_grouping_variable() const { return this->ith_grouping_variable_; }

        // Gets the aggregate function type.
        [[nodiscard]] esqlxx::AggregateType get_type() const { return this->type_; }

        // Gets the attribute targeted by the aggregate.
        [[nodiscard]] std::string get_attribute() const { return this->attribute_; }

    private:
        std::string string_;
        int i_;
        int ith_grouping_variable_;
        esqlxx::AggregateType type_;
        std::string attribute_;
    };
}

#endif
