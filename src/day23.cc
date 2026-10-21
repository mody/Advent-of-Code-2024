#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <set>

using Node = std::array<char, 2>;
using Nodes = std::map<Node, std::set<Node>>;


void part1(Nodes const& nodes)
{
    //  for (auto const& [from, to] : nodes) {
    //      fmt::println("{}-{}", from, to);
    //  }

    std::set<std::set<Node>> set3;

    for (auto const& [k1, v1] : nodes) {
        for (auto const& k2 : v1) {
            auto const& v2{nodes.at(k2)};
            if (v2.contains(k1)) {
                std::vector<Node> common;
                std::ranges::set_intersection(v1, v2, std::back_inserter(common));

                for (auto const& k3 : common) {
                    if (k1.at(0) == 't' || k2.at(0) == 't' || k3.at(0) == 't') {

                        std::set<Node> n3;
                        n3.insert(k1);
                        n3.insert(k2);
                        n3.insert(k3);

                        if (set3.insert(std::move(n3)).second) {
                            //  fmt::println("{}, {}, {}", k1, k2, k3);
                        }
                    }
                }
            }
        }
    }

    fmt::println("1: {}", set3.size());
}


int main()
{
    Nodes nodes;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        if (line.at(2) != '-') {
            throw std::runtime_error("Invalid input!");
        }

        Node a({line.at(0), line.at(1)});
        Node b({line.at(3), line.at(4)});

        nodes[a].insert(b);
        nodes[b].insert(a);
    }

    part1(nodes);

    return 0;
}
