#include "../include/predicate.hpp"
#include "../include/utility.hpp"

#include <iostream>

esqlxx::predicate::predicate(int const i, std::string const& predicate)
{
    this->i_ = i;
    auto const& tokens = esqlxx::utility::split(predicate, '.');
    this->grouping_variable_i_ = std::stoi(&tokens[0].back()) - 1;
    auto const& tokens_ = esqlxx::utility::split(tokens[1], ' ');
    this->lhs_ = tokens_[0];
    this->operator_ = tokens_[1];
    this->rhs_ = tokens_[2];
}

std::string esqlxx::predicate::get_string() const
{
    return this->lhs_ + " " + this->operator_ + " " + this->rhs_;
}
