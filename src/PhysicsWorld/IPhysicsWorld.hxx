#ifndef IPHYSICSWORLD_HXX_
#define IPHYSICSWORLD_HXX_

#include <SFML/Graphics.hpp>

/*
  Interface for PhysicsWorld, needed so that it can be mocked in tests
*/
class IPhysicsWorld {
  public:
    virtual void collapsePiece(int piece, sf::Vector2i position) = 0;
    virtual void simulate() = 0;

    /* Destructor */
    virtual ~IPhysicsWorld() {}
};

#endif
