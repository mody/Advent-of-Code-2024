#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/view/drop.hpp>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <vector>

using Level = std::vector<unsigned>;
using Levels = std::vector<Level>;

void part1(Levels levels)
{
    unsigned safe{0};

    for (auto const& level : levels) {
        if (level.at(0) == level.at(1))
        {
            continue;
        }
        const auto sign {level.at(0) > level.at(1) ? 1 : -1};

        bool valid{true};
        unsigned last {level.at(0)};
        for (auto val : level | ranges::views::drop(1))
        {
            const auto diff {(last - val) * sign};
            last = val;
            if (diff >= 1 && diff <= 3) {
                // ok
            } else {
                valid = false;
                break;
            }
        }
        if (valid)
            ++safe;
    }

    fmt::println("1: {}", safe);
}


int main()
{
    Levels levels;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);

        Level level;
        level.reserve(parts.size());
        for (auto const& val : parts)
        {
            level.push_back(std::stoul(val));
        }
        levels.emplace_back(std::move(level));
    }

    part1(levels);

    return 0;
}
