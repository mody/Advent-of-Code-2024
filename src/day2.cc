#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <vector>
#include <ranges>

using Level = std::vector<unsigned>;
using Levels = std::vector<Level>;

int find_error(Level const& level)
{
    unsigned err{0};
    if (level.at(0) == level.at(1))
    {
        return err;
    }
    const auto sign {level.at(0) > level.at(1) ? 1 : -1};

    unsigned last {level.at(0)};
    for (auto val : level | std::views::drop(1))
    {
        ++err;
        const auto diff {(last - val) * sign};
        last = val;
        if (diff >= 1 && diff <= 3)
        {
            // ok
        }
        else
        {
            return err;
        }
    }
    return -1;
}


void part1(Levels levels)
{
    unsigned safe{0};

    for (auto const& level : levels) {
        if (find_error(level) == -1)
            ++safe;
    }

    fmt::println("1: {}", safe);
}


void part2(Levels levels)
{
    unsigned safe{0};

    auto test_level = [](Level level) -> bool
    {
        auto idx = find_error(level);
        if (idx == -1)
        {
            return true;
        }
        level.erase(level.begin() + idx);
        idx = find_error(level);
        return (idx == -1);
    };

    for (auto level : levels)
    {
        if (test_level(level))
        {
            ++safe;
        }
        else
        {
            std::reverse(level.begin(), level.end());
            if (test_level(level))
            {
                ++safe;
            }
        }
    }

    fmt::println("2: {}", safe);
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
    part2(levels);

    return 0;
}
