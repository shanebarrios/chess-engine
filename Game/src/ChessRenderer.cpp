#include "ChessRenderer.hpp"

#include <unordered_map>

#include "GameManager.hpp"
#include "Player.hpp"
#include "DisplayConstants.hpp"

using namespace DisplayConstants;

namespace {
    constexpr Rect gameSquareToDrawRect(int square, Chess::PieceColor playerColor) {

        int row = square / 8;
        int col = square % 8;
        if (playerColor == Chess::PieceColor::Black) {
            row = 7 - row;
            col = 7 - col;
        }
        return Rect{ k_boardStartX + col * k_squareWidth,
                    k_boardStartY + row * k_squareWidth, k_squareWidth,
                    k_squareWidth };
    }

}  // namespace

Texture ChessRenderer::spriteSheet{};

ChessRenderer::ChessRenderer(Window& window) : m_renderer{ window } {
    if (!spriteSheet) {
        spriteSheet.loadImage(m_renderer, "assets/chess_pieces.png");
    }
}

Rect ChessRenderer::getSpriteRect(Chess::Piece piece) {
    int x = 0;

    switch (piece.type) {
    case Chess::PieceType::Null:
        throw std::invalid_argument{ "Invalid piece" };
    case Chess::PieceType::King:
        x = 0;
        break;
    case Chess::PieceType::Queen:
        x = 1;
        break;
    case Chess::PieceType::Bishop:
        x = 2;
        break;
    case Chess::PieceType::Knight:
        x = 3;
        break;
    case Chess::PieceType::Rook:
        x = 4;
        break;
    case Chess::PieceType::Pawn:
        x = 5;
        break;
    }

    int y = (piece.color == Chess::PieceColor::White) ? 0 : 1;
    int imgWidth = spriteSheet.getWidth();
    int imgHeight = spriteSheet.getHeight();
    int xGap = imgWidth / 6;
    int yGap = imgHeight / 2;
    return Rect{ x * xGap, y * yGap, xGap, yGap };
}

void ChessRenderer::drawBoard(const GameManager& gm) {
    const Chess::Position& position{ gm.getPosition() };
    const HumanPlayer* player{ gm.getHumanPlayer() };

    int colorIndex = 0;
    for (int square = 0; square < 64; square++) {
        if (square % 8 != 0) {
            colorIndex = (colorIndex + 1) % 2;
        }
        Rect rect = gameSquareToDrawRect(
            square, player ? player->getColor() : Chess::PieceColor::White);
        Color color = boardColors[colorIndex];
        m_renderer.setDrawColor(color);
        m_renderer.fillRect(rect);

        Chess::Piece piece = position.getPieceAt(square);
        if (piece && (!player || !player->getIsDragging() ||
            player->getSelectedSquare() != square)) {
            Rect spriteRect = getSpriteRect(piece);
            m_renderer.drawTexture(spriteSheet, spriteRect, rect);
        }
    }
}

void ChessRenderer::drawDraggingPiece(const GameManager& gm) {
    const HumanPlayer* player{ gm.getHumanPlayer() };
    if (!player || !player->getIsDragging()) {
        return;
    }
    Chess::Piece draggedPiece = gm.getPiece(player->getSelectedSquare());
    Rect rect{ player->getDragX() - k_squareWidth / 2,
              player->getDragY() - k_squareWidth / 2, k_squareWidth,
              k_squareWidth };
    m_renderer.drawTexture(spriteSheet, getSpriteRect(draggedPiece), rect);
}

void ChessRenderer::drawLegalMoves(const GameManager& gm) {
    const HumanPlayer* player{ gm.getHumanPlayer() };

    if (!player || player->getSelectedSquare() == Chess::Square::invalidSquare ||
        gm.getPosition().getTurn() != player->getColor()) {
        return;
    }
    uint8_t selectedSquare = player->getSelectedSquare();
    const Chess::MoveList& legalMoves{ gm.getLegalMoves() };
    for (int i = 0; i < legalMoves.size(); i++) {
        Chess::Move move = legalMoves[i];
        if (move.start == selectedSquare) {
            m_renderer.setDrawColor(legalMoveColor);
            Rect rect = gameSquareToDrawRect(move.target, player->getColor());
            m_renderer.fillRect(rect);
        }
    }
}

void ChessRenderer::render(const GameManager& gm) {
    m_renderer.setDrawColor(0x30, 0x30, 0x30);
    m_renderer.clear();
    drawBoard(gm);
    drawLegalMoves(gm);
    drawDraggingPiece(gm);
    m_renderer.present();
}