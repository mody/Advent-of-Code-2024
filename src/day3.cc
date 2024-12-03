#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/regex.hpp>

#include <iostream>

int main()
{
    uint64_t result1 {0}, result2 {0};

    auto part1 = [&result1](std::string const& input) {
        static boost::regex RX {"mul\\(([0-9]{1,3}),([0-9]{1,3})\\)"};

        boost::sregex_iterator it(input.begin(), input.end(), RX);
        boost::sregex_iterator end {};

        for (; it != end; ++it)
        {
            auto const& match {*it};
            assert(match.size() == 3);

            result1 += std::stoul(match[1]) * std::stoul(match[2]);
        }
    };

    auto part2 = [&result2](std::string const& input) {
        static boost::regex RX {"(mul)\\(([0-9]{1,3}),([0-9]{1,3})\\)|(do)\\(\\)|(don't)\\(\\)"};
        static bool enabled {true};

        boost::sregex_iterator it(input.begin(), input.end(), RX);
        boost::sregex_iterator end {};

        for (; it != end; ++it)
        {
            auto const& match {*it};

            if (!match.str(5).empty()) {
                // don't
                enabled = false;
            }
            else if (!match.str(4).empty()) {
                // do
                enabled = true;
            } else if (enabled && !match.str(1).empty()) {
                // mul
                result2 += std::stoul(match.str(2)) * std::stoul(match.str(3));
            }
        }
    };

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        part1(line);
        part2(line);
    }

    fmt::println("1: {}", result1);
    fmt::println("2: {}", result2);

    return 0;
}
