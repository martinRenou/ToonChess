<p align="center"><img width="700" src="images/ToonChess.png"></p>
<h1 align="center"><a href="https://martinrenou.github.io/ToonChess/">ToonChess</a></h1>
<h1 align="center"> 3D Chess game based on OpenGL and Stockfish </h1>

[![Build Status](https://travis-ci.org/martinRenou/ToonChess.svg?branch=master)](https://travis-ci.org/martinRenou/ToonChess)

## Installation (linux only)

Install [CMake](https://cmake.org/), [SFML](https://www.sfml-dev.org/index.php) and [Stockfish](https://stockfishchess.org/):
```bash
sudo apt-get install cmake libsfml-dev stockfish
```

Download source code:
```bash
git clone https://github.com/martinRenou/ToonChess.git
cd ToonChess
```

Compile ToonChess:
```bash
mkdir build && cd build && cmake ..
make
```

Run it!
```bash
./ToonChess
```

## Tests

Tests are written using [GoogleTest](https://github.com/google/googletest), you can run them after installation with:
```bash
./tests
```
