#include "Utils.hpp"

#include <cstdint>
#include <sstream>
#include <iostream>
#include <cassert>
#include <string_view>

using namespace Chess;

std::string Utils::squareToStr(uint8_t square) {
    int row = square / 8;
    int col = square % 8;

    char file = col + 'a';
    char rank = '8' - row;

    return std::string{ file } + std::string{ rank };
}

std::string Utils::moveToStr(Move move) {
    return squareToStr(move.start) + squareToStr(move.target);
}

char Utils::pieceToChar(Piece piece) {
    char lower;
    switch (piece.type) {
    case PieceType::Pawn:
        lower = 'p';
        break;
    case PieceType::Bishop:
        lower = 'b';
        break;
    case PieceType::Knight:
        lower = 'n';
        break;
    case PieceType::Rook:
        lower = 'r';
        break;
    case PieceType::Queen:
        lower = 'q';
        break;
    case PieceType::King:
        lower = 'k';
        break;
    default:
        return '?';
    }
    return piece.color == PieceColor::White ? toupper(lower) : lower;
}

PieceType Utils::charToPieceType(char c) {
    switch (tolower(c)) {
    case 'p':
        return PieceType::Pawn;
    case 'b':
        return PieceType::Bishop;
    case 'n':
        return PieceType::Knight;
    case 'r':
        return PieceType::Rook;
    case 'q':
        return PieceType::Queen;
    case 'k':
        return PieceType::King;
    default:
        return PieceType::Null;
    }
}

Move Utils::strToMove(std::string_view moveStr) {
    assert(moveStr.length() >= 4);
    int rowStart = 7 - (moveStr[1] - '1');
    int rowEnd = 7 - (moveStr[3] - '1');
    int colStart = moveStr[0] - 'a';
    int colEnd = moveStr[2] - 'a';

    uint8_t squareStart = static_cast<uint8_t>(rowStart * 8 + colStart);
    uint8_t squareEnd = static_cast<uint8_t>(rowEnd * 8 + colEnd);

    PieceType promotion = moveStr.length() > 4 ? charToPieceType(moveStr[4]) : PieceType::Null;
    
    return Move{ squareStart, squareEnd, promotion};
}

std::vector<Bitboard> Utils::getSubsets(Bitboard board) {
    std::vector<Bitboard> subsets{};
    const uint64_t boardNum = board.get();
    uint64_t subset = 0;
    do {
        subsets.push_back(Bitboard{ subset });
        subset = (subset - boardNum) & boardNum;
    } while (subset != 0);
    return subsets;
}

std::string Utils::bitboardToStr(Bitboard board) {
    std::stringstream ss{};
    for (uint8_t i = 0; i < 64; i++) {
        ss << (board.checkBit(i) ? '1' : '0');
        if (i % 8 == 7) {
            ss << '\n';
        }
        else {
            ss << ' ';
        }
    }
    return ss.str();
}

std::vector<std::string> Utils::split(std::string_view str, char delimeter) {
    if (str.length() == 0) {
        return {};
    }
    std::vector<std::string> ret{};
    size_t end{ str.find_first_of(delimeter, 0) };
    size_t cur{ 0 };
    while (end != std::string::npos) {
        ret.emplace_back(str.substr(cur, end - cur));
        cur = end + 1;
        end = str.find_first_of(delimeter, cur);
    }
    ret.emplace_back(str.substr(cur, end - cur));
    return ret;
}

void Utils::positionDebugPrint(const Position& position) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char c = Utils::pieceToChar(position.getPieceAt(i * 8 + j));
            std::cout << c << " ";
        }
        std::cout << '\n';
    }
}