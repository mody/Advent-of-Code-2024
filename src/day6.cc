#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>

using Coord = int32_t;
using Point = Gfx_2d::Point<Coord>;

using Map = std::unordered_map<Point, char, boost::hash<Point>>;

struct Guard
{
    Point px;
    Gfx_2d::Direction dir;
};

static const std::map<char, Gfx_2d::Direction> TO_DIR{
    {'^', Gfx_2d::Up},
    {'v', Gfx_2d::Down},
    {'<', Gfx_2d::Left},
    {'>', Gfx_2d::Right},
};

void part1(Map const& world, Guard g)
{
    std::unordered_set<Point, boost::hash<Point>> visited;
    visited.insert(g.px);

    for (; world.contains(g.px);)
    {
        Point next {g.px + g.dir};
        const auto it = world.find(next);
        if (it == world.end()) break;
        if (it->second == '#') {
            next = g.px;
            g.dir = g.dir.cw90();
        }
        g.px = next;
        visited.insert(g.px);
    }

    fmt::println("1: {}", visited.size());
}

int main()
{
    Map world;
    Guard guard;

    {
        std::string line;
        for (Coord y {0}; std::getline(std::cin, line); ++y)
        {
            if (line.empty())
                break;

            Coord x {0};
            for (auto const& c : line)
            {
                if (c == '.' || c == '#')
                {
                    world.insert({{x, y}, c});
                }
                else
                {
                    guard.px = {x, y};
                    guard.dir = TO_DIR.at(c);
                    world.insert({{x, y}, '.'});
                }
                ++x;
            }
        }
    }

    part1(world, guard);

    return 0;
}
