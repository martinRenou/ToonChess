#ifndef FRAGMENT_HXX_
#define FRAGMENT_HXX_

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include "../mesh/Mesh.hxx"


// cppcheck-suppress noCopyConstructor
class Fragment {
  public:
    /* Constructor */
    explicit Fragment(Mesh* mesh);

    /* Fragment shape */
    btShapeHull* hull;
    btConvexHullShape* convexHullShape;

    /* Mesh */
    Mesh* mesh;

    btVector3 inertia;
    float mass;

    /* Destructor */
    ~Fragment();
};

#endif
