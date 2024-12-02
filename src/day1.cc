#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <vector>
#include <map>

using Val = int64_t;
using Vector = std::vector<Val>;

void part1(Vector v1, Vector v2)
{
    assert(v1.size() == v2.size());

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());

    uint64_t sum{0};

    for (unsigned i {0}; i < v1.size(); ++i)
    {
        sum += std::abs(v1.at(i) - v2.at(i));
    }

    fmt::print("1: {}\n", sum);
}


void part2(Vector v1, Vector v2)
{
    assert(v1.size() == v2.size());

    std::map<Val, Val> counts;
    uint64_t sum{0};

    for (auto const v : v1) {
        auto const it {counts.find(v)};
        if (it != counts.end()) {
            sum += v * it->second;
        } else {
            const auto times {std::count_if(v2.cbegin(), v2.cend(), [&v](Val const& vv2) { return vv2 == v; })};
            counts[v] = times;
            sum += v * times;
        }
    }

    fmt::print("2: {}\n", sum);
}

int main()
{
    Vector v1, v2;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
        assert(parts.size() == 2);

        v1.push_back(std::stoul(parts.at(0)));
        v2.push_back(std::stoul(parts.at(1)));
    }

    part1(v1, v2);
    part2(v1, v2);

    return 0;
}
