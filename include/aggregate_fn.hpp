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

    class aggregate_fn
    {
    public:
        aggregate_fn(int i, std::string const& aggregate);

        [[nodiscard]] std::string get_string() const { return this->string_; }

        [[nodiscard]] int get_i() const { return this->i_; }

        [[nodiscard]] int get_grouping_variable_i() const { return this->grouping_variable_i_; }

        [[nodiscard]] esqlxx::AggregateType get_type() const { return this->type_; }

        [[nodiscard]] std::string get_attribute() const { return this->attribute_; }

    private:
        std::string string_;
        int i_;
        int grouping_variable_i_;
        esqlxx::AggregateType type_;
        std::string attribute_;
    };
}

#endif
