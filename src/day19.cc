#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <iostream>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <vector>

using Dictionary = std::vector<std::string>;
using WordViewSet = std::unordered_set<std::string_view>;

struct Part1 {
    Part1(Dictionary d)
        : dict {std::move(d)}
    {
    }

    bool process(std::string_view word) noexcept {
        invalid.clear();
        return inner(word);
    }

protected:
    bool inner(std::string_view word) noexcept
    {
        if (word.empty())
            return true;
        if (invalid.contains(word))
            return false;

        bool solved {false};
        for (auto const& d : dict) {
            if (auto pos {word.find(d)}; pos != std::string::npos) {
                std::string_view left {word.substr(0, pos)};
                std::string_view right {word.substr(pos + d.size())};
                if (!inner(left)) {
                    invalid.insert(left);
                    continue;
                }
                if (!inner(right)) {
                    invalid.insert(right);
                    continue;
                }
                solved = true;
                break;
            }
        }
        return solved;
    }

    const Dictionary dict;
    WordViewSet invalid;
};


// inspiration from https://github.com/mquig42/AdventOfCode2024/blob/main/src/day19.cpp
struct Part2 {
    Part2(Dictionary d)
        : dict {std::move(d)}
    {
    }

    uint64_t process(std::string_view word) noexcept {
        cache.clear();
        return inner(word, 0);
    }

protected:
    uint64_t inner(std::string_view word, const unsigned pos)
    {
        if (word.empty())
            return 1;

        if (auto it{cache.find(pos)}; it != cache.end())
            return it->second;

        uint64_t sum{0};
        for (auto const& d : dict) {
            if (word.starts_with(d)) {
                sum += inner(word.substr(d.size()), pos + d.size());
            }
        }
        cache.insert({pos, sum});
        return sum;
    }

    const Dictionary dict;
    std::unordered_map<unsigned, uint64_t> cache;
};


int main()
{
    Dictionary dict;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" ,"), boost::token_compress_on);

        for (auto w : parts) {
            dict.push_back(std::move(w));
        }
    }
    std::sort(dict.begin(), dict.end(), [](const std::string& l, const std::string& r) { return l.size() > r.size(); });

    Part1 part1{dict};
    Part2 part2{dict};

    unsigned valid1{0};
    uint64_t valid2{0};

    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;

        valid1 += part1.process(line) ? 1 : 0;

        valid2 += part2.process(line);
    }
    fmt::println("1: {}", valid1);
    fmt::println("2: {}", valid2);

    return 0;
}
