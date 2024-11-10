#include <iostream>
#include <pqxx/pqxx>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct output_tuple {
  std::unordered_map<int, int> n;
  mutable int gv3_sum_quant{};
  mutable int gv3_avg_quant{};
  mutable int gv2_sum_quant{};
  mutable int gv1_sum_quant{};
  mutable int gv1_avg_quant{};
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

int main() {
  pqxx::connection connection("postgresql://terrence:terrence@127.0.0.1:5432/cs562");
  pqxx::work tx(connection);
  std::unordered_map<std::string, output_tuple> output;
  std::set<std::string> set;
  for (auto tuple : tx.exec("SELECT * FROM sales")) {
    if (auto const hash = std::format("{}", tuple["cust"].as<std::string>()); !set.contains(hash)) {
      output_tuple r;
      r.n[3] = 0;
      r.gv3_sum_quant = 0;
      r.n[3] = 0;
      r.gv3_avg_quant = 0;
      r.n[2] = 0;
      r.gv2_sum_quant = 0;
      r.n[1] = 0;
      r.gv1_sum_quant = 0;
      r.n[1] = 0;
      r.gv1_avg_quant = 0;
      set.insert(hash);
    }
  }
  return 0;
}
