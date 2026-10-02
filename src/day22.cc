#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/view/zip.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <ranges>
#include <vector>


uint64_t seller_hash(uint64_t val)
{
    val = (val ^ (val * 64)) % 16777216;
    val = (val ^ (val / 32)) % 16777216;
    val = (val ^ (val * 2048)) % 16777216;
    return val;
}


signed char price(uint64_t const& val)
{
    return static_cast<signed char>(val % 10);
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


void part2(std::vector<uint64_t> const& seller)
{
    std::map<uint32_t, std::map<unsigned, signed char>> seq_to_price;

    for (auto const& [idx, val0] : std::views::enumerate(seller)) {
        auto val1{val0};
        signed char price1{price(val1)};
        uint32_t seq{0};
        for (int i{0}; i < 2000; ++i) {
            const uint64_t val2{seller_hash(val1)};
            const signed char price2{price(val2)};
            const signed char diff = price2 - price1;

            seq = ((seq << 8) & 0xFFFFFF00) | (diff & 0xFF);

            if (i >= 3) {
                seq_to_price[seq].emplace(idx, price2);
            }

            val1 = val2;
            price1 = price2;
        }
    }

    [[maybe_unused]] int32_t best_seq{0};
    int64_t best_price{0};

    for (auto const& [seq, prices] : seq_to_price) {
        int price{0};
        for (auto const& [idx, val] : prices) {
            price += val;
        }
        if (best_price < price) {
            best_seq = seq;
            best_price = price;
        }
    }

    //  auto seq_to_arr = [](uint32_t seq) -> std::array<signed char, 4>
    //  {
    //      return {static_cast<signed char>((seq & 0xFF000000) >> 24),
    //              static_cast<signed char>((seq & 0x00FF0000) >> 16),
    //              static_cast<signed char>((seq & 0x0000FF00) >> 8),
    //              static_cast<signed char>((seq & 0x000000FF))};
    //  };
    //  fmt::println("2: {:x} -> {} -> {}", best_seq, seq_to_arr(best_seq), best_price);

    fmt::println("2: {}", best_price);
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

    part1(seller);
    part2(seller);

    return 0;
}
