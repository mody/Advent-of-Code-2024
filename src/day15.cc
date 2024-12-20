#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using Coord = uint32_t;
using Point = Gfx_2d::Point<Coord>;
using World = std::unordered_map<Point, char, boost::hash<Point>>;

struct Map
{
    World world;
    Coord max_x {0}, max_y {0};
    std::string moves;
    Point start {};

    [[nodiscard]] bool push(Point from, const Gfx_2d::Direction dir)
    {
        assert(world.at(from) == 'O');

        Point next {from + dir};
        auto [it, inserted] = world.insert({next, 'O'});
        if (!inserted)
        {
            if (it->second == 'O')
            {
                if (push(next, dir))
                {
                    std::tie(it, inserted) = world.insert({next, 'O'});
                }
            }
        }
        if (inserted)
        {
            world.erase(from);
        }
        return inserted;
    }

    [[nodiscard]] uint64_t score1() const 
    {
        uint64_t res{0};
        for (Coord y {0}; y < max_y; ++y)
        {
            for (Coord x {0}; x < max_x; ++x)
            {
                const Point px {x, y};
                if (world.contains(px) && world.at(px) == 'O') {
                    res += 100 * y + x;
                }
            }
        }
        return res;
    }

    void dump(Point const& me) const
    {
        for (Coord y {0}; y < max_y; ++y)
        {
            for (Coord x {0}; x < max_x; ++x)
            {
                const Point px {x, y};
                if (px == me)
                {
                    fmt::print("@");
                }
                else if (world.contains(px))
                {
                    fmt::print("{}", world.at(px));
                }
                else
                {
                    fmt::print(" ");
                }
            }
            fmt::println("");
        }
        fmt::println("");
    }
};

void part1(Map map)
{
    static const std::map<char, Gfx_2d::Direction> TO_DIR {
        {'^', Gfx_2d::Up},
        {'v', Gfx_2d::Down},
        {'<', Gfx_2d::Left},
        {'>', Gfx_2d::Right},
    };

    Point me {map.start};
    // map.dump(me);

    for (auto m : map.moves)
    {
        // fmt::println("Move: {}", m);
        Point next {me + TO_DIR.at(m)};
        auto it = map.world.find(next);
        if (it != map.world.end())
        {
            if (it->second == 'O')
            {
                if (map.push(next, TO_DIR.at(m)))
                {
                    me = next;
                }
            }
        }
        else
        {
            me = next;
        }
        // map.dump(me);
    }

    // map.dump(me);
    fmt::println("1: {}", map.score1());
}

int main()
{
    Map map;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        map.max_x = 0;
        for (auto c : line)
        {
            if (c == '@')
            {
                map.start = {map.max_x, map.max_y};
            }
            else if (c != '.')
            {
                map.world.insert({{map.max_x, map.max_y}, c});
            }
            ++map.max_x;
        }
        ++map.max_y;
    }

    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;
        map.moves += std::move(line);
    }

    if (map.moves.empty())
    {
        std::cerr << "Missing moves!" << std::endl;
    }

    if (map.start == Point {0, 0})
    {
        std::cerr << "Missing start position!" << std::endl;
    }

    part1(map);

    return 0;
}

#if 0
##########
#.O.O.OOO#
#........#
#OO......#
#OO@.....#
#O#.....O#
#O.....OO#
#O.....OO#
#OO....OO#
##########
#endif
