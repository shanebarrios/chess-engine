#pragma once

#include <bit>
#include <cstdint>

namespace Chess 
{
    class Bitboard {
    public:
        constexpr Bitboard() : m_encoding{} {}
        explicit constexpr Bitboard(uint64_t encoding) : m_encoding{ encoding } {}

        static constexpr Bitboard full() { return Bitboard{ UINT64_MAX }; }

        static constexpr Bitboard empty() { return Bitboard{}; }

        static constexpr Bitboard fromSquare(uint8_t square) {
            return Bitboard{ 1ULL << square };
        }

        static constexpr Bitboard mask8() { return Bitboard{ 0xFFULL << (8 * 0) }; }
        static constexpr Bitboard mask7() { return Bitboard{ 0xFFULL << (8 * 1) }; }
        static constexpr Bitboard mask6() { return Bitboard{ 0xFFULL << (8 * 2) }; }
        static constexpr Bitboard mask5() { return Bitboard{ 0xFFULL << (8 * 3) }; }
        static constexpr Bitboard mask4() { return Bitboard{ 0xFFULL << (8 * 4) }; }
        static constexpr Bitboard mask3() { return Bitboard{ 0xFFULL << (8 * 5) }; }
        static constexpr Bitboard mask2() { return Bitboard{ 0xFFULL << (8 * 6) }; }
        static constexpr Bitboard mask1() { return Bitboard{ 0xFFULL << (8 * 7) }; }

        static constexpr Bitboard maskA() { return Bitboard{ 0x0101010101010101ULL }; }
        static constexpr Bitboard maskH() { return Bitboard{ 0x8080808080808080ULL }; }

        constexpr Bitboard north() const { return Bitboard{ m_encoding >> 8 }; }

        constexpr Bitboard south() const { return Bitboard{ m_encoding << 8 }; }

        constexpr Bitboard east() const {
            return Bitboard{ (m_encoding << 1) } &~maskA();
        }

        constexpr Bitboard west() const {
            return Bitboard{ (m_encoding >> 1) } &~maskH();
        }

        constexpr Bitboard northEast() const {
            return Bitboard{ (m_encoding >> 7) } &~maskA();
        }

        constexpr Bitboard northWest() const {
            return Bitboard{ (m_encoding >> 9) } &~maskH();
        }

        constexpr Bitboard southEast() const {
            return Bitboard{ (m_encoding << 9) } &~maskA();
        }

        constexpr Bitboard southWest() const {
            return Bitboard{ (m_encoding << 7) } &~maskH();
        }

        constexpr uint64_t get() const { return m_encoding; }

        void setBit(uint8_t square) { m_encoding |= (1ULL << square); }

        void clearBit(uint8_t square) { m_encoding &= ~(1ULL << square); }

        constexpr uint8_t getLSBIndex() const {
            return static_cast<uint8_t>(std::countr_zero(m_encoding));
        }

        void clearLSB() { m_encoding &= (m_encoding - 1); };

        uint8_t popLSB() {
            uint8_t square = getLSBIndex();
            clearLSB();
            return square;
        }

        constexpr bool checkBit(uint8_t square) const {
            return m_encoding & (1ULL << square);
        }

        constexpr uint8_t numBits() const {
            return static_cast<uint8_t>(std::popcount(m_encoding));
        }

        constexpr Bitboard operator|(Bitboard other) const {
            return Bitboard{ m_encoding | other.m_encoding };
        }

        void operator|=(Bitboard other) { m_encoding |= other.m_encoding; }

        constexpr Bitboard operator&(Bitboard other) const {
            return Bitboard{ m_encoding & other.m_encoding };
        }

        void operator&=(Bitboard other) { m_encoding &= other.m_encoding; }

        constexpr Bitboard operator~() const { return Bitboard{ ~m_encoding }; }

        constexpr Bitboard operator<<(int shift) const {
            return Bitboard{ m_encoding << shift };
        }

        void operator<<=(int shift) { m_encoding <<= shift; }

        constexpr Bitboard operator>>(int shift) const {
            return Bitboard{ m_encoding >> shift };
        }

        void operator>>=(int shift) { m_encoding >>= shift; }

        constexpr uint64_t operator*(uint64_t num) const { return m_encoding * num; }

        constexpr operator bool() const { return m_encoding != 0; }

        constexpr bool operator!() const { return m_encoding == 0; };

        constexpr bool operator==(Bitboard other) const {
            return m_encoding == other.m_encoding;
        }

        constexpr bool operator!=(Bitboard other) const {
            return m_encoding != other.m_encoding;
        }

    private:
        uint64_t m_encoding{};
    };
}