#include <boost/regex/v5/regex_fwd.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/regex.hpp>

#include <iostream>
#include <numeric>
#include <stdexcept>

static const boost::regex BUTTON_RX{"Button .: X\\+([0-9]+), Y\\+([0-9]+)"};
static const boost::regex PRIZE_RX{"Prize: X=([0-9]+), Y=([0-9]+)"};

struct Game
{
    int64_t dx1{0}, dy1{0};
    int64_t dx2{0}, dy2{0};
    int64_t prize_x{0}, prize_y{0};
};

using Games = std::vector<Game>;


void part1(Games const& games)
{
    unsigned tokens{0};

    for (auto const g : games)
    {
        const int64_t cm = std::lcm(g.dx2, g.dy2);
        const int64_t t1 {cm / g.dx2};
        const int64_t t2 {cm / g.dy2};
        const int64_t X {(g.prize_x * t1 - g.prize_y * t2) / (g.dx1 * t1 - g.dy1 * t2)};
        const int64_t Y {(g.prize_x - g.dx1 * X) / g.dx2};

        // fmt::println("> {}*{} + {}*{} = {} ? {}", X, g.dx1, Y, g.dx2, (X * g.dx1 + Y * g.dx2), g.prize_x);
        // fmt::println("> {}*{} + {}*{} = {} ? {}", X, g.dy1, Y, g.dy2, (X * g.dy1 + Y * g.dy2), g.prize_y);

        if (X <= 100 && Y <= 100 && (X * g.dx1 + Y * g.dx2) == g.prize_x && (X * g.dy1 + Y * g.dy2) == g.prize_y)  {
            tokens += X * 3 + Y;
        }
        // fmt::println("X: {}, Y: {}", X, Y);
    }

    // 37084 TOO HIGH
    fmt::println("1: {}", tokens);
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

        // fmt::println("{},{}; {},{}; {},{}", g.dx1, g.dy1, g.dx2, g.dy2, g.prize_x, g.prize_y);

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

    return 0;
}

#if 0
Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400

94a + 22b = 8400
34a + 67b = 5400

Scitaci:
========
lcm(67,22) = 1474

94a + 22b = 8400        / * 67   t1
34a + 67b = 5400        / *(-22) t2

    6298a = 562800
    -748a = -118800

    5550a = 444000
    a = 80


#endif
