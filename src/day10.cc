#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths_no_color_map.hpp>

#include <iostream>
#include <unordered_map>

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;
using World = std::unordered_map<Point, unsigned, boost::hash<Point>>;
using PointSet = std::unordered_set<Point, boost::hash<Point>>;

void part1(World const& world, const unsigned max_x, const unsigned max_y)
{
    using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
    using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexDistance, EdgeWeight>;
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

    Graph g;
    std::unordered_map<Point, Vertex, boost::hash<Point>> point2vertex;

    for (Coord y {0}; y < max_y; ++y)
    {
        for (Coord x {0}; x < max_x; ++x)
        {
            point2vertex.insert({{x, y}, boost::add_vertex(g)});
        }
    }

    for (auto const& [px, myHeight] : world)
    {
        const Vertex from = point2vertex.at(px);
        for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
        {
            const auto dst {px + dir};
            auto i2 = world.find(dst);
            if (i2 != world.end() && i2->second == (myHeight + 1))
            {
                boost::add_edge(from, point2vertex.at(dst), EdgeWeight {1}, g);
            }
        }
    }

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


    unsigned score {0};

    for (auto const& from : zeros)
    {
        const Vertex start_v = point2vertex.at(from);
        auto dist_map = boost::get(boost::vertex_distance, g);
        std::vector<Vertex> pred(boost::num_vertices(g));
        boost::dijkstra_shortest_paths_no_color_map(
            g, start_v, boost::distance_map(dist_map).predecessor_map(&pred[0]));

        for (auto const& dst : nines)
        {
            const Vertex dst_v = point2vertex.at(dst);
            if (dist_map[dst_v] == 9)
            {
                ++score;
            }
        }

    }

    fmt::println("1: {}", score);
}

int main()
{
    World world;
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

    part1(world, max_x, max_y);

    return 0;
}
