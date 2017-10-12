#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;

int main(int argc, char* argv[]) {
  sf::Window window(sf::VideoMode(800, 600), "3DToonChess");

  while(window.isOpen()){
    sf::Event event;

    while(window.pollEvent(event)){
      if(event.type == sf::Event::Closed) window.close();
    }

    window.display();
  }

  return 0;
}
