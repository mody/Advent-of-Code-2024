#include "point2d.h"

#include <atomic>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/config.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <cstdint>
#include <deque>
#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>

using Coord = uint32_t;
using Point = Gfx_2d::Point<Coord>;
using Points = std::unordered_set<Point, boost::hash<Point>>;

using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexDistance, EdgeWeight>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;


struct World
{
    Points map;
    Coord max_x {0}, max_y {0};
    Point start, finish;
};

struct Node
{
    Point px;
    Gfx_2d::Direction in;

    bool operator==(const Node& oth) const noexcept { return px == oth.px && in == oth.in; }

    friend size_t hash_value(Node const& n) noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, n.px);
        boost::hash_combine(seed, n.in);
        return seed;
    }
};


[[maybe_unused]] void dump(World const& w, Points const& visited)
{
    for (Coord y {0}; y < w.max_y; ++y)
    {
        for (Coord x {0}; x < w.max_x; ++x)
        {
            if (w.start == Point {x, y})
            {
                fmt::print("S");
            }
            else if (w.finish == Point {x, y})
            {
                fmt::print("E");
            }
            else if (visited.contains({x, y}))
            {
                fmt::print("O");
            }
            else if (w.map.contains({x, y}))
            {
                fmt::print(".");
            }
            else
            {
                fmt::print("#");
            }
        }
        fmt::println("");
    }
    fmt::println("");
}

struct FullGraph
{
    std::unordered_map<Node, Vertex, boost::hash<Node>> point2vertex;
    std::unordered_map<Vertex, Node, boost::hash<Vertex>> vertex2point;

    Graph g;

    FullGraph(World const& w)
    {
        for (auto const& px : w.map)
        {
            for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
            {
                auto v {boost::add_vertex(g)};
                point2vertex.insert({{px, dir}, v});
                vertex2point.insert({v, {px, dir}});
            }
        }

        for (auto const& [node, v] : point2vertex)
        {
            if (w.map.contains(node.px + node.in))
            {
                boost::add_edge(v, point2vertex.at({node.px + node.in, node.in}), EdgeWeight {1}, g);
            }
            if (w.map.contains(node.px + node.in.back()))
            {
                boost::add_edge(v, point2vertex.at({node.px + node.in.back(), node.in.back()}), EdgeWeight {1}, g);
            }
            if (w.map.contains(node.px + node.in.cw90()))
            {
                boost::add_edge(v, point2vertex.at({node.px + node.in.cw90(), node.in.cw90()}), EdgeWeight {1001}, g);
            }
            if (w.map.contains(node.px + node.in.ccw90()))
            {
                boost::add_edge(v, point2vertex.at({node.px + node.in.ccw90(), node.in.ccw90()}), EdgeWeight {1001}, g);
            }
        }
    }

    [[nodiscard]] unsigned shortest(World const& w, Node const& n)
    {
        const Vertex start_v = point2vertex.at(n);

        auto dist_map = boost::get(boost::vertex_distance, g);
        std::vector<Vertex> pred(boost::num_vertices(g));
        boost::dijkstra_shortest_paths(g, start_v, boost::distance_map(boost::get(boost::vertex_distance, g)));

        unsigned shortest {UINT32_MAX};
        for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
        {
            shortest = std::min(shortest, dist_map[point2vertex.at({w.finish, dir})]);
        }
        return shortest;
    }
};

[[nodiscard]] unsigned part1(World w)
{
    FullGraph fg(w);

    return fg.shortest(w, {w.start, Gfx_2d::Right});
}


struct Path
{
    std::deque<Node> path;
    unsigned score {0};
};

void part2(World w, unsigned const bestScore)
{
    std::vector<Path> result;

    FullGraph fg(w);

    std::deque<Path> q;
    q.push_back(Path {{{w.start, Gfx_2d::Right}}, 0});

    while (!q.empty())
    {
        Path currentPath {q.front()};
        q.pop_front();

        if (currentPath.score > bestScore)
        {
            continue;
        }

        Node& currentPx {currentPath.path.back()};
        if (currentPx.px == w.finish)
        {
            result.push_back(std::move(currentPath));
            continue;
        }

        if ((currentPath.score + fg.shortest(w, currentPx)) > bestScore)
        {
            continue;
        }

        for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
        {
            if (currentPx.in.back() == dir)
            {
                continue;  // don't go back
            }

            Path nextPath {currentPath};
            Node nextPx {currentPx};
            nextPx.px += dir;
            nextPx.in = dir;
            if (!w.map.contains(nextPx.px))
            {
                continue;
            }
            nextPath.path.push_back(nextPx);
            nextPath.score += (currentPx.in == dir) ? 1 : 1001;
            q.push_back(std::move(nextPath));
        }
    }

    Points visited;
    for (auto const& path : result)
    {
        for (auto const& p : path.path) {
            visited.insert(p.px);
        }
    }

    fmt::println("2: {}", visited.size());
}


int main()
{
    World w;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        w.max_x = 0;
        for (auto c : line)
        {
            if (c == 'S')
            {
                w.start = {w.max_x, w.max_y};
            }
            else if (c == 'E')
            {
                w.finish = {w.max_x, w.max_y};
            }
            if (c != '#')
            {
                w.map.insert({w.max_x, w.max_y});
            }
            ++w.max_x;
        }
        ++w.max_y;
    }

    auto shortest {part1(w)};
    fmt::println("1: {}", shortest);
    part2(w, shortest);

    return 0;
}
