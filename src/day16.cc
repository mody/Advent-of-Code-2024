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

using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexDistance, EdgeWeight>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

Graph make_graph(
    World const& w,
    std::unordered_map<Node, Vertex, boost::hash<Node>>& point2vertex,
    std::unordered_map<Vertex, Node, boost::hash<Vertex>>& vertex2point)
{
    Graph g;
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
    return g;
}


void part1(World w)
{
    std::unordered_map<Node, Vertex, boost::hash<Node>> point2vertex;
    std::unordered_map<Vertex, Node, boost::hash<Vertex>> vertex2point;
    Graph g {make_graph(w, point2vertex, vertex2point)};

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

class PathDFSVisitor : public boost::default_dfs_visitor
{
public:
    PathDFSVisitor(std::set<Vertex> destinations, std::vector<std::deque<Vertex>>& dstAllPaths)
        : target {std::move(destinations)}
        , allPaths {dstAllPaths}
    {
    }

    // Called when a vertex is discovered
    void discover_vertex(Vertex v, const Graph& g)
    {
        currentPath.push_back(v);
        if (target.contains(v))
        {
            // Found a path to the target
            allPaths.push_back(currentPath);
        }
    }

    // Called when backtracking
    void finish_vertex(Vertex v, const Graph& g) { currentPath.pop_back(); }

private:
    std::set<Vertex> target;
    std::deque<Vertex> currentPath;
    std::vector<std::deque<Vertex>>& allPaths;
};

void dump(World const& w, Points const& visited)
{
    for (Coord y{0}; y < w.max_y; ++y) {
        for (Coord x{0}; x < w.max_x; ++x) {
            if (w.start == Point{x,y}) {
                fmt::print("S");
            } else if (w.finish == Point{x,y}) {
                fmt::print("E");
            } else if (visited.contains({x,y})) {
                fmt::print("O");
            }
            else if (w.map.contains({x, y}))
            {
                fmt::print(".");
            } else {
                fmt::print("#");
            }
        }
        fmt::println("");
    }
    fmt::println("");
}

void part2(World w)
{
    dump(w, {});

    std::unordered_map<Node, Vertex, boost::hash<Node>> point2vertex;
    std::unordered_map<Vertex, Node, boost::hash<Vertex>> vertex2point;
    Graph g {make_graph(w, point2vertex, vertex2point)};

    const Vertex start_v = point2vertex.at({w.start, Gfx_2d::Left});

    std::set<Vertex> destinations;
    for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right})
    {
        destinations.insert(point2vertex.at({w.finish, dir}));
    }

    std::vector<std::deque<Vertex>> allPaths;
    auto allPathsVisitor {PathDFSVisitor{std::move(destinations), allPaths}};

    std::vector<boost::default_color_type> color_map(num_vertices(g));
    boost::depth_first_visit(
        g, start_v, allPathsVisitor, boost::make_iterator_property_map(color_map.begin(), get(boost::vertex_index, g)));


    Points all_points;
    for (auto path : allPaths)
        for (auto v : path)
            all_points.insert(vertex2point.at(v).px);

    fmt::println("2: {}", all_points.size());
    dump(w, all_points);
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
    part2(w);

    return 0;
}
