#include "../include/utility.hpp"

#include <regex>
#include <sstream>

std::vector<std::string> esqlxx::utility::split(std::string const& s, char const c)
{
    std::stringstream ss(s);
    std::vector<std::string> tokens;
    std::string token;

    while (getline(ss, token, c))
    {
        token.erase(token.begin(), std::ranges::find_if(token, [](int const e)
        {
            return !isspace(e);
        }));

        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }

    return tokens;
}

std::string esqlxx::utility::join(std::vector<std::string> const& vector, std::string const& separator)
{
    std::string s;
    for (int i = 0; i < vector.size(); i++)
    {
        s += vector[i];
        if (i != vector.size() - 1)
        {
            s += separator;
        }
    }
    return s;
}

std::string esqlxx::utility::to_cpp(std::string s)
{
    s = std::regex_replace(s, std::regex("\'"), "\"");
    s = std::regex_replace(s, std::regex("="), "==");
    s = std::regex_replace(s, std::regex("AND"), "&&");
    s = std::regex_replace(s, std::regex("OR"), "||");
    s = std::regex_replace(s, std::regex("NOT"), "!");
    return s;
}
