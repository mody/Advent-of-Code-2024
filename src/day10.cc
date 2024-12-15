#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <deque>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;
using PointMap = std::unordered_map<Point, unsigned, boost::hash<Point>>;
using PointSet = std::unordered_set<Point, boost::hash<Point>>;

struct State
{
    Point px;
    Gfx_2d::Direction dir;
};

void process(PointMap const& world, const unsigned max_x, const unsigned max_y)
{
    PointSet zeros, nines;
    for (auto const& [px, myHeight] : world)
    {
        switch (myHeight)
        {
        case 0: zeros.insert(px); break;
        case 9: nines.insert(px); break;
        default: break;
        }
    }

    unsigned count1{0};
    unsigned count2{0};

    for (auto zero : zeros)
    {
        PointSet ninesHit;
        PointMap ninesPaths; 

        std::deque<Point> work;
        work.push_back(zero);

        while (!work.empty())
        {
            Point px{work.front()};
            work.pop_front();

            if (world.at(px) == 9)
            {
                ninesHit.insert(px);
                ninesPaths.insert({px, 0}).first->second++;
                continue;
            }

            for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
            {
                const auto dst {px + dir};
                if (auto it {world.find(dst)}; it != world.end())
                {
                    if (it->second == (world.at(px) + 1)) {
                        work.push_front(dst);
                    }
                }
            }
        }

        count1 += ninesHit.size();
        for (auto const& [nine, cnt] : ninesPaths) {
            count2 += cnt;
        }
    }

    fmt::println("1: {}\n2: {}", count1, count2);
}

int main()
{
    PointMap world;
    unsigned max_x {0}, max_y {0};

    {
        Coord y {0};
        std::string line;
        while (std::getline(std::cin, line))
        {
            if (line.empty())
                break;

            Coord x {0};
            for (unsigned char c : line)
            {
                world.insert({{x, y}, (unsigned)(c - '0')});
                ++x;
            }
            max_x = x;
            ++y;
        }
        max_y = y;
    }

    process(world, max_x, max_y);

    return 0;
}
