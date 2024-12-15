#include <cstdint>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <vector>

using Data = std::vector<uint64_t>;

void part1(Data data)
{
    for (unsigned blink {0}; blink < 25; ++blink)
    {
        Data d2;
        for (auto const v : data)
        {
            if (v == 0)
            {
                d2.push_back(1);
            }
            else
            {
                const uint64_t v_digits {static_cast<uint64_t>(std::log10(v) + 1)};
                if ((v_digits & 1) == 0)
                {
                    // even
                    const uint64_t base10div = std::pow(10, v_digits / 2);
                    const uint64_t v1 {v / base10div};
                    d2.push_back(v1);
                    d2.push_back(v - (v1 * base10div));
                }
                else
                {
                    d2.push_back(v * 2024);
                }
            }
        }

        std::swap(data, d2);
    }

    fmt::println("1: {}", data.size());
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
                   result.reserve(parts.size());
                   for (auto const& val : parts)
                   {
                       result.push_back(std::stoull(val));
                   }
                   return result;
               }()};

    part1(data);

    return 0;
}
