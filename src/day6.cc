#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>

using Coord = int32_t;
using Point = Gfx_2d::Point<Coord>;

using Map = std::unordered_map<Point, char, boost::hash<Point>>;
using Visited = std::unordered_map<Point, std::unordered_set<Gfx_2d::Direction, boost::hash<Gfx_2d::Direction>>, boost::hash<Point>>;

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

std::pair<Visited, bool> walk_guard(Map const& world, Guard g)
{
    Visited visited;
    bool finished{true};

    for (; world.contains(g.px);)
    {
        auto [vit, inserted] = visited.insert({g.px, {g.dir}});
        if (!inserted)
        {
            if (!vit->second.insert(g.dir).second)
            {
                finished = false;
                break;
            }
        }

        const Point next {g.px + g.dir};
        const auto it {world.find(next)};
        if (it == world.end())
        {
            break;
        }
        if (it->second == '#')
        {
            g.dir = g.dir.cw90();
        }
        else
        {
            g.px = next;
        }
    }

    return std::make_pair(visited, finished);
}

void part1(Map const& world, Guard g)
{
    auto const& [visited, finished] = walk_guard(world, g);
    fmt::println("1: {}", visited.size());
}

void part2(Map world, Guard const& g)
{
    const Visited visited {walk_guard(world, g).first};

    std::unordered_set<Point, boost::hash<Point>> walls;

    for (auto const& [px, dirs] : visited)
    {
        for (auto const& dir : dirs)
        {
            if (dirs.contains(dir.back())) continue; // tam i zpet, nemuzu zastavet

            const auto pnext {px + dir};
            if (auto wit {world.find(pnext)}; wit != world.end() && wit->second == '.')
            {
                const auto side_dir {dir.cw90()};
                bool wall {false};
                for (auto side_px {px + side_dir}; world.contains(side_px); side_px += side_dir)
                {
                    if (world.at(side_px) == '#')
                    {
                        wall = true;
                        break;
                    }
                }
                if (wall)
                {
                    wit->second = '#';
                    if (!walk_guard(world, g).second)
                    {
                        walls.insert(pnext);
                    }
                    wit->second = '.';
                }
            }
        }
    }

    walls.erase(g.px);

    fmt::println("2: {}", walls.size());
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
    part2(world, guard);

    return 0;
}
