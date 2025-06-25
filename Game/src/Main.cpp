#include <SDL_main.h>

#include <chrono>
#include <Chess.hpp>
#include "Graphics.hpp"
#include "InputHandler.hpp"
#include "ChessRenderer.hpp"
#include "GameManager.hpp"
#include "DisplayConstants.hpp"

int main(int argc [[maybe_unused]], char** argv [[maybe_unused]] ) {
    graphicsInit();
    Window window{ "Chess", DisplayConstants::k_windowWidth, DisplayConstants::k_windowHeight };
    ChessRenderer renderer{ window };
    Chess::init();

    GameManager gm{ GameType::PlayerComputer };
    window.show();

    while (!InputHandler::instance().shouldQuit()) {
        InputHandler::instance().handle();
        renderer.render(gm);
    }
    return 0;
}
