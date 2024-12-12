#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/view/enumerate.hpp>

#include <iostream>
#include <vector>

using Id = int;
using Disk = std::vector<Id>;

void part1(Disk disk)
{
    Disk::iterator from {disk.begin()};
    Disk::iterator to {std::prev(disk.end())};

    for (;;)
    {
        while (from != disk.end() && *from != -1)
            ++from;
        while (to != disk.begin() && *to == -1)
            --to;
        if (from == disk.end() || to == disk.begin() || std::distance(from, to) <= 0)
            break;
        std::swap(*from, *to);
        ++from;
        --to;
    }

    uint64_t crc {0};
    for (auto [idx, id] : ranges::views::enumerate(disk))
    {
        if (id > 0)
        {
            crc += idx * id;
        }
    }

    fmt::println("1: {}", crc);
}

int main()
{
    Disk disk;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        Id id {0};
        for (auto [idx, c] : ranges::views::enumerate(line))
        {
            Id x {id};
            if ((idx & 1) == 1)
            {
                x = -1;
            }

            for (int i {0}; i < (c - '0'); ++i)
            {
                disk.push_back(x);
            }

            if ((idx & 1) == 0)
            {
                ++id;
            }
        }
    }

    part1(disk);

    return 0;
}
