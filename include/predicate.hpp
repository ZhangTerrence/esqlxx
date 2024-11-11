#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <string>

namespace esqlxx
{
    class predicate
    {
    public:
        predicate(int i, std::string const& predicate);

        [[nodiscard]] std::string get_string() const;

        [[nodiscard]] int get_i() const { return this->i_; }

        [[nodiscard]] int get_grouping_variable_i() const { return this->grouping_variable_i_; }

        [[nodiscard]] std::string get_lhs() const { return this->lhs_; }

        [[nodiscard]] std::string get_operator() const { return this->operator_; }

        [[nodiscard]] std::string get_rhs() const { return this->rhs_; }

    private:
        int i_;
        int grouping_variable_i_;
        std::string lhs_;
        std::string operator_;
        std::string rhs_;
    };
}

#endif
