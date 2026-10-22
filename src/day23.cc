#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <ranges>
#include <set>
#include <sstream>

using Node = std::array<char, 2>;
using NodeSet = std::set<Node>;
using Nodes = std::map<Node, NodeSet>;


void part1(Nodes const& nodes)
{
    std::set<std::set<Node>> set3;

    for (auto const& [k1, v1] : nodes) {
        for (auto const& k2 : v1) {
            auto const& v2{nodes.at(k2)};
            if (v2.contains(k1)) {
                std::vector<Node> common;
                std::ranges::set_intersection(v1, v2, std::back_inserter(common));

                for (auto const& k3 : common) {
                    if (k1.at(0) == 't' || k2.at(0) == 't' || k3.at(0) == 't') {
                        set3.insert({k1, k2, k3});
                    }
                }
            }
        }
    }

    fmt::println("1: {}", set3.size());
}


bool find_key(NodeSet const& k, NodeSet const& v)
{
    if (v.size() < k.size())
        return false;

    NodeSet common;
    std::ranges::set_intersection(k, v, std::inserter(common, common.end()));

    return common.size() == k.size();
}


void dive(Nodes const& nodes, NodeSet key, NodeSet common, Nodes::const_iterator it, unsigned level, unsigned max_connections, NodeSet& top)
{
    if (level == max_connections) return;
    if (it == nodes.cend()) return;
    if (common.empty()) return;

    for (auto i2 = it; i2 != nodes.cend(); ++i2) {
        if (!common.contains(i2->first) || !find_key(key, i2->second))
            continue;

        NodeSet knext{key};
        knext.insert(i2->first);

        if (knext.size() > top.size()) {
            top = knext;
        }

        NodeSet c2;
        std::ranges::set_intersection(common, i2->second, std::inserter(c2, c2.end()));

        dive(nodes, knext, c2, std::next(i2), level + 1, max_connections, top);
    }
}


void part2(Nodes const& nodes)
{
    unsigned long max_connections{0};

    for (auto const& [from, to] : nodes) {
        max_connections = std::max(max_connections, to.size());
    }

    NodeSet top;
    for (auto it = nodes.begin(); it != nodes.cend(); ++it) {
        dive(nodes, NodeSet{it->first}, it->second, std::next(it), 0, max_connections, top);
    }

    std::stringstream ss;
    for (auto const& p : top) {
        ss << p.at(0) << p.at(1) << ",";
    }
    auto s{ss.str()};
    fmt::println("2: {}", std::string_view(&s[0], s.size() - 1));
}


int main()
{
    Nodes nodes;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;

        if (line.at(2) != '-') {
            throw std::runtime_error("Invalid input!");
        }

        Node a({line.at(0), line.at(1)});
        Node b({line.at(3), line.at(4)});

        nodes[a].insert(b);
        nodes[b].insert(a);
    }

    part1(nodes);
    part2(nodes);

    return 0;
}
