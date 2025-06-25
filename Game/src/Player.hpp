#pragma once

#include "IMouseListener.hpp"
#include <Chess.hpp>

#include "SquareAliases.hpp"

class GameManager;
class Player;

using OnMoveCallback = std::function<void(const Player&, Chess::Move)>;

class Player {
public:
    Player(Chess::PieceColor color, const OnMoveCallback& callback) : m_color{ color }, m_callback{ &callback } {};
    virtual ~Player() = default;

    void updatePosition(const Chess::Position& position) { m_position = &position; };
    void reseatCallback(const OnMoveCallback& callback) { m_callback = &callback;  }

    Chess::PieceColor getColor() const { return m_color; };

    virtual void queryMove() = 0;

protected:
    Chess::PieceColor m_color;
    const Chess::Position* m_position{nullptr};
    const OnMoveCallback* m_callback;
};

class HumanPlayer : public Player, public IMouseListener {
public:
    HumanPlayer(Chess::PieceColor color, const OnMoveCallback& callback);
    ~HumanPlayer();

    void queryMove() override {};

    void onMouseMove(int x, int y) override;
    void onMousePress(int x, int y) override;
    void onMouseRelease(int x, int y) override;

    bool getIsDragging() const;
    uint8_t getSelectedSquare() const;

    int getDragX() const;
    int getDragY() const;

private:
    uint8_t m_selectedSquare{ Chess::Square::invalidSquare };
    bool m_isDragging{ false };
    int m_dragX{};
    int m_dragY{};
};

class AIPlayer : public Player {
public:
    AIPlayer(Chess::PieceColor color, const OnMoveCallback& callback);

    void queryMove() override;

private:
    Chess::Searcher m_searcher{};
};