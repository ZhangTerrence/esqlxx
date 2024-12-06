#include "../include/generator.hpp"
#include "../include/utility.hpp"

#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <utility>

esqlxx::generator::generator(std::string connection_string, std::string table)
    : num_grouping_aggregates(0), connection_string_(std::move(connection_string)), table_(std::move(table))
{
}

void esqlxx::generator::read_inputs()
{
    std::string inputs[6];

    for (auto& i : inputs)
    {
        std::string input;
        std::getline(std::cin, input);
        i = input;
    }

    std::vector<std::string> const output_structure = esqlxx::utility::split(inputs[0], ',');
    int const n = std::stoi(inputs[1]);
    std::vector<std::string> const grouping_attributes = esqlxx::utility::split(inputs[2], ',');
    std::vector<std::string> const grouping_variable_aggregates = esqlxx::utility::split(inputs[3], ',');
    std::vector<std::string> const grouping_variable_predicates = esqlxx::utility::split(inputs[4], ',');
    std::string const having_clause = inputs[5];

    this->num_grouping_aggregates = grouping_variable_aggregates.size();
    this->output_structure_ = output_structure;
    this->grouping_attributes_ = grouping_attributes;
    this->grouping_variables_ = esqlxx::grouping_variable::get_grouping_variables(n, grouping_variable_aggregates, grouping_variable_predicates);
    this->having_clause_ = having_clause;
}

void esqlxx::generator::print() const
{
    std::cout << "Output structure: ";
    for (const auto& e : this->output_structure_)
    {
        std::cout << e << " ";
    }
    std::cout << "\n";
    std::cout << "Grouping attributes: ";
    for (const auto& grouping_attribute : this->grouping_attributes_)
    {
        std::cout << grouping_attribute << " ";
    }
    std::cout << "\n";
    std::cout << "Grouping variables: \n";
    for (const auto& grouping_variable : this->grouping_variables_)
    {
        std::cout << grouping_variable.get_i() << " ";
        std::cout << "Aggregate Functions: ";
        for (auto const& aggregate_fn : grouping_variable.get_aggregates())
        {
            std::cout << aggregate_fn.get_string() << " ";
        }
        std::cout << "Predicate: ";
        std::cout << grouping_variable.get_predicate().get_string() << " ";
        std::cout << "\n";
    }
    std::cout << "Having clause: " << this->having_clause_ << std::endl;
}

void esqlxx::generator::generate(bool const count_rows)
{
    this->get_table_info();

    this->generate_headers();
    this->s_ += "\n";
    this->generate_struct();
    this->s_ += "\n";
    this->generate_helpers(count_rows);
    this->s_ += "\n";
    this->generate_main();

    this->write_file();
}

void esqlxx::generator::get_table_info()
{
    pqxx::connection connection(this->connection_string_);
    pqxx::work tx(connection);
    std::unordered_map<std::string, std::string> psql_types = {
        {"integer", "int"},
        {"character varying", "std::string"},
        {"character", "std::string"},
        {"date", "std::string"},
    };

    int i = 0;
    for (const auto& [attribute, psql_type] : tx.query<std::string, std::string>(
             std::format("SELECT column_name, data_type FROM information_schema.columns WHERE table_schema = 'public' AND table_name = '{}';", this->table_)))
    {
        this->attributes_[attribute] = {i++, psql_types[psql_type]};
    }
}

void esqlxx::generator::generate_headers()
{
    this->s_ +=
        "#include <iomanip>\n"
        "#include <iostream>\n"
        "#include <pqxx/pqxx>\n"
        "#include <set>\n"
        "#include <string>\n"
        "#include <unordered_map>\n"
        "#include <vector>\n";
}

void esqlxx::generator::generate_struct()
{
    this->s_ +=
        "struct output_tuple {\n"
        "  std::unordered_map<int, int> n;\n";
    for (auto const& grouping_attribute : this->grouping_attributes_)
    {
        this->s_ += "  " + std::get<1>(this->attributes_[grouping_attribute]) + " " + grouping_attribute + ";\n";
    }
    std::vector<esqlxx::aggregate> aggregate_fns;
    for (auto const& grouping_variable : this->grouping_variables_)
    {
        for (auto const& aggregate_fn : grouping_variable.get_aggregates())
        {
            aggregate_fns.push_back(aggregate_fn);
        }
    }
    for (auto const& aggregate_fn : aggregate_fns)
    {
        this->s_ = this->s_ + "  mutable ";
        if (this->attributes_.contains(aggregate_fn.get_string()))
        {
            this->s_ += std::get<1>(this->attributes_[aggregate_fn.get_string()]);
        }
        else
        {
            this->s_ += "double ";
        }
        this->s_ += aggregate_fn.get_string() + "{};\n";
    }
    this->s_ += "};\n";
}

void esqlxx::generator::generate_helpers(bool const count_rows)
{
    this->s_ +=
        "template<typename T> void print(T t, const int& width) {\n"
        "  std::cout << std::left << std::setw(width) << std::setfill(' ') << t;\n"
        "}\n";

    this->s_ += "\n";

    this->s_ +=
        "std::vector<std::string> split(std::string const& s, char const c) {\n"
        "  std::stringstream ss(s);\n"
        "  std::vector<std::string> tokens;\n"
        "  std::string token;\n"
        "  while (getline(ss, token, c)) {\n"
        "    token.erase(token.begin(), std::ranges::find_if(token, [](int const e) {\n"
        "      return !isspace(e);\n"
        "    }));\n"
        "    if (!token.empty()) {\n"
        "      tokens.push_back(token);\n"
        "    }\n"
        "  }\n"
        "  return tokens;\n"
        "}\n";

    this->s_ += "\n";

    this->s_ +=
        "void print_output(std::unordered_map<std::string, output_tuple> const& output) {\n";
    for (auto const& e : this->output_structure_)
    {
        this->s_ += "  print(\"" + e + "\", 20);\n";
    }
    this->s_ +=
        "  std::cout << std::endl;\n"
        "  int i = 0;\n"
        "  for (const auto& [_, r] : output) {\n"
        "    if (" + (this->having_clause_.length() > 0 ? this->having_clause_ : "true") + ") {\n";
    for (auto const& e : this->output_structure_)
    {
        this->s_ += "      print(" + e + ", 20);\n";
    }
    this->s_ +=
        "      std::cout << std::endl;\n"
        "      i++;\n"
        "    }\n"
        "  }\n";

    this->s_ += count_rows ? "  std::cout << \"Rows: \" << i << std::endl;\n" : "";
    this->s_ += "}\n";
}

