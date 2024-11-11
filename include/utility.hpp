#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace esqlxx
{
    class utility
    {
    public:
        static std::vector<std::string> split(std::string const& s, char c);

        static std::string join(std::vector<std::string> const& vector, std::string const& separator);

        template <typename K, typename V>
        static std::tuple<std::vector<K>, std::vector<V>> sort_split(std::unordered_map<K, std::tuple<int, V>> map);

        static std::string to_cpp(std::string s);
    };

    template <typename K, typename V>
    std::tuple<std::vector<K>, std::vector<V>> utility::sort_split(std::unordered_map<K, std::tuple<int, V>> map)
    {
        std::vector<std::tuple<int, K>> key_tuples;
        std::vector<std::tuple<int, V>> value_tuples;
        for (auto const& [key, value] : map)
        {
            int i = std::get<0>(value);
            key_tuples.emplace_back(i, key);
            value_tuples.emplace_back(i, std::get<1>(value));
        }
        std::ranges::sort(key_tuples, [](auto const& a, auto const& b)
        {
            return std::get<0>(a) < std::get<0>(b);
        });
        std::ranges::sort(value_tuples, [](auto const& a, auto const& b)
        {
            return std::get<0>(a) < std::get<0>(b);
        });
        std::vector<K> keys;
        std::vector<V> values;
        for (auto const& [_, key] : key_tuples)
        {
            keys.push_back(key);
        }
        for (auto const& [_, value] : value_tuples)
        {
            values.push_back(value);
        }
        return std::make_tuple(keys, values);
    }
}

#endif
