#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iostream>
#include <unordered_map>
#include <unordered_set>

using Coord = long;
using Point = Gfx_2d::Point<Coord>;
using PointSet = std::unordered_set<Point, boost::hash<Point>>;

struct World
{
    unsigned size_x {0}, size_y {0};
    std::unordered_map<unsigned char, PointSet> antennas;
};

void dump(World const& world, PointSet const& other)
{
    for (unsigned y {0}; y < world.size_y; ++y)
    {
        for (unsigned x {0}; x < world.size_x; ++x)
        {
            const Point px {x, y};
            bool found{false};
            for (auto const& [c, points] : world.antennas)
            {
                found = points.contains(px);
                if (found)
                {
                    fmt::print("{:c}", c);
                    break;
                } else {
                    found = other.contains(px);
                    if (found)
                    {
                        fmt::print("#");
                        break;
                    }
                }
            }
            if (!found) {
                fmt::print(".");
            }
        }
        fmt::print("\n");
    }
}

void part1(World const& world)
{
    PointSet result;

    for (auto const& [_, points] : world.antennas)
    {
        for (auto const& me : points) 
        {
            for (auto const& px : points)
            {
                if (px == me) continue;
                result.insert(me + (me - px));
                result.insert(px + (px - me));
            }
        }
    }

    bool done {false};
    do
    {
        done = true;
        for (auto const& px : result)
        {
            if (px.x < 0 || px.y < 0 || px.x >= world.size_x || px.y >= world.size_y)
            {
                result.erase(px);
                done = false;
                break;
            }
        }
    } while(!done);

    fmt::println("1: {}", result.size());
}

int main()
{
    World world;

    {
        long y {0};
        std::string line;
        while (std::getline(std::cin, line))
        {
            if (line.empty())
                break;

            long x {0};
            for (auto c : line)
            {
                if (c != '.')
                {
                    world.antennas[c].insert({x, y});
                }
                ++x;
            }
            world.size_x = x;
            ++y;
        }
        world.size_y = y;
    }

    fmt::println("size_x: {}, size_y: {}", world.size_x, world.size_y);

    part1(world);

    return 0;
}
