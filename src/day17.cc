#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <vector>

constexpr const char* header = R"XX(
#include <boost/algorithm/string/join.hpp>

#include <iostream>
#include <cmath>
#include <vector>

constexpr unsigned to_reg(unsigned val)
{
    return val & 0x7;
}

int main() {
)XX";


std::string to_reg(char val)
{
    switch (val)
    {
    case '4': return "regA"; break;
    case '5': return "regB"; break;
    case '6': return "regC"; break;
    case '7': assert(false); break;
    default: break;
    }
    return std::string {} + val;
}

void process_commands(std::vector<std::string> const& input)
{
    assert((input.size() % 2) == 0);

    std::cout << R"XX(
        std::vector<std::string> output;

        bool stop{false};
        for (int ip{0}; !stop; ip +=2) {
        switch(ip) {
    )XX";

    unsigned ip{0};
    for (auto it = input.begin(); it != input.end();)
    {
        const char cmd {(*it++)[0]};
        const char arg {(*it++)[0]};

        std::cout << "\tcase " << ip << ":\n";

        switch(cmd) {
        case '0':
            std::cout << "\t\tregA = regA / std::pow(2, " << to_reg(arg) << ");\n";
            std::cout << "\t\tbreak;\n";
            break;
        case '1':
            std::cout << "\t\tregB = regB ^ " << arg << ";\n";
            std::cout << "\t\tbreak;\n";
            break;
        case '2':
            std::cout << "\t\tregB = " << to_reg(arg) << " & 0x7;\n";
            std::cout << "\t\tbreak;\n";
            break;
        case '3':
            std::cout << "\t\tif (regA) ip = " << arg << " - 2;\n";
            std::cout << "\t\tbreak;\n";
            break;
        case '4':
            std::cout << "\t\tregB = regB ^ regC;\n";
            std::cout << "\t\tbreak;\n";
            break;
        case '5':
            std::cout << "\t\toutput.push_back(std::to_string(" << to_reg(arg) << " & 0x7));\n";
            std::cout << "\t\tbreak;\n";
            break;
        case '6':
            std::cout << "\t\tregB = regA / std::pow(2, " << to_reg(arg) << ");\n";
            std::cout << "\t\tbreak;\n";
            break;
        case '7':
            std::cout << "\t\tregC = regA / std::pow(2, " << to_reg(arg) << ");\n";
            std::cout << "\t\tbreak;\n";
            break;
        default:
            assert(false);
        }
        ip += 2;
    }
    std::cout << "\tcase " << ip << ":\n\t\tstop = true;\n\t\tbreak;\n";
    std::cout << R"XX(
        default:
            assert(false);
    } // switch
    } // for

    std::cout << boost::algorithm::join(output, ",") << std::endl;

    return 0;
    }

    )XX";
}


int main()
{
    std::cout << header;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            continue;
        }
        else if (boost::starts_with(line, "Register A: "))
        {
            std::cout << "[[maybe_unused]] unsigned regA{" << line.substr(12) << "};\n";
        }
        else if (boost::starts_with(line, "Register B: "))
        {
            std::cout << "[[maybe_unused]] unsigned regB{" << line.substr(12) << "};\n";
        }
        else if (boost::starts_with(line, "Register C: "))
        {
            std::cout << "[[maybe_unused]] unsigned regC{" << line.substr(12) << "};\n";
        }
        else if (boost::starts_with(line, "Program: "))
        {
            std::vector<std::string> parts;
            boost::algorithm::split(parts, line.substr(9), boost::algorithm::is_any_of(","), boost::token_compress_on);

            process_commands(parts);

            break;
        }
    }

    return 0;
}
