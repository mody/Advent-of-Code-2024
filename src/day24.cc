#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>

#include <array>
#include <deque>
#include <iostream>
#include <map>
#include <variant>

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


struct CPU
{
    using REG = std::array<char, 3>;
    using REGS = std::map<REG, bool>;

    struct BASE_OP
    {
        BASE_OP(CPU::REGS& regs, CPU::REG in1, CPU::REG in2, CPU::REG out)
            : mRegs{regs}
            , mIn1{std::move(in1)}
            , mIn2{std::move(in2)}
            , mOut{std::move(out)}
        {
        }

        bool trigger(this auto&& self) { return self.impl(); }

        CPU::REG get_in1() const { return mIn1; }
        CPU::REG get_in2() const { return mIn2; }
        CPU::REG get_out() const { return mOut; }

    protected:
        CPU::REGS& mRegs;
        CPU::REG mIn1, mIn2, mOut;
    };

    struct AND : public BASE_OP
    {
        using BASE_OP::BASE_OP;

        bool impl()
        {
            if (mRegs.contains(mIn1) && mRegs.contains(mIn2)) {
                mRegs[mOut] = mRegs.at(mIn1) && mRegs.at(mIn2);
                return true;
            }
            return false;
        }
    };

    struct OR : public BASE_OP
    {
        using BASE_OP::BASE_OP;

        bool impl()
        {
            if (mRegs.contains(mIn1) && mRegs.contains(mIn2)) {
                mRegs[mOut] = mRegs.at(mIn1) || mRegs.at(mIn2);
                return true;
            }
            return false;
        }
    };

    struct XOR : public BASE_OP
    {
        using BASE_OP::BASE_OP;

        bool impl()
        {
            if (mRegs.contains(mIn1) && mRegs.contains(mIn2)) {
                mRegs[mOut] = mRegs.at(mIn1) != mRegs.at(mIn2);
                return true;
            }
            return false;
        }
    };

    using OP = std::variant<AND, OR, XOR>;
    using OPS = std::vector<OP>;
    using ROPS = std::map<REG, std::vector<unsigned>>;

    void insert_op(std::string_view const sop, CPU::REG in1, CPU::REG in2, CPU::REG out)
    {
        unsigned pos = mOps.size();
        mROps[in1].push_back(pos);
        mROps[in2].push_back(pos);

        if (sop == "AND") {
            mOps.emplace_back(CPU::AND(mRegs, in1, in2, out));
        }
        else if (sop == "OR") {
            mOps.emplace_back(CPU::OR(mRegs, in1, in2, out));
        }
        else if (sop == "XOR") {
            mOps.emplace_back(CPU::XOR(mRegs, in1, in2, out));
        }
        else {
            throw std::runtime_error("Invalid OP");
        }
    }

    uint64_t run()
    {
        for (auto const& [r, v] : mInit) {
            mRegs[r] = v;

            std::deque<CPU::REG> q;
            q.push_back(r);

            while (!q.empty()) {
                auto current_r = q.front();
                q.pop_front();

                if (!mROps.contains(current_r))
                    continue;

                for (auto const& op_pos : mROps.at(current_r)) {
                    std::visit(
                            [&](auto&& arg)
                            {
                                if (arg.trigger()) {
                                    q.push_back(arg.get_out());
                                }
                            },
                            mOps.at(op_pos));
                }
            }
        }

        uint64_t result{0};
        for (auto const& [r, v] : mRegs) {
            if (r.at(0) == 'z' && v) {
                const unsigned pos = ((r.at(1) - '0') * 10) + (r.at(2) - '0');
                result |= 1ULL << pos;
            }
        }
        return result;
    }

    REGS mInit, mRegs;
    OPS mOps;
    ROPS mROps;
};


int main()
{
    CPU cpu;

    auto to_reg = [](const std::string_view v) -> CPU::REG { return CPU::REG{v.at(0), v.at(1), v.at(2)}; };

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty() || line.at(3) != ':')
            break;

        // x00: 1
        cpu.mInit.emplace(to_reg(line), line.at(5) == '1');
    }

    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;

        // ntg XOR fgs -> mjb

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
        assert(parts.size() == 5);

        cpu.insert_op(parts.at(1), to_reg(parts.at(0)), to_reg(parts.at(2)), to_reg(parts.at(4)));
    }

    fmt::println("1: {}", cpu.run());

    return 0;
}
