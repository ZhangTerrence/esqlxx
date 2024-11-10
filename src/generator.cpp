#include "../include/generator.hpp"

#include <fstream>

#include "../include/utility.hpp"

#include <iostream>
#include <pqxx/pqxx>
#include <utility>

esqlxx::generator::generator(std::string connection_string, std::string table)
    : connection_string_(std::move(connection_string)), table_(std::move(table))
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

    this->output_structure_ = output_structure;
    this->grouping_attributes_ = grouping_attributes;
    this->grouping_variables_ = esqlxx::grouping_variable::get_grouping_variables(n, grouping_variable_aggregates, grouping_variable_predicates);
    this->having_clause_ = esqlxx::utility::to_cpp(having_clause);
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
    for (const auto& e : this->grouping_attributes_)
    {
        std::cout << e << " ";
    }
    std::cout << "\n";
    std::cout << "Grouping variables: \n";
    for (const auto& e : this->grouping_variables_)
    {
        std::cout << e.get_i() << " ";
        std::cout << "Aggregate Functions: ";
        for (auto const& fn : e.get_aggregate_fns())
        {
            std::cout << fn << " ";
        }
        std::cout << "Predicates: ";
        for (auto const& predicate : e.get_predicates())
        {
            std::cout << predicate.string() << " ";
        }
        std::cout << "\n";
    }
    std::cout << "Having clause: " << this->having_clause_ << std::endl;
}

void esqlxx::generator::generate()
{
    get_table_info();

    generate_headers();
    this->s_ += "\n";
    generate_struct();
    this->s_ += "\n";
    generate_helpers();
    this->s_ += "\n";
    generate_main();

    write_file();
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

    for (const auto& [attribute, psql_type] : tx.query<std::string, std::string>(
             std::format("SELECT column_name, data_type FROM information_schema.columns WHERE table_schema = 'public' AND table_name = '{}';", this->table_)))
    {
        this->attributes_[attribute] = psql_types[psql_type];
    }
}

void esqlxx::generator::generate_headers()
{
    this->s_ +=
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
    std::vector<std::string> aggregate_fns;
    for (auto const& grouping_variable : this->grouping_variables_)
    {
        for (auto const& aggregate_fn : grouping_variable.get_aggregate_fns())
        {
            aggregate_fns.push_back(aggregate_fn);
        }
    }
    for (auto const& aggregate_fn : aggregate_fns)
    {
        this->s_ = this->s_ + "  mutable " + (this->attributes_.contains(aggregate_fn) ? this->attributes_[aggregate_fn] : "int") + " " + aggregate_fn + "{};\n";
    }
    this->s_ += "};\n";
}

void esqlxx::generator::generate_helpers()
{
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
}

void esqlxx::generator::generate_main()
{
    this->s_ +=
        "int main() {\n"
        "  pqxx::connection connection(\"" + this->connection_string_ + "\");\n"
        "  pqxx::work tx(connection);\n"
        "  std::unordered_map<std::string, output_tuple> output;\n"
        + generate_init_loop() +
        "  return 0;\n"
        "}\n";
}

std::string esqlxx::generator::generate_init_loop() const
{
    std::string s;

    s +=
        "  std::set<std::string> set;\n"
        "  for (auto tuple : tx.exec(\"SELECT * FROM " + this->table_ + "\")) {\n"
        "    if (auto const hash = " + generate_hash_fn() + "; !set.contains(hash)) {\n"
        "      output_tuple r;\n";
    std::vector<std::tuple<int, std::string>> aggregate_fns;
    for (auto const& grouping_variable : this->grouping_variables_)
    {
        for (auto const& aggregate_fn : grouping_variable.get_aggregate_fns())
        {
            aggregate_fns.emplace_back(grouping_variable.get_i(), aggregate_fn);
        }
    }
    for (auto const& [i, aggregate_fn] : aggregate_fns)
    {
        s += std::format("      r.n[{}] = 0;\n", i) + std::format("      r.{} = 0;\n", aggregate_fn);
    }
    s +=
        "      set.insert(hash);\n"
        "    }\n"
        "  }\n"
        "";

    return s;
}

std::string esqlxx::generator::generate_hash_fn() const
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
        s += "tuple[\"" + this->grouping_attributes_[i] + "\"].as<" + this->attributes_.at(this->grouping_attributes_[i]) + ">()";
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


void esqlxx::generator::write_file() const
{
    std::ofstream out("../code.cpp");
    out << this->s_;
    out.close();
}
