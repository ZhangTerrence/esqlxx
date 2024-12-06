#include "../include/predicate.hpp"
#include "../include/utility.hpp"

#include <iostream>

esqlxx::predicate::predicate(int const i, std::string const& predicate)
{
    this->string_ = predicate;
    this->ith_grouping_variable_ = i;
}
