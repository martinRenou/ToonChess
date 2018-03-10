#include "gmock/gmock.h"

#include <SFML/Graphics.hpp>

#include "../src/PhysicsWorld/IPhysicsWorld.hxx"

class MockPhysicsWorld : public IPhysicsWorld {
public:
  MockPhysicsWorld() {};
  MOCK_METHOD2(collapsePiece, void(int piece, sf::Vector2i position));
  MOCK_METHOD0(simulate, void());
};
