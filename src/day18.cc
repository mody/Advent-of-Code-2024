#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/view/take.hpp>
#include <range/v3/view/drop.hpp>

#include <boost/container_hash/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <iostream>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;
using PointMap = std::unordered_map<Point, unsigned, boost::hash<Point>>;
using PointSet = std::unordered_set<Point, boost::hash<Point>>;
using Points = std::vector<Point>;

using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexDistance, EdgeWeight>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

static constexpr Coord SIZE {71};

void dump(Points const& falling)
{
    PointSet ps;
    for (auto px : falling)
        ps.insert(std::move(px));

    for (Coord y {0}; y < SIZE; ++y)
    {
        for (Coord x {0}; x < SIZE; ++x)
        {
            const Point px {x, y};
            if (ps.contains(px))
            {
                fmt::print("#");
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


void part1(Points falling)
{
    falling.resize(1024);

    PointSet ps;
    for (auto px : falling)
        ps.insert(std::move(px));

    Graph g;

    std::unordered_map<Point, Vertex, boost::hash<Point>> point2vertex;
    for (Coord y {0}; y < SIZE; ++y)
    {
        for (Coord x {0}; x < SIZE; ++x)
        {
            const Point px {x, y};
            if (!ps.contains(px))
            {
                point2vertex.insert({px, boost::add_vertex(g)});
            }
        }
    }

    for (auto const& [px, v] : point2vertex) {
        for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
        {
            if (auto it {point2vertex.find(px + dir)}; it != point2vertex.end())
            {
                boost::add_edge(v, it->second, EdgeWeight {1}, g);
            }
        }
    }

    const Vertex start_v = point2vertex.at({0, 0});

    auto dist_map = boost::get(boost::vertex_distance, g);
    std::vector<Vertex> pred(boost::num_vertices(g));
    boost::dijkstra_shortest_paths(g, start_v, boost::distance_map(dist_map).predecessor_map(&pred[0]));

    fmt::println("1: {}", dist_map[point2vertex.at({SIZE - 1, SIZE - 1})]);
}

void part2(Points falling)
{
    PointSet ps;
    for (auto px : falling | ranges::views::take(1024))
        ps.insert(std::move(px));

    Graph g;

    std::unordered_map<Point, Vertex, boost::hash<Point>> point2vertex;
    for (Coord y {0}; y < SIZE; ++y)
    {
        for (Coord x {0}; x < SIZE; ++x)
        {
            const Point px {x, y};
            if (!ps.contains(px))
            {
                point2vertex.insert({px, boost::add_vertex(g)});
            }
        }
    }

    for (auto const& [px, v] : point2vertex) {
        for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
        {
            if (auto it {point2vertex.find(px + dir)}; it != point2vertex.end())
            {
                boost::add_edge(v, it->second, EdgeWeight {1}, g);
            }
        }
    }

    for (auto px : falling | ranges::views::drop(1024))
    {
        boost::clear_vertex(point2vertex.at(px), g);
        point2vertex.erase(px);

        const Vertex start_v = point2vertex.at({0, 0});

        auto dist_map = boost::get(boost::vertex_distance, g);
        std::vector<Vertex> pred(boost::num_vertices(g));
        boost::dijkstra_shortest_paths(g, start_v, boost::distance_map(dist_map).predecessor_map(&pred[0]));

        if (dist_map[point2vertex.at({SIZE - 1, SIZE - 1})] == std::numeric_limits<unsigned>::max()) {
            fmt::println("2: {},{}", px.x, px.y);
            break;
        }
    }
}

int main()
{
    Points falling;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        auto comma = line.find(',');
        assert(comma != std::string::npos);

        falling.push_back(Point(std::stoul(line.substr(0, comma)), std::stoul(line.substr(comma + 1))));
    }

    part1(falling);
    part2(falling);

    return 0;
}
