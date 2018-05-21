<p align="center"><a href="https://martinrenou.github.io/ToonChess/"><img width="300" src="images/logo.png"></a></p>
<h1 align="center">ToonChess</h1>
<h2 align="center"> 3D Chess game based on OpenGL </h1>
<h2 align="center"><a href="https://martinrenou.github.io/ToonChess/">Website</a></h1>

[![Build Status](https://travis-ci.org/martinRenou/ToonChess.svg?branch=master)](https://travis-ci.org/martinRenou/ToonChess)

## Installation (linux only)

### Installation of dependencies

Install [CMake](https://cmake.org/) and [OpenGL](https://www.opengl.org/):
```bash
sudo apt-get install cmake xorg-dev freeglut3-dev
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

If you want to use [Stockfish](https://stockfishchess.org/) as AI (Stockfish will be used by default but any AI using the [UCI protocol](http://wbec-ridderkerk.nl/html/UCIProtocol.html) can work with ToonChess) you can install it using the following:
```bash
sudo apt-get install stockfish
```

### Installation of ToonChess

You can install the Python GUI for ToonChess, it provides a nice GUI for changing game parameters like the AI that you want to use, the screen resolution or color of pieces.

But it's not required to install the Python GUI if you do not want to, you would still be able to configure ToonChess as you wish, manually changing the `/home/user/.config/toonchess/config.txt` file.

Download source code:
```bash
git clone https://github.com/martinRenou/ToonChess.git
cd ToonChess
```

#### Installation of ToonChess with Python GUI

If you want to, you can install the python GUI for ToonChess, you will need one of the following installed on your computer in order to make it work:

- PyQt
- wxPython
- PySide
- PyQt5

Install ToonChess:
```bash
mkdir build && cd build
cmake ..
sudo make install
```

#### Installation of ToonChess without Python GUI

```bash
mkdir build && cd build
cmake -DTOONCHESS_BUILD_PYTHON_GUI=OFF ..
sudo make install
```

#### Run it!
```bash
toonchess
```

## Tests

Tests are written using [GoogleTest](https://github.com/google/googletest),
you can compile and run them using:
```bash
cmake -DTOONCHESS_BUILD_TESTS=ON ..
make
./toonchess_tests
```
