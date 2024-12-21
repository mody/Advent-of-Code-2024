#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cassert>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

using Coord = uint32_t;
using Point = Gfx_2d::Point<Coord>;
using Points = std::unordered_set<Point, boost::hash<Point>>;
using World = std::unordered_map<Point, char, boost::hash<Point>>;

struct Box
{
    Point px1, px2;

    bool operator == (Box const& oth) const noexcept {
        return std::tie(px1, px2) == std::tie(oth.px1, oth.px2);
    }

    friend size_t hash_value(Box const& b) noexcept {
        size_t seed = 0;
        boost::hash_combine(seed, b.px1);
        boost::hash_combine(seed, b.px2);
        return seed;
    }
};
using Boxes = std::unordered_set<Box, boost::hash<Box>>;

struct Map
{
    World world;
    Coord max_x {0}, max_y {0};
    std::string moves;
    Point start {};

    [[maybe_unused]] void dump(Point const& me) const
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

    [[nodiscard]] bool push_by_one(Point from, const Gfx_2d::Direction dir)
    {
        Point next {from + dir};
        auto [it, inserted] = world.insert({next, world.at(from)});
        if (!inserted)
        {
            if (it->second != '#')
            {
                if (push_by_one(next, dir))
                {
                    std::tie(it, inserted) = world.insert({next, world.at(from)});
                }
            }
        }
        if (inserted)
        {
            world.erase(from);
        }
        return inserted;
    }
};

struct Map1 : public Map
{
    [[nodiscard]] bool push(Point from, const Gfx_2d::Direction dir)
    {
        return push_by_one(from, dir);
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
};

struct Map2 : public Map
{
    [[nodiscard]] std::optional<Box> make_box(Point const& pa) const
    {
        if (world.contains(pa))
        {
            if (world.at(pa) == '[')
            {
                return Box {pa, pa + Gfx_2d::Right};
            }
            else if (world.at(pa) == ']')
            {
                return Box {pa + Gfx_2d::Left, pa};
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] bool push(Point from, const Gfx_2d::Direction dir)
    {
        if (dir == Gfx_2d::Left || dir == Gfx_2d::Right) {
            return push_by_one(from, dir);
        }
        assert(world.at(from) == '[' || world.at(from) == ']');
        return push_boxes({make_box(from).value()}, dir);
    }

    [[nodiscard]] bool push_boxes(Boxes const& from, const Gfx_2d::Direction dir)
    {
        if (from.empty())
            return true;

        Points points;
        for (auto const& b : from)
        {
            points.insert(b.px1 + dir);
            points.insert(b.px2 + dir);
        }

        for (auto const& p : points)
            if (world.contains(p) && world.at(p) == '#')
                return false;

        Boxes boxes;
        for (auto const& p : points)
        {
            const auto bb {make_box(p)};
            if (bb)
                boxes.insert(bb.value());
        }

        const bool can_move {push_boxes(boxes, dir)};
        if (!can_move)
            return false;

        for (auto const& p : points)
        {
            const Point prev {p + dir.back()};
            assert(!world.contains(p));
            assert(world.contains(prev));
            world.insert({p, world.at(prev)});
            world.erase(prev);
        }

        return true;
    }

    [[nodiscard]] uint64_t score2() const 
    {
        uint64_t res{0};
        for (Coord y {0}; y < max_y; ++y)
        {
            for (Coord x {0}; x < max_x; ++x)
            {
                const Point px {x, y};
                if (world.contains(px) && world.at(px) == '[') {
                    res += 100 * y + x;
                }
            }
        }
        return res;
    }
};

void part1(Map1 map1)
{
    static const std::map<char, Gfx_2d::Direction> TO_DIR {
        {'^', Gfx_2d::Up},
        {'v', Gfx_2d::Down},
        {'<', Gfx_2d::Left},
        {'>', Gfx_2d::Right},
    };

    Point me {map1.start};

    for (auto m : map1.moves)
    {
        Point next {me + TO_DIR.at(m)};
        auto it = map1.world.find(next);
        if (it != map1.world.end())
        {
            if (it->second == 'O')
            {
                if (map1.push(next, TO_DIR.at(m)))
                {
                    me = next;
                }
            }
        }
        else
        {
            me = next;
        }
    }
    fmt::println("1: {}", map1.score1());
}

void part2(Map2 map2)
{
    static const std::map<char, Gfx_2d::Direction> TO_DIR {
        {'^', Gfx_2d::Up},
        {'v', Gfx_2d::Down},
        {'<', Gfx_2d::Left},
        {'>', Gfx_2d::Right},
    };

    Point me {map2.start};
    // map2.dump(me);

    for (auto m : map2.moves)
    {
        // fmt::println("Move: {}", m);
        Point next {me + TO_DIR.at(m)};
        auto it = map2.world.find(next);
        if (it != map2.world.end())
        {
            if (it->second == '[' || it->second == ']')
            {
                if (map2.push(next, TO_DIR.at(m)))
                {
                    me = next;
                }
            }
        }
        else
        {
            me = next;
        }
        // map2.dump(me);
    }

    // map2.dump(me);
    fmt::println("2: {}", map2.score2());
}

int main()
{
    Map1 map1;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        map1.max_x = 0;
        for (auto c : line)
        {
            if (c == '@')
            {
                map1.start = {map1.max_x, map1.max_y};
            }
            else if (c != '.')
            {
                map1.world.insert({{map1.max_x, map1.max_y}, c});
            }
            ++map1.max_x;
        }
        ++map1.max_y;
    }

    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;
        map1.moves += std::move(line);
    }

    if (map1.moves.empty())
    {
        std::cerr << "Missing moves!" << std::endl;
    }

    if (map1.start == Point {0, 0})
    {
        std::cerr << "Missing start position!" << std::endl;
    }

    part1(map1);

    Map2 map2;
    for (Coord y {0}; y < map1.max_y; ++y)
    {
        for (Coord x {0}; x < map1.max_x; ++x)
        {
            const Point px {x, y};
            auto it = map1.world.find(px);
            if (it != map1.world.end())
            {
                const Point px2 {x * 2, y};
                if (it->second == 'O')
                {
                    map2.world.insert({px2, '['});
                    map2.world.insert({px2 + Gfx_2d::Right, ']'});
                }
                else if (it->second == '#')
                {
                    map2.world.insert({px2, '#'});
                    map2.world.insert({px2 + Gfx_2d::Right, '#'});
                }
            }
            map2.max_x += 2;
        }
        ++map2.max_y;
    }
    map2.start = Point {map1.start.x * 2, map1.start.y};
    map2.moves = map1.moves;

    part2(map2);

    return 0;
}
