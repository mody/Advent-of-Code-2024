#include "point2d.h"

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using PointMap = std::unordered_map<Point, char, boost::hash<Point>>;
using PointSet = std::unordered_set<Point, boost::hash<Point>>;
using PointVec = std::vector<Point>;

using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexDistance, EdgeWeight>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;


[[maybe_unused]] void dump(PointMap const& mapa, Coord max_x, Coord max_y)
{
    for (Coord y {0}; y < max_y; ++y) {
        for (Coord x {0}; x < max_x; ++x) {
            auto c {mapa.at({x, y})};
            fmt::print("{}", c);
        }
        fmt::println("");
    }
    fmt::println("");
}

void process(PointMap mapa, Coord max_x, Coord max_y, const Point start, const Point finish)
{
    std::unordered_map<Point, Vertex, boost::hash<Point>> point2vertex;
    std::unordered_map<Vertex, Point, boost::hash<Vertex>> vertex2point;

    Graph g;

    for (auto const& [px, c] : mapa) {
        if (c == '.') {
            auto v {boost::add_vertex(g)};
            point2vertex.insert({px, v});
            vertex2point.insert({v, px});
        }
    }

    for (auto const& [px, v] : point2vertex) {
        for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right}) {
            const Point dst {px + dir};
            if (auto it = point2vertex.find(dst); it != point2vertex.end()) {
                boost::add_edge(v, it->second, EdgeWeight {1}, g);
            }
        }
    }

    const Vertex start_v = point2vertex.at(start);
    const Vertex finish_v = point2vertex.at(finish);

    auto dist_map = boost::get(boost::vertex_distance, g);
    std::vector<Vertex> parent(boost::num_vertices(g));
    boost::dijkstra_shortest_paths(
        g, start_v, boost::distance_map(boost::get(boost::vertex_distance, g)).predecessor_map(&parent[0]));

    PointSet path;
    for (Vertex v {finish_v}; v != start_v; v = parent.at(v)) {
        path.insert(vertex2point.at(v));
    }
    path.insert(start);

    unsigned saves1_100 {0};
    for (auto const& me : path) {
        const auto current_time {dist_map[point2vertex.at(me)]};

        for (auto const& [dst, c] : mapa) {
            if (c != '.' || me.manhattan_dist(dst) < 2 || me.manhattan_dist(dst) > 2)
                continue;
            const auto dst_time {dist_map[point2vertex.at(dst)]};
            if (current_time >= dst_time)
                continue;
            const unsigned saves {dst_time - current_time - 2};
            if (saves >= 100)
                saves1_100++;
        }
    }

    std::unordered_map<std::pair<Point, Point>, unsigned, boost::hash<std::pair<Point, Point>>> saves2_100;
    for (auto const& me : path) {
        const auto current_time {dist_map[point2vertex.at(me)]};

        for (auto const& [dst, c] : mapa) {
            const unsigned dist = me.manhattan_dist(dst);
            if (c != '.' || dist < 2 || dist > 20)
                continue;
            const auto dst_time {dist_map[point2vertex.at(dst)]};
            if ((current_time + dist) >= dst_time)
                continue;
            const unsigned saves {dst_time - current_time - dist};
            if (saves >= 100)
                saves2_100.insert({{me, dst}, saves});
        }
    }

    fmt::println("1: {}", saves1_100);
    fmt::println("2: {}", saves2_100.size());
}

int main()
{
    PointMap mapa;
    Point start, finish;

    Coord max_y {0};
    Coord max_x {0};
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;

        max_x = 0;
        for (auto c : line) {
            if (c == 'S') {
                start = {max_x, max_y};
                c = '.';
            } else if (c == 'E') {
                finish = {max_x, max_y};
                c = '.';
            }
            if (c != '#') {
                mapa.insert({{max_x, max_y}, c});
            }
            ++max_x;
        }
        ++max_y;
    }

    process(mapa, max_x, max_y, start, finish);

    return 0;
}
