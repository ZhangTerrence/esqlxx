#include <iostream>
#include <pqxx/pqxx>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct output_tuple {
  std::unordered_map<int, int> n;
  std::string cust;
  mutable int gv1_count_quant{};
  mutable int gv2_sum_quant{};
  mutable int gv3_max_quant{};
};

std::vector<std::string> split(std::string const& s, char const c) {
  std::stringstream ss(s);
  std::vector<std::string> tokens;
  std::string token;
  while (getline(ss, token, c)) {
    token.erase(token.begin(), std::ranges::find_if(token, [](int const e) {
      return !isspace(e);
    }));
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }
  return tokens;
}
void print_output(std::unordered_map<std::string, output_tuple> const& output) {
  for (const auto& [key, value] : output) {
    std::cout << value.cust << " ";
    std::cout << value.gv1_count_quant << " ";
    std::cout << value.gv2_sum_quant << " ";
    std::cout << value.gv3_max_quant << " ";
    std::cout << std::endl;
  }
}

int main() {
  pqxx::connection connection("postgresql://terrence:terrence@127.0.0.1:5432/cs562");
  pqxx::work tx(connection);
  std::unordered_map<std::string, output_tuple> output;
  std::set<std::string> set;
  for (auto [cust, prod, day, month, year, state, quant, date] : tx.query<std::string, std::string, int, int, int, std::string, int, std::string>("SELECT * FROM sales")) {
    if (auto const hash = std::format("{}", cust); !set.contains(hash)) {
      output_tuple r;
      r.cust = cust;
      r.n[0] = 0;
      r.gv1_count_quant = 0;
      r.n[1] = 0;
      r.gv2_sum_quant = 0;
      r.n[2] = -2147483648;
      r.gv3_max_quant = 0;
      output[hash] = r;
      set.insert(hash);
    }
  }
  for (auto [cust, prod, day, month, year, state, quant, date] : tx.query<std::string, std::string, int, int, int, std::string, int, std::string>("SELECT * FROM sales")) {
    auto const hash = std::format("{}", cust);
    if (state == "NY") {
      output[hash].n[0]++;
      output[hash].gv1_count_quant += 1;
    }
  }
  for (auto [cust, prod, day, month, year, state, quant, date] : tx.query<std::string, std::string, int, int, int, std::string, int, std::string>("SELECT * FROM sales")) {
    auto const hash = std::format("{}", cust);
    if (state == "NJ") {
      output[hash].n[1]++;
      output[hash].gv2_sum_quant += quant;
    }
  }
  for (auto [cust, prod, day, month, year, state, quant, date] : tx.query<std::string, std::string, int, int, int, std::string, int, std::string>("SELECT * FROM sales")) {
    auto const hash = std::format("{}", cust);
    if (state == "CT") {
      output[hash].n[2]++;
      output[hash].gv3_max_quant = std::max(output[hash].gv3_max_quant, quant);
    }
  }
  print_output(output);
  return 0;
}
