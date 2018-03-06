#ifndef FRAGMENT_HXX_
#define FRAGMENT_HXX_

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include <SFML/Graphics.hpp>

#include "../mesh/Mesh.hxx"


// cppcheck-suppress noCopyConstructor
class Fragment {
  public:
    /* Constructor */
    explicit Fragment(Mesh* mesh, sf::Vector2i position);

    /* Shape */
    btShapeHull* hull;
    btConvexHullShape* convexHullShape;

    /* Motion State */
    btDefaultMotionState* motionState;

    /* Rigid body */
    btRigidBody* rigidBody;

    /* Mesh */
    Mesh* mesh;

    btVector3 inertia;
    float mass;

    /* Destructor */
    ~Fragment();
};

#endif
