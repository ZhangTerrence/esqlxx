#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <string>

namespace esqlxx
{
    class predicate
    {
    public:
        predicate(int i, std::string const& predicate);

        // Gets the raw string of the predicate.
        [[nodiscard]] std::string get_string() const { return this->string_; }

        // Gets the grouping variable that this aggregate belongs to.
        [[nodiscard]] int get_ith_grouping_variable() const { return this->ith_grouping_variable_; }

    private:
        std::string string_;
        int ith_grouping_variable_;
    };
}

#endif
