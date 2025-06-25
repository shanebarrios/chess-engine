
#ifdef GEN_MAGIC

#include <array>
#include <fstream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Bitboard.hpp"
#include "Utils.hpp"
#include "DataStructures.hpp"

namespace Chess {
    struct MagicEntry {
        Bitboard mask;
        uint64_t magic;
        uint8_t shifts;
    };

    Bitboard getMagicMask(uint8_t square, bool rook) {
        Bitboard mask{};

        const uint8_t start = rook ? 0 : 4;
        const uint8_t end = rook ? 3 : 7;

        for (uint8_t dirIndex = start; dirIndex <= end; dirIndex++) {
            const Utils::Coordinate dcoord = Utils::slidingDirections[dirIndex];
            auto coord = Utils::Coordinate::fromSquare(square);

            while ((coord + dcoord).inBounds()) {
                mask.setBit(coord.toSquare());
                coord += dcoord;
            }
        }
        mask.clearBit(square);
        return mask;
    }

    MagicEntry genMagic(uint8_t square, bool rook) {
        Bitboard mask = getMagicMask(square, rook);
        uint8_t shifts = 64 - mask.numBits();

        static std::mt19937 mt{ std::random_device{}() };
        static std::uniform_int_distribution<uint64_t> distrib(0, UINT64_MAX);

        const std::vector<Bitboard> subsets{ Utils::getSubsets(mask) };

        while (true) {
            std::vector<int> visited(1 << (64 - shifts));
            const uint64_t candidate = distrib(mt) & distrib(mt) & distrib(mt);
            bool success = true;

            for (Bitboard blockers : subsets) {
                const uint64_t index = (blockers * candidate) >> shifts;
                if (visited[index]) {
                    success = false;
                    break;
                }
                visited[index] = 1;
            }
            if (success) {
                return MagicEntry{ mask, candidate, shifts };
            }
        }
    }

    std::ostream& operator<<(std::ostream& out, const MagicEntry& entry) {
        out << "{Bitboard {" << entry.mask.get() << "ULL}, " << entry.magic << "ULL, "
            << static_cast<int>(entry.shifts) << "}";
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const Array<MagicEntry, 64>& arr) {
        out << "{{";
        for (uint8_t i = 0; i < 63; i++) {
            out << arr[i] << ", ";
        }
        out << arr[63] << "}}";
        return out;
    }

    void outputToFile(std::string_view file) {
        std::ofstream outf{ file.data() };
        Array<MagicEntry, 64> rookMagics;
        Array<MagicEntry, 64> bishopMagics;

        if (!outf) {
            throw std::runtime_error{ "Failed to open file" };
        }

        for (uint8_t square = 0; square < 64; square++) {
            rookMagics[square] = genMagic(square, true);
            bishopMagics[square] = genMagic(square, false);
        }

        outf << "#pragma once\n\n"
            "#include \"Bitboard.hpp\"\n\n"
            "namespace Chess {\n"
            "  struct MagicEntry {\n"
            "    Bitboard mask;\n"
            "    uint64_t magic;\n"
            "    uint8_t shifts;\n"
            "  };\n\n"
            "  namespace MagicData {\n";
        outf << "    constexpr Array<MagicEntry, 64> rookMagics " << rookMagics << ";\n\n";
        outf << "    constexpr Array<MagicEntry, 64> bishopMagics " << bishopMagics << ";\n";
        outf << "  }\n";
        outf << "}"
    }

    int main() { outputToFile("src/game/MagicData.hpp"); }
}
#endif