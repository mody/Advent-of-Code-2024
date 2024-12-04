#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using Map = std::unordered_map<Point, unsigned char>;


struct Occurence {
    Point px;
    Gfx_2d::Direction dir;

    friend size_t hash_value(Occurence const& o) noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, o.px);
        boost::hash_combine(seed, o.dir);
        return seed;
    }

    bool operator== (Occurence const& oth) const noexcept { return px == oth.px && dir == oth.dir; }
};

using Occurences = std::unordered_set<Occurence, boost::hash<Occurence>>;

Occurences find_all(Map const& map, std::string const& word)
{
    Occurences result;

    for (auto const& dir :
         {Gfx_2d::Up,
          Gfx_2d::Down,
          Gfx_2d::Left,
          Gfx_2d::Right,
          Gfx_2d::TopLeft,
          Gfx_2d::TopRight,
          Gfx_2d::BottomLeft,
          Gfx_2d::BottomRight})
    {
        for (Coord y{0}; ; ++y) {
            if (!map.contains({0, y})) break;

            for (Coord x{0}; ; ++x) {
                Point px {x, y}, start {x, y};
                if (!map.contains(px)) break;

                unsigned word_idx{0};
                if (map.at(px) != word.at(word_idx))  continue;

                for(;;)
                {
                    ++word_idx;
                    if (word_idx == word.size()) break;

                    px += dir;
                    if (!map.contains(px)) break;

                    if(map.at(px) != word.at(word_idx)) break;
                }

                if (word_idx == word.size()) {
                    result.insert({start, dir});
                }
            }
        }
    }

    return result;
}

void part1(Map const& map)
{
    fmt::println("1: {}", find_all(map, "XMAS").size());
}

void part2(Map const& map)
{
    const auto occurences {find_all(map, "MAS")};

    static const std::unordered_map<Gfx_2d::Direction, std::vector<std::pair<Gfx_2d::Direction, Gfx_2d::Direction>>>
        OPTIONS {
            {Gfx_2d::TopLeft,       {{Gfx_2d::Left, Gfx_2d::TopRight}, {Gfx_2d::Up,    Gfx_2d::BottomLeft}}},
            {Gfx_2d::BottomLeft,    {{Gfx_2d::Left, Gfx_2d::BottomRight}, {Gfx_2d::Down,  Gfx_2d::TopLeft}}},
            {Gfx_2d::BottomRight,   {{Gfx_2d::Down, Gfx_2d::TopRight}, {Gfx_2d::Right, Gfx_2d::BottomLeft}}},
            {Gfx_2d::TopRight,      {{Gfx_2d::Up  , Gfx_2d::BottomRight}, {Gfx_2d::Right, Gfx_2d::TopLeft}}},
            {Gfx_2d::Up, {}},
            {Gfx_2d::Down, {}},
            {Gfx_2d::Left, {}},
            {Gfx_2d::Right, {}},
        };

    unsigned xmas_count {0};

    for (auto const& occurence : occurences) 
    {
        for (auto const& [rel, opposite] : OPTIONS.at(occurence.dir)) {
            Point other{occurence.px};
            other += rel * 2;
            if (occurences.contains({other, opposite}))
            {
                ++xmas_count;
                break;
            }
        }
    }

    fmt::println("2: {}", xmas_count / 2);
}

int main()
{
    Map map;

    {
        Coord y {0};

        std::string line;
        while (std::getline(std::cin, line))
        {
            if (line.empty())
                break;

            Coord x{0};
            for (unsigned char c : line) {
                map.insert({{x, y}, c});
                ++x;
            }
            ++y;
        }
    }

    part1(map);
    part2(map);

    return 0;
}
