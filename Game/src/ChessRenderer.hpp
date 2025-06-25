#pragma once

#include <optional>

#include <Chess.hpp>

#include "Graphics.hpp"

class GameManager;
class Window;

class ChessRenderer {
public:
	explicit ChessRenderer(Window& window);

	void render(const GameManager& gm);

private:
	static Texture spriteSheet;
	static Rect getSpriteRect(Chess::Piece piece);

	Renderer m_renderer;

	void drawBoard(const GameManager& gm);
	void drawDraggingPiece(const GameManager& gm);
	void drawLegalMoves(const GameManager& gm);
};