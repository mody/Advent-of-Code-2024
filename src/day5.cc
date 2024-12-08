#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/container_hash/hash.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <unordered_set>
#include <vector>

using Ordering = std::unordered_set<std::pair<unsigned, unsigned>, boost::hash<std::pair<unsigned, unsigned>>>;
using Update = std::vector<unsigned>;
using Updates = std::vector<Update>;

void process(Ordering const& ordering, Updates const& updates)
{
    uint64_t sum1 {0}, sum2 {0};

    for (auto update : updates)
    {
        bool ok {true};
        unsigned prev = update.at(0);
        for (auto const& val : update | std::views::drop(1))
        {
            if (ordering.contains(std::make_pair(prev, val)))
            {
                // pass
            }
            else if (ordering.contains(std::make_pair(val, prev)))
            {
                // fail
                ok = false;
                break;
            }
            prev = val;
        }
        if (ok)
        {
            sum1 += update.at(update.size() / 2);
        }
        else
        {
            std::ranges::sort(
                update, [&ordering](unsigned a, unsigned b) { return ordering.contains(std::make_pair(a, b)); });
            sum2 += update.at(update.size() / 2);
        }
    }
    fmt::println("1: {}", sum1);
    fmt::println("2: {}", sum2);
}

int main()
{
    Ordering ord;
    Updates updates;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of("|"), boost::token_compress_on);

        ord.insert(std::make_pair(std::stoi(parts.at(0)), std::stoi(parts.at(1))));
    }

    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(","), boost::token_compress_on);

        Update up;
        for (auto const& v : parts)
        {
            up.push_back(std::stoul(v));
        }
        updates.push_back(std::move(up));
    }

    process(ord, updates);

    return 0;
}
