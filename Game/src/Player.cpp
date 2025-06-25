#include "Player.hpp"

#include <thread>
#include <functional>

#include "InputHandler.hpp"
#include "DisplayConstants.hpp"

using namespace DisplayConstants;

HumanPlayer::HumanPlayer( Chess::PieceColor color, const OnMoveCallback& callback)
    : Player{ color, callback } {
    InputHandler::instance().addListener(this);
}

HumanPlayer::~HumanPlayer() { InputHandler::instance().removeListener(this); }

static uint8_t screenCoordsToSquare(int x, int y, Chess::PieceColor playerColor) {
    int row = (y - k_boardStartY) / k_squareWidth;
    int col = (x - k_boardStartX) / k_squareWidth;
    if (playerColor == Chess::PieceColor::Black) {
        row = 7 - row;
        col = 7 - col;
    }
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        return row * 8 + col;
    }
    else {
        return Chess::Square::invalidSquare;
    }
}

void HumanPlayer::onMouseMove(int x, int y) {
    if (m_isDragging) {
        m_dragX = x;
        m_dragY = y;
    }
}

void HumanPlayer::onMousePress(int x, int y) {
    const uint8_t gameSquare = screenCoordsToSquare(x, y, m_color);

    if (gameSquare == Chess::Square::invalidSquare) {
        m_selectedSquare = Chess::Square::invalidSquare;
        m_isDragging = false;
    }

    else if (m_position->getPieceAt(gameSquare)) {
        m_selectedSquare = gameSquare;
        m_dragX = x;
        m_dragY = y;
        m_isDragging = true;
    }
    else if (m_selectedSquare != Chess::Square::invalidSquare) {
        (*m_callback)(*this, Chess::Move{ m_selectedSquare, gameSquare });

        m_selectedSquare = Chess::Square::invalidSquare;
        m_isDragging = false;
    }
}

void HumanPlayer::onMouseRelease(int x, int y) {
    const uint8_t gameSquare = screenCoordsToSquare(x, y, m_color);
    if (gameSquare == Chess::Square::invalidSquare) {
        m_selectedSquare = Chess::Square::invalidSquare;
    }
    else if (m_selectedSquare != Chess::Square::invalidSquare &&
        m_selectedSquare != gameSquare) {
        (*m_callback)(*this, Chess::Move{ m_selectedSquare, gameSquare });
        m_selectedSquare = Chess::Square::invalidSquare;
    }
    m_isDragging = false;
}

uint8_t HumanPlayer::getSelectedSquare() const { return m_selectedSquare; }

bool HumanPlayer::getIsDragging() const { return m_isDragging; }

int HumanPlayer::getDragX() const { return m_dragX; }

int HumanPlayer::getDragY() const { return m_dragY; }

AIPlayer::AIPlayer(Chess::PieceColor color, const OnMoveCallback& callback)
    : Player{ color, callback } {
}

// make this functor
void AIPlayer::queryMove() {
    std::thread t{ [this]() {
      const Chess::Move move = m_searcher.getMove(*m_position);
      (*m_callback)(*this, move);
    }};
    t.detach();
}