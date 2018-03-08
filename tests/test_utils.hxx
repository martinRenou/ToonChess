#include "gmock/gmock.h"

#include <SFML/Graphics.hpp>

#include


class MockPhysicsWorld : public PhysicsWorld {
public:
  MockPhysicsWorld() {};
  MOCK_METHOD2(collapsePiece, void(int piece, sf::Vector2i position));
};
