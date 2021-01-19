# TetrisConsole

This is a Tetris game for a console Window.
[It is based on the great example by javidx9 -OneLoneCoder-.]
(https://www.youtube.com/watch?reload=9&v=8OK8_tHeCIA)


## Language
It is written using C++, it uses `std::array`, `std::vector` instead of tradicional C arrays,`constexpr` members and I tried to avoid some crypted expressions and instead use a more verbose sintax, but I think the code is easier to understand for a novice programmer.

## Files
- `vpgTetris.cpp` contains the main function and initialize the game.
- `CTetris.cpp` contains the CTetris class that implements the game functionality
- `tetrisLoneCoder.cpp` contains the original code created by javidX9
- `Tetris\Tetris.sln` contains a Visual Studio 2019 solution for building and debbuging the application

## Building
It can be built using the VS solution or from the command line:
`cl /W4 /EHsc /Fe:Tetris.exe vpgTetris.cpp CTetris.cpp`

## Notes
As this is console program that depends on the size of the window, the program checks that the size of the window is 80*30