#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <deque>
#include <iostream>
#include <ranges>
#include <vector>

struct Equation
{
    uint64_t result {0}, tmp {0};
    std::deque<uint64_t> values;
};

using Equations = std::vector<Equation>;

bool process(Equation e)
{
    const uint64_t v1 {e.values.front()};
    e.values.pop_front();

    const uint64_t v_plus {e.tmp + v1};
    const uint64_t v_times {e.tmp * v1};

    if (e.values.empty())
        return v_plus == e.result || v_times == e.result;

    if (v_plus <= e.result)
    {
        e.tmp = v_plus;
        if (process(e))
            return true;
    }

    if (v_times <= e.result)
    {
        e.tmp = v_times;
        if (process(e))
            return true;
    }

    return false;
}

void part1(Equations equations)
{
    uint64_t sum {0};

    for (auto e : equations)
    {
        e.tmp = e.values.front();
        e.values.pop_front();

        if (process(e))
        {
            sum += e.result;
        }
    }

    fmt::println("1: {}", sum);
}

int main()
{
    Equations equations;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(": "), boost::token_compress_on);

        Equation e;
        e.result = std::stoull(parts.at(0));
        assert(parts.at(0) == std::to_string(e.result));
        for (const auto& val : parts | std::views::drop(1))
        {
            e.values.push_back(std::stoull(val));
            assert(val == std::to_string(e.values.back()));
        }
        assert(e.values.size() > 1);
        equations.push_back(std::move(e));
    }

    part1(equations);

    return 0;
}
