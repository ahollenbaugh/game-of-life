# Conway's Game of Life

## How to run locally
### macOS (Apple Silicon)
1. Open Terminal and run `brew install sfml` (installs v3 by default; this program is no longer compatible with v2)
2. Run `brew info sfml` to get the installation path (you'll need it for the next step)
3. Run `g++ -std=c++17 main.cpp -I/opt/homebrew/Cellar/sfml/3.0.2/include -o prog -L/opt/homebrew/Cellar/sfml/3.0.2/lib -lsfml-graphics -lsfml-window -lsfml-system` 
    1. g++ is a compiler
    2. `-std=c++17` tells g++ to use the C++17 standard
    3. `-I/opt/homebrew/Cellar/sfml/3.0.2/include` tells it where to find the SFML header files
    4. `-o prog` tells it to bundle everything into an executable file called `prog`
    5. `-L/opt/homebrew/Cellar/sfml/3.0.2/lib` tells it where to find the SFML library files
    6. `-lsfml-graphics -lsfml-window -lsfml-system` tells it to link these SFML libraries with the GOL code
4. Run `./prog`

## Features
- Play/pause
- Clear screen
- Generate random pattern
- Toggle individual squares as live/dead
- Save current pattern
- Load pattern

Demo: https://youtu.be/9Ng2I91awQY
