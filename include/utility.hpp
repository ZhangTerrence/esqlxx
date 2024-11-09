#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <vector>

namespace esqlxx
{
    class utility
    {
    public:
        static std::vector<std::string> split(std::string const& s, char c);
        static std::string to_cpp(std::string s);
    };
}

#endif
