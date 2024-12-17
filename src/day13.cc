#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/regex.hpp>

#include <iostream>
#include <numeric>
#include <stdexcept>

static const boost::regex BUTTON_RX {"Button .: X\\+([0-9]+), Y\\+([0-9]+)"};
static const boost::regex PRIZE_RX {"Prize: X=([0-9]+), Y=([0-9]+)"};

struct Game
{
    int64_t dx1 {0}, dy1 {0};
    int64_t dx2 {0}, dy2 {0};
    int64_t prize_x {0}, prize_y {0};
};

using Games = std::vector<Game>;


void part1(Games const& games)
{
    unsigned tokens {0};

    for (auto const g : games)
    {
        const int64_t cm = std::lcm(g.dx2, g.dy2);
        const int64_t t1 {cm / g.dx2};
        const int64_t t2 {cm / g.dy2};
        const int64_t X {(g.prize_x * t1 - g.prize_y * t2) / (g.dx1 * t1 - g.dy1 * t2)};
        const int64_t Y {(g.prize_x - g.dx1 * X) / g.dx2};

        if (X <= 100 && Y <= 100 && (X * g.dx1 + Y * g.dx2) == g.prize_x && (X * g.dy1 + Y * g.dy2) == g.prize_y)
        {
            tokens += X * 3 + Y;
        }
    }

    fmt::println("1: {}", tokens);
}


void part2(Games const& games)
{
    int64_t tokens {0};

    for (auto g : games)
    {
        g.prize_x += 10000000000000;
        g.prize_y += 10000000000000;

        const int64_t cm = std::lcm(g.dx2, g.dy2);
        const int64_t t1 {cm / g.dx2};
        const int64_t t2 {cm / g.dy2};
        const int64_t A {(g.prize_x * t1 - g.prize_y * t2) / (g.dx1 * t1 - g.dy1 * t2)};
        const int64_t B {(g.prize_x - g.dx1 * A) / g.dx2};

        if ((A * g.dx1 + B * g.dx2) == g.prize_x && (A * g.dy1 + B * g.dy2) == g.prize_y)
        {
            tokens += A * 3 + B;
        }
    }

    fmt::println("2: {}", tokens);
}


int main()
{
    Games games;

    for (;;)
    {
        Game g;

        auto parse_two = [](const boost::regex rx)
        {
            boost::smatch res;
            std::string line;
            std::getline(std::cin, line);
            if (!boost::regex_search(line, res, rx))
            {
                throw std::runtime_error("RX failed!");
            }
            return std::make_pair(std::stoul(res.str(1)), std::stoul(res.str(2)));
        };

        std::tie(g.dx1, g.dy1) = parse_two(BUTTON_RX);
        std::tie(g.dx2, g.dy2) = parse_two(BUTTON_RX);
        std::tie(g.prize_x, g.prize_y) = parse_two(PRIZE_RX);

        games.push_back(std::move(g));

        {
            std::string line;
            if (!std::getline(std::cin, line))
            {
                break;
            }
            assert(line.empty());
        }
    }

    part1(games);
    part2(games);

    return 0;
}
