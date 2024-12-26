#include "day17_CPU.h"

#include <algorithm>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/core.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>

#include <iostream>
#include <string>
#include <vector>

using REG = uint64_t;

void part1(REG A, REG B, REG C)
{
    std::vector<REG> result1;
    for (auto cpu {CPU(A, B, C)};;)
    {
        auto v = cpu.run_once();
        if (!v)
            break;
        result1.push_back(v.value());
    }

    fmt::println(
        "1: {}",
        boost::algorithm::join(result1 | ranges::views::transform([](REG i) { return std::to_string(i); }), ","));
}

// Inspiration in https://old.reddit.com/r/adventofcode/comments/1hg38ah/2024_day_17_solutions/m36hefc/
std::optional<REG> process2(std::vector<REG> program1, REG prev = 0)
{
    if (program1.empty()) return prev;

    for (REG i = 0; i < (1 << 10); ++i)
    {
        if ((i >> 3) == (prev & 0x7f))
        {
            auto cpu {CPU {i, 0, 0}};
            const auto x {cpu.run_once()};
            if (x && x.value() == program1.front())
            {
                auto r {process2(program1 | ranges::views::drop(1) | ranges::to_vector, (prev << 3) | (i & 0x7))};
                if (r)
                    return r;
            }
        }
    }
    return {};
}

void part2(std::vector<REG> program)
{
    std::reverse(program.begin(), program.end());

    fmt::println("2: {}", process2(program).value());
}

int main()
{
    REG A{0}, B{0}, C{0};
    std::vector<REG> program;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            continue;
        }
        else if (boost::starts_with(line, "Register A: "))
        {
            A = std::stoul(line.substr(12));
        }
        else if (boost::starts_with(line, "Register B: "))
        {
            B = std::stoul(line.substr(12));
        }
        else if (boost::starts_with(line, "Register C: "))
        {
            C = std::stoul(line.substr(12));
        }
        else if (boost::starts_with(line, "Program: "))
        {
            std::vector<std::string> parts;
            boost::algorithm::split(parts, line.substr(9), boost::algorithm::is_any_of(","), boost::token_compress_on);
            program = parts | ranges::views::transform([](std::string const& v) { return (REG)std::stoul(v); }) | ranges::to_vector;
        }
    }

    part1(A, B, C);
    part2(program);

    return 0;
}