void esqlxx::generator::generate_main()
{
    this->s_ +=
        "int main() {\n"
        "  pqxx::connection connection(\"" + this->connection_string_ + "\");\n"
        "  pqxx::work tx(connection);\n"
        "  std::unordered_map<std::string, output_tuple> output;\n" +
        this->generate_init_loop() +
        this->generate_scan_loops() +
        "  print_output(output);\n"
        "  return 0;\n"
        "}\n";
}

std::string esqlxx::generator::generate_init_loop() const
{
    auto [keys, values] = esqlxx::utility::sort_split(this->attributes_);
    std::string s;
    s +=
        "  std::set<std::string> set;\n"
        "  for (auto [" + esqlxx::utility::join(keys, ", ") + "] : tx.query<" + esqlxx::utility::join(values, ", ") + ">(\"SELECT * FROM " + this->table_ + "\")) {\n" +
        "    if (auto const hash = " + this->generate_hash_fn(true) + "; !set.contains(hash)) {\n"
        "      output_tuple r;\n";
    for (auto const& grouping_attribute : this->grouping_attributes_)
    {
        s += "      r." + grouping_attribute + " = " + grouping_attribute + ";\n";
    }
    int i = 0;
    for (auto const& grouping_variable : this->grouping_variables_)
    {
        for (auto const& aggregate_fn : grouping_variable.get_aggregates())
        {
            s += std::format("      r.n[{}] = 0;\n", i) + std::format("      r.{} = {};\n", aggregate_fn.get_string(), esqlxx::generator::generate_aggregate_init(aggregate_fn));
            i++;
        }
    }
    s +=
        "      output[hash] = r;\n"
        "      set.insert(hash);\n"
        "    }\n"
        "  }\n"
        "";

    return s;
}

std::string esqlxx::generator::generate_scan_loops() const
{
    auto [keys, values] = esqlxx::utility::sort_split(this->attributes_);
    std::string s;
    for (auto const& grouping_variable : this->grouping_variables_)
    {
        s +=
            "  for (auto [" + esqlxx::utility::join(keys, ", ") + "] : tx.query<" + esqlxx::utility::join(values, ", ") + ">(\"SELECT * FROM " + this->table_ + "\")) {\n"
            "    for (auto const& [_, r] : output) {\n"
            "      auto const hash = " + this->generate_hash_fn(false) + ";\n"
            "      if (" + grouping_variable.get_predicate().get_string() + ") {\n";
        for (auto const& aggregate : grouping_variable.get_aggregates())
        {
            s +=
                std::format("        output[hash].n[{}]++;\n", aggregate.get_i()) +
                std::format("        output[hash].{} {}", aggregate.get_string(), esqlxx::generator::generate_aggregate_inc(aggregate));
        }
        s +=
            "      }\n"
            "    }\n"
            "  }\n";
    }
    return s;
}

std::string esqlxx::generator::generate_hash_fn(bool const init) const
{
    std::string s;
    s += "std::format(\"";
    for (int i = 0; i < this->grouping_attributes_.size(); i++)
    {
        s += "{}";
        if (i != this->grouping_attributes_.size() - 1)
        {
            s += ";";
        }
        else
        {
            s += "\", ";
        }
    }
    for (int i = 0; i < this->grouping_attributes_.size(); i++)
    {
        s += (init ? "" : "r.") + this->grouping_attributes_[i];
        if (i != this->grouping_attributes_.size() - 1)
        {
            s += ", ";
        }
        else
        {
            s += ")";
        }
    }
    return s;
}

int esqlxx::generator::generate_aggregate_init(esqlxx::aggregate const& aggregate)
{
    switch (aggregate.get_type())
    {
    case esqlxx::AggregateType::Count:
    case esqlxx::AggregateType::Sum:
    case esqlxx::AggregateType::Average:
    default:
        return 0;
    case Min:
        return std::numeric_limits<int>::max();
    case Max:
        return std::numeric_limits<int>::min();
    }
}

std::string esqlxx::generator::generate_aggregate_inc(esqlxx::aggregate const& aggregate)
{
    switch (aggregate.get_type())
    {
    case Count:
    default:
        return "+= 1;\n";
    case Sum:
        return std::format("+= {};\n", aggregate.get_attribute());
    case Average:
        return std::format("= output[hash].{} + ({} - output[hash].{}) / output[hash].n[{}];\n",
                           aggregate.get_string(),
                           aggregate.get_attribute(),
                           aggregate.get_string(),
                           aggregate.get_i());
    case Min:
        return std::format("= std::min(output[hash].{}, static_cast<double>({}));\n", aggregate.get_string(), aggregate.get_attribute());
    case Max:
        return std::format("= std::max(output[hash].{}, static_cast<double>({}));\n", aggregate.get_string(), aggregate.get_attribute());
    }
}

void esqlxx::generator::write_file() const
{
    std::ofstream out("../code.cpp");
    out << this->s_;
    out.close();
}
