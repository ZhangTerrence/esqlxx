#include "../include/aggregate_fn.hpp"
#include "../include/utility.hpp"

#include <ostream>

esqlxx::aggregate_fn::aggregate_fn(int const i, std::string const& aggregate)
{
    this->i_ = i;
    auto const& tokens = esqlxx::utility::split(aggregate, '_');
    this->grouping_variable_i_ = std::stoi(&tokens[0].back()) - 1;
    this->type_ = esqlxx::AggregateType::Count;
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
    else if (tokens[1].contains("max"))
    {
        this->type_ = esqlxx::AggregateType::Max;
    }
    this->attribute_ = tokens[2];
    this->string_ = aggregate;
}
