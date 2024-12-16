#include <cstdint>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <map>
#include <vector>

using Data = std::map<uint64_t, uint64_t>;

uint64_t process(Data const& data, unsigned blinks)
{
    auto d1 {data};

    for (unsigned blink {0}; blink < blinks; ++blink)
    {
        Data d2;
        for (auto const& [v, cnt] : d1)
        {
            if (v == 0)
            {
                d2.insert({1ul, 0}).first->second += cnt;
            }
            else
            {
                const uint64_t v_digits {static_cast<uint64_t>(std::log10(v) + 1)};
                if ((v_digits & 1) == 0)
                {
                    // even
                    const uint64_t base10div = std::pow(10, v_digits / 2);
                    const uint64_t v1 {v / base10div};
                    d2.insert({v1, 0}).first->second += cnt;
                    d2.insert({v - (v1 * base10div), 0}).first->second += cnt;
                }
                else
                {
                    d2.insert({v * 2024ul, 0}).first->second += cnt;
                }
            }
        }

        std::swap(d1, d2);
    }

    uint64_t count {0};
    for (auto const& [_, cnt] : d1)
    {
        count += cnt;
    }

    return count;
}


int main()
{
    Data data {[]()
               {
                   Data result;
                   std::string line;
                   std::getline(std::cin, line);
                   std::vector<std::string> parts;
                   boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
                   for (auto const& val : parts)
                   {
                       result.insert({std::stoull(val), 1});
                   }
                   return result;
               }()};


    fmt::println("1: {}", process(data, 25));
    fmt::println("2: {}", process(data, 75));

    return 0;
}
