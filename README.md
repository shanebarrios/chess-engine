# Chess Engine

A relatively standard chess engine build with C++. The repository consists of three distinct but related projects: the engine itself, a GUI built with SDL, and a client that communicates with the Lichess API.   

As of now, the bot has not played any rated games, but a rough estimate of its strength is about 2000 elo. It usually beats Stockfish level 6 on Lichess, which is about 1900 elo.

## Features

- Engine
  - Magic bitboards
  - Piece square tables
  - Alpha beta pruning
  - Transposition tables
  - Killer moves
  - History heuristic
  - Quiescence search
  - Iterative deepening
- UI
  - Drag and drop or click to move pieces
  - Blocking, event based game loop to limit CPU usage
  - Play against the AI
- Lichess Client
  - Handles incoming challenges from other players
  - Sends challenges to the Lichess AI
  - Manages concurrent game sessions, up to six at once
  - Formatted logging
    
## Building

### Prerequisites

You will need CMake, a C++20 compatible compiler, and a package manager (the setup guide assumes you are using vcpkg). You should be able to use other package managers, but it's up to you to make sure CMake can find the necessary packages. [Instructions for setting up vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell)

The applications have been tested with MSVC on Windows and clang on macOS. 

### Setup

First, clone the repository
```
git clone https://github.com/shanebarrios/chess-engine
cd chess-engine
```
Then run
```
cmake -Bbuild -S. -DCMAKE_TOOLCHAIN_FILE="<path to vcpkg root>/scripts/buildsystems/vcpkg.cmake"
cmake --build build
```
Alternatively, there are presets set in `CMakePresets.json` that work well with Visual Studio. 
Afterwards, the `Chess` and `Client` executables will generate within a `build/bin`. 
Note that you must set an environment variable `LICHESS_API_TOKEN` to a valid api token to run the `Client` program.

