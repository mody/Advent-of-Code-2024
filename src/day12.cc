#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/view/zip.hpp>

#include <deque>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;

struct Tile
{
    char plantType {0};
    unsigned fieldId {0};
};

using Map = std::unordered_map<Point, Tile>;
using Area = std::map<unsigned, unsigned>;
using Perimeter = std::map<unsigned, unsigned>;

std::pair<Map, Area> classify(Map map, Coord const max_x, Coord const max_y)
{
    unsigned fieldId {0};
    Area area;

    for (;;)
    {
        Point start;
        bool found {false};
        for (Coord y {0}; !found && y < max_y; ++y)
        {
            for (Coord x {0}; !found && x < max_x; ++x)
            {
                if (map.at({x, y}).fieldId == 0)
                {
                    start = {x, y};
                    found = true;
                }
            }
        }

        if (!found)
        {
            // no more unclassified fields
            break;
        }

        const char plantType {map.at(start).plantType};
        ++fieldId;

        std::deque<Point> work;
        std::unordered_set<Point> seen;

        work.push_back(start);
        while (!work.empty())
        {
            Point px {work.front()};
            work.pop_front();

            Tile& tile {map.at(px)};

            assert(tile.plantType == plantType);
            assert(tile.fieldId == 0);

            tile.fieldId = fieldId;
            area.insert({fieldId, 0}).first->second++;

            for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
            {
                auto p2 {px + dir};
                if (auto it = map.find(p2); it != map.end())
                {
                    auto const& t2 = it->second;
                    if (t2.fieldId == 0 && t2.plantType == plantType && !seen.contains(p2))
                    {
                        seen.insert(p2);
                        work.push_back(std::move(p2));
                    }
                }
            }
        }
    }

    return {map, area};
}

uint64_t calculateScore(Area const& area, Perimeter const& perimeter)
{
    assert(area.size() == perimeter.size());

    uint64_t score {0};
    for (auto const& [a, p] : ranges::views::zip(area, perimeter))
    {
        score += a.second * p.second;
    }
    return score;
}

Perimeter part1(Map const& map, Coord const max_x, Coord const max_y)
{
    Perimeter perimeter;
    for (Coord y {0}; y < max_y; ++y)
    {
        for (Coord x {0}; x < max_x; ++x)
        {
            const Point px {x, y};
            const Tile& tile {map.at(px)};

            for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
            {
                if (auto it = map.find(px + dir); it != map.end())
                {
                    auto const& t2 = it->second;
                    if (tile.fieldId != t2.fieldId)
                    {
                        perimeter.insert({tile.fieldId, 0}).first->second++;
                    }
                }
                else
                {
                    perimeter.insert({tile.fieldId, 0}).first->second++;
                }
            }
        }
    }

    return perimeter;
}

Perimeter part2(Map const& map, Area const& area, Coord const max_x, Coord const max_y)
{
    Perimeter perimeter;

    auto count_sides = [&map](unsigned id, Point start, Gfx_2d::Direction moving, Gfx_2d::Direction testing) -> unsigned
    {
        unsigned sides{0};
        for (; map.contains(start); start += testing.back())
        {
            bool found {false};
            for (Point px {start}; map.contains(px); px += moving)
            {
                const Tile& tile {map.at(px)};
                if (tile.fieldId != id)
                {
                    found = false;
                    continue;
                }

                const Point pt {px + testing};
                if (!map.contains(pt) || map.at(pt).fieldId != tile.fieldId)
                {
                    if (!found)
                    {
                        ++sides;
                    }
                    found = true;
                }
                else
                {
                    found = false;
                }
            }
        }
        return sides;
    };

    for (auto const& [id, _] : area)
    {
        auto& sides = perimeter.insert({id, 0}).first->second;
        sides += count_sides(id, {0, 0}, Gfx_2d::Right, Gfx_2d::Up);
        sides += count_sides(id, {0, 0}, Gfx_2d::Down, Gfx_2d::Left);
        sides += count_sides(id, {max_x - 1, max_y - 1}, Gfx_2d::Left, Gfx_2d::Down);
        sides += count_sides(id, {max_x - 1, max_y - 1}, Gfx_2d::Up, Gfx_2d::Right);
    }

    return perimeter;
}


int main()
{
    Map map;

    Coord max_x {0}, max_y {0};
    {
        std::string line;
        while (std::getline(std::cin, line))
        {
            if (line.empty())
                break;

            max_x = 0;
            for (auto c : line)
            {
                Tile t {.plantType = c};
                map.insert({{max_x, max_y}, std::move(t)});
                ++max_x;
            }
            ++max_y;
        }
    }

    Area area;
    std::tie(map, area) = classify(map, max_x, max_y);

    fmt::println("1: {}", calculateScore(area, part1(map, max_x, max_y)));
    fmt::println("2: {}", calculateScore(area, part2(map, area, max_x, max_y)));

    return 0;
}
