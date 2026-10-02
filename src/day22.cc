#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/view/zip.hpp>

#include <algorithm>
#include <iostream>
#include <vector>


uint64_t seller_hash(uint64_t val)
{
    val = (val ^ (val * 64)) % 16777216;
    val = (val ^ (val / 32)) % 16777216;
    val = (val ^ (val * 2048)) % 16777216;
    return val;
}


void part1(std::vector<uint64_t> seller)
{
    for (auto& val : seller) {
        for (int i{0}; i < 2000; ++i) {
            val = seller_hash(val);
        }
    }

    fmt::println("1: {}", std::ranges::fold_left(seller, 0ULL, std::plus{}));
}


int main()
{
    std::vector<uint64_t> seller;

    {
        std::string line;
        while (std::getline(std::cin, line))
        {
            if (line.empty())
                break;

            seller.emplace_back(std::stoull(line));
        }
    }

    //  uint64_t val{123};
    //  for (int i{0}; i < 10; ++i) {
    //      val = seller_hash(val);
    //      fmt::println("{}", val);
    //  }
    part1(seller);

    return 0;
}
