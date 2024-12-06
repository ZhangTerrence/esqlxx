#include "../include/aggregate.hpp"
#include "../include/utility.hpp"

#include <ostream>

esqlxx::aggregate::aggregate(int const i, std::string const& aggregate)
{
    this->string_ = aggregate;
    this->i_ = i;
    auto const& tokens = esqlxx::utility::split(aggregate, '_');
    this->ith_grouping_variable_ = std::stoi(&tokens[0].back()) - 1;
    if (tokens[1].contains("count"))
    {
        this->type_ = esqlxx::AggregateType::Count;
    }
    else if (tokens[1].contains("sum"))
    {
        this->type_ = esqlxx::AggregateType::Sum;
    }
    else if (tokens[1].contains("avg"))
    {
        this->type_ = esqlxx::AggregateType::Average;
    }
    else if (tokens[1].contains("min"))
    {
        this->type_ = esqlxx::AggregateType::Min;
    }
    else
    {
        this->type_ = esqlxx::AggregateType::Max;
    }
    this->attribute_ = tokens[2];
}
