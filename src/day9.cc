#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/view/enumerate.hpp>

#include <iostream>
#include <vector>

using Id = int;
using Disk1 = std::vector<Id>;

struct Item
{
    Id id {-1};
    unsigned len {0};
};
using Disk2 = std::vector<Item>;

void part1(Disk1 disk1)
{
    Disk1::iterator from {disk1.begin()};
    Disk1::iterator to {std::prev(disk1.end())};

    for (;;)
    {
        while (from != disk1.end() && *from != -1)
            ++from;
        while (to != disk1.begin() && *to == -1)
            --to;
        if (from == disk1.end() || to == disk1.begin() || std::distance(from, to) <= 0)
            break;
        std::swap(*from, *to);
        ++from;
        --to;
    }

    uint64_t crc {0};
    for (auto [idx, id] : ranges::views::enumerate(disk1))
    {
        if (id > 0)
        {
            crc += idx * id;
        }
    }

    fmt::println("1: {}", crc);
}

void part2(Disk2 disk2)
{
    Disk2::iterator from {std::prev(disk2.end())};

    Id actual_id{from->id};
    assert(actual_id != -1);

    for (; from != disk2.begin(); --from)
    {
        if (from->id != actual_id)
            continue;

        for (auto to {disk2.begin()}; to != disk2.end(); ++to)
        {
            if (to->id == actual_id)
                break;  // too far

            if (to->id == -1 && to->id != from->id)
            {
                if (to->len == from->len)
                {
                    // exact match
                    std::swap(*to, *from);
                    break;
                }
                else if (to->len > from->len)
                {
                    const unsigned diff {to->len - from->len};
                    std::swap(*to, *from);
                    from->len = to->len;
                    disk2.insert(std::next(to), {-1, diff}); // possible iterator invalidation
                    from = disk2.end();
                    break;
                }
            }
        }
        if (actual_id == 0)
        {
            break;
        }
        --actual_id;
    }

    uint64_t crc {0};
    unsigned idx{0};
    for (auto const& item : disk2)
    {
        for (unsigned i {0}; i < item.len; ++i)
        {
            if (item.id > 0)
            {
                crc += idx * item.id;
            }
            ++idx;
        }
    }

    fmt::println("2: {}", crc);
}

int main()
{
    Disk1 disk1;
    Disk2 disk2;

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
                disk1.push_back(x);
            }
            disk2.push_back({x, (unsigned)(c - '0')});

            if ((idx & 1) == 0)
            {
                ++id;
            }
        }
    }

    part1(disk1);
    part2(disk2);

    return 0;
}
