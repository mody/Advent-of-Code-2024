#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/regex.hpp>

#include <iostream>

static boost::regex RX {"mul\\(([0-9]{1,3}),([0-9]{1,3})\\)"};

int main()
{
    uint64_t result1 {0};

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        boost::sregex_iterator it(line.begin(), line.end(), RX);
        boost::sregex_iterator end {};

        for (; it != end; ++it)
        {
            auto const& match {*it};
            assert(match.size() == 3);

            result1 += std::stoul(match[1]) * std::stoul(match[2]);
        }
    }

    fmt::println("1: {}\n", result1);

    return 0;
}
