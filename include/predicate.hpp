#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <string>

namespace esqlxx
{
    class predicate
    {
    public:
        explicit predicate(std::string const& predicate);

        [[nodiscard]] std::string string() const;

    private:
        std::string lhs_;
        std::string operator_;
        std::string rhs_;
    };
}

#endif
