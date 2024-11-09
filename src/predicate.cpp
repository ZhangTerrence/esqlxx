#include "../include/predicate.hpp"
#include "../include/utility.hpp"

esqlxx::predicate::predicate(std::string const& predicate)
{
    auto const& tokens = esqlxx::utility::split(predicate, ' ');
    this->lhs_ = tokens[0];
    this->operator_ = tokens[1];
    this->rhs_ = tokens[2];
}

std::string esqlxx::predicate::string() const
{
    return this->lhs_ + " " + this->operator_ + " " + this->rhs_;
}
