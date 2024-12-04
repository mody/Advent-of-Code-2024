#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using Map = std::unordered_map<Point, unsigned char>;

void part1(Map const& map)
{
    unsigned xmas_count{0};

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
        const std::string word{"XMAS"};

        for (Coord y{0}; ; ++y) {
            if (!map.contains({0, y})) break;

            for (Coord x{0}; ; ++x) {
                Point px {x, y};
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
                    ++xmas_count;
                }
            }
        }
    }

    fmt::println("1: {}", xmas_count);
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

    return 0;
}
