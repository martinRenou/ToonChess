<p align="center"><a href="https://martinrenou.github.io/ToonChess/"><img width="300" src="images/logo.png"></a></p>
<h1 align="center">ToonChess</h1>
<h2 align="center"> 3D Chess game based on OpenGL and Stockfish </h1>
<h2 align="center"><a href="https://martinrenou.github.io/ToonChess/">Website</a></h1>

[![Build Status](https://travis-ci.org/martinRenou/ToonChess.svg?branch=master)](https://travis-ci.org/martinRenou/ToonChess)

## Installation (linux only)

Install [CMake](https://cmake.org/), [Stockfish](https://stockfishchess.org/) and [OpenGL](https://www.opengl.org/):
```bash
sudo apt-get install cmake stockfish xorg-dev freeglut3-dev
```

Install [Bullet Physics](http://bulletphysics.org/wordpress/):
```bash
wget https://github.com/bulletphysics/bullet3/archive/2.87.tar.gz
tar -xzf 2.87.tar.gz
cd bullet3-2.87/
mkdir build && cd build && cmake .. -DBUILD_EXTRAS=OFF -DBUILD_BULLET2_DEMOS=OFF -DBUILD_CPU_DEMOS=OFF -DBUILD_OPENGL3_DEMOS=OFF
sudo make install
cd ..
```

Download source code:
```bash
git clone https://github.com/martinRenou/ToonChess.git
cd ToonChess
```

Install ToonChess:
```bash
mkdir build && cd build && cmake ..
sudo make install
```

Run it!
```bash
ToonChess
```

## Tests

Tests are written using [GoogleTest](https://github.com/google/googletest),
you can compile and run them using:
```bash
cmake -DTOONCHESS_BUILD_TESTS=ON ..
make
./toonchess_tests
```
