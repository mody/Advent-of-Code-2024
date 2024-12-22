#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/config.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <cstdint>
#include <iostream>
#include <unordered_set>

using Coord = uint32_t;
using Point = Gfx_2d::Point<Coord>;
using Points = std::unordered_set<Point, boost::hash<Point>>;

struct World
{
    Points map;
    Coord max_x{0}, max_y{0};
    Point start, finish;
};

struct Node
{
    Point px;
    Gfx_2d::Direction in;

    bool operator== (const Node& oth) const noexcept
    {
        return px == oth.px && in == oth.in;
    }

    friend size_t hash_value(Node const& n) noexcept {
        size_t seed = 0;
        boost::hash_combine(seed, n.px);
        boost::hash_combine(seed, n.in);
        return seed;
    }
};

void part1(World w)
{
    using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
    using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexDistance, EdgeWeight>;
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

    Graph g;

    std::unordered_map<Node, Vertex, boost::hash<Node>> point2vertex;
    for (auto const& px : w.map)
        for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
            point2vertex.insert({{px, dir}, boost::add_vertex(g)});

    for (auto const& [node, v] : point2vertex)
    {
        if (w.map.contains(node.px + node.in)) {
            boost::add_edge(v, point2vertex.at({node.px + node.in, node.in}), EdgeWeight {1}, g);
        }
        if (w.map.contains(node.px + node.in.back())) {
            boost::add_edge(v, point2vertex.at({node.px + node.in.back(), node.in.back()}), EdgeWeight {1}, g);
        }
        if (w.map.contains(node.px + node.in.cw90())) {
            boost::add_edge(v, point2vertex.at({node.px + node.in.cw90(), node.in.cw90()}), EdgeWeight {1001}, g);
        }
        if (w.map.contains(node.px + node.in.ccw90())) {
            boost::add_edge(v, point2vertex.at({node.px + node.in.ccw90(), node.in.ccw90()}), EdgeWeight {1001}, g);
        }
    }

    const Vertex start_v = point2vertex.at({w.start, Gfx_2d::Left});

    auto dist_map = boost::get(boost::vertex_distance, g);
    std::vector<Vertex> pred(boost::num_vertices(g));
    boost::dijkstra_shortest_paths(g, start_v, boost::distance_map(boost::get(boost::vertex_distance, g)));

    unsigned shortes{UINT32_MAX};
    for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
    {
        shortes = std::min(shortes, dist_map[point2vertex.at({w.finish, dir})]);
    }
    fmt::println("1: {}", shortes);
}

int main()
{
    World w;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

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

    part1(w);

    return 0;
}
