#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

constexpr int64_t FIELD_MAX_X {101};
constexpr int64_t FIELD_MAX_Y {103};
constexpr int64_t HALF_X {(FIELD_MAX_X / 2 + 1)};
constexpr int64_t HALF_Y {(FIELD_MAX_Y / 2 + 1)};

using Coord = int64_t;
using Point = Gfx_2d::Point<Coord>;

struct Robot
{
    Point px {0, 0};
    Gfx_2d::Direction dir;

    friend size_t hash_value(Robot const& r) noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, r.px);
        boost::hash_combine(seed, r.dir);
        return seed;
    }
};

using Robots = std::vector<Robot>;

void dump(Robots const& robots)
{
    std::unordered_map<Point, unsigned, boost::hash<Point>> points;
    for (auto const& r : robots)
    {
        points.insert({r.px, 0}).first->second++;
    }

    for (unsigned y {0}; y < FIELD_MAX_Y; ++y)
    {
        for (unsigned x {0}; x < FIELD_MAX_X; ++x)
        {
            if (points.contains({x, y}))
            {
                fmt::print("{}", points.at({x,y}));
            }
            else
            {
                fmt::print(".");
            }
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

int64_t score(Robots const& robots)
{
    std::map<unsigned, unsigned> sectors;

    for (auto const& r : robots)
    {
        const auto xx = r.px.x / HALF_X;
        const auto yy = r.px.y / HALF_Y;

        if (r.px.x == HALF_X - 1 || r.px.y == HALF_Y - 1)
        {
            // pass
        }
        else
        {
            sectors.insert({yy * 10 + xx, 0}).first->second++;
        }
    }

    assert(sectors.size() == 4);

    int64_t score{1};
    for (auto const& [_, v] : sectors) {
        score *= v;
    }

    return score;
}

void part1(Robots robots)
{
    for (unsigned i {0}; i < 100; ++i)
    {
        for (auto& r : robots)
        {
            r.px += r.dir;
            while (r.px.x < 0)
                r.px.x += FIELD_MAX_X;
            while (r.px.y < 0)
                r.px.y += FIELD_MAX_Y;
            r.px.x %= FIELD_MAX_X;
            r.px.y %= FIELD_MAX_Y;
        }
    }
    fmt::println("1: {}", score(robots));
}

void part2(Robots robots)
{
    for (unsigned i {1}; i < FIELD_MAX_X * FIELD_MAX_Y; ++i)
    {
        for (auto& r : robots)
        {
            r.px += r.dir;
            while (r.px.x < 0)
                r.px.x += FIELD_MAX_X;
            while (r.px.y < 0)
                r.px.y += FIELD_MAX_Y;
            r.px.x %= FIELD_MAX_X;
            r.px.y %= FIELD_MAX_Y;
        }

        std::unordered_map<Point, unsigned, boost::hash<Point>> points;
        for (auto const& r : robots)
        {
            if (r.px.y > HALF_Y)
                points.insert({r.px, 0}).first->second++;
        }

        for (unsigned y {HALF_Y}; y < FIELD_MAX_Y; ++y)
        {
            bool has_line{false};
            Point from;
            for (unsigned x {0}; x < FIELD_MAX_X; ++x)
            {
                Point to{x,y};
                if (points.contains(to))
                {
                    if (!has_line) {
                        from = to;
                        has_line = true;
                    }
                }
                else if (has_line)
                {
                    if ((to.x - from.x) > 10) {
                        dump(robots);
                        fmt::println("2: {}", i);
                        return;
                    }
                    has_line = false;
                }
            }
        }
    }
}

int main()
{
    Robots robots;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" =,"), boost::token_compress_on);

        Robot r;
        r.px.x = std::stoul(parts.at(1));
        r.px.y = std::stoul(parts.at(2));
        r.dir.dx = std::stol(parts.at(4));
        r.dir.dy = std::stol(parts.at(5));

        robots.push_back(std::move(r));
    }

    part1(robots);
    part2(robots);

    return 0;
}
