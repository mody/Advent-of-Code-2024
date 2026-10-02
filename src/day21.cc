#include <algorithm>
#include <array>
#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using Cost = uint64_t;

struct Point {
    int x;
    int y;

    auto operator<=>(const Point&) const = default;
};

class Keypad {
public:
    explicit Keypad(std::initializer_list<std::pair<const char, Point>> keys)
        : keys_ {keys}
    {
    }

    const std::vector<std::string>& paths(char from, char to) const
    {
        const auto cache_key = std::pair {from, to};
        if (const auto it = paths_.find(cache_key); it != paths_.end())
            return it->second;

        std::queue<std::pair<Point, std::string>> pending;
        std::map<Point, unsigned> distances;
        const Point start = keys_.at(from);
        const Point destination = keys_.at(to);
        pending.push({start, ""});
        distances.emplace(start, 0);

        auto& result = paths_[cache_key];
        unsigned shortest = std::numeric_limits<unsigned>::max();
        constexpr std::array moves {
            std::pair {Point {0, -1}, '^'},
            std::pair {Point {0, 1}, 'v'},
            std::pair {Point {-1, 0}, '<'},
            std::pair {Point {1, 0}, '>'},
        };
        while (!pending.empty()) {
            auto [position, sequence] = std::move(pending.front());
            pending.pop();
            if (sequence.size() > shortest)
                continue;
            if (position == destination) {
                shortest = sequence.size();
                result.push_back(sequence + 'A');
                continue;
            }

            for (const auto& [delta, button] : moves) {
                const Point next {position.x + delta.x, position.y + delta.y};
                if (!valid(next))
                    continue;
                const unsigned next_distance = sequence.size() + 1;
                if (const auto it = distances.find(next); it != distances.end() && it->second < next_distance)
                    continue;
                distances[next] = next_distance;
                pending.push({next, sequence + button});
            }
        }
        return result;
    }

private:
    bool valid(Point point) const
    {
        for (const auto& [button, key_point] : keys_) {
            if (key_point == point)
                return true;
        }
        return false;
    }

    std::map<char, Point> keys_;
    mutable std::map<std::pair<char, char>, std::vector<std::string>> paths_;
};

class Solver {
public:
    Solver()
        : numeric_ {{{'7', {0, 0}}, {'8', {1, 0}}, {'9', {2, 0}},
                     {'4', {0, 1}}, {'5', {1, 1}}, {'6', {2, 1}},
                     {'1', {0, 2}}, {'2', {1, 2}}, {'3', {2, 2}},
                     {'0', {1, 3}}, {'A', {2, 3}}}}
        , directional_ {{{'^', {1, 0}}, {'A', {2, 0}},
                          {'<', {0, 1}}, {'v', {1, 1}}, {'>', {2, 1}}}}
    {
    }

    Cost shortest_code(std::string_view code, unsigned robots)
    {
        Cost total = 0;
        char previous = 'A';
        for (const char button : code) {
            total += transition(false, previous, button, robots);
            previous = button;
        }
        return total;
    }

private:
    struct CacheKey {
        bool directional;
        char from;
        char to;
        unsigned robots;

        auto operator<=>(const CacheKey&) const = default;
    };

    Cost transition(bool directional, char from, char to, unsigned robots)
    {
        const CacheKey key {directional, from, to, robots};
        if (const auto it = cache_.find(key); it != cache_.end())
            return it->second;

        const Keypad& keypad = directional ? directional_ : numeric_;
        Cost best = std::numeric_limits<Cost>::max();
        for (const auto& sequence : keypad.paths(from, to)) {
            Cost cost = 0;
            if (robots == 0) {
                cost = sequence.size();
            } else {
                char previous = 'A';
                for (const char button : sequence) {
                    cost += transition(true, previous, button, robots - 1);
                    previous = button;
                }
            }
            best = std::min(best, cost);
        }
        cache_.emplace(key, best);
        return best;
    }

    Keypad numeric_;
    Keypad directional_;
    std::map<CacheKey, Cost> cache_;
};

int main()
{
    Solver solver;
    Cost part1 = 0;
    Cost part2 = 0;
    std::string code;
    while (std::getline(std::cin, code)) {
        if (code.empty())
            continue;

        const Cost numeric_part = std::stoull(code);
        part1 += solver.shortest_code(code, 2) * numeric_part;
        part2 += solver.shortest_code(code, 25) * numeric_part;
    }

    fmt::println("1: {}", part1);
    fmt::println("2: {}", part2);
}
