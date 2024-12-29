#include <algorithm>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <iostream>
#include <unordered_set>
#include <vector>

using Dictionary = std::vector<std::string>;
using Invalid = std::unordered_set<std::string_view>;

bool part1(Dictionary const& dict, Invalid& invalid, std::string_view word)
{
    if (word.empty()) return true;
    if (invalid.contains(word)) return false;

    bool solved {false};
    for (auto const& d : dict) {
        if (auto pos {word.find(d)}; pos != std::string::npos) {
            std::string_view left {word.substr(0, pos)};
            std::string_view right {word.substr(pos + d.size())};
            if (!part1(dict, invalid, left)) {
                invalid.insert(left);
                continue;
            }
            if (!part1(dict, invalid, right)) {
                invalid.insert(right);
                continue;
            }
            solved = true;
            break;
        }
    }
    return solved;
}

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

    unsigned valid1{0};
    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;
        Invalid i;
        valid1 += part1(dict, i, line) ? 1 : 0;
    }
    fmt::println("1: {}", valid1);

    return 0;
}
