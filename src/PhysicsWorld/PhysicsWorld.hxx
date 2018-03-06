#ifndef PHYSICSWORLD_HXX_
#define PHYSICSWORLD_HXX_

#include <vector>
#include <map>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include <btBulletDynamicsCommon.h>

#include "Fragment.hxx"
#include "../mesh/Mesh.hxx"


// cppcheck-suppress noCopyConstructor
class PhysicsWorld {
  private:
    /* Map of fragments, it contains the Mesh instances associated to each
      piece */
    std::map<int, std::vector<Mesh*>>* fragmentMeshes;

    /* Map of fragments, it contains the Fragment instances associated to each
      piece */
    std::map<int, std::vector<Fragment*>> fragments;

    /* Dynamics world */
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    /* Ground rigid body */
    btCollisionShape* groundShape;
    btDefaultMotionState* groundMotionState;
    btRigidBody* groundRigidBody;

    /* Initialize the list of fragment instances, given the piece */
    std::vector<Fragment*> initFragments(int piece);

    /* Delete fragments from memory, given the piece */
    void deleteFragments(int piece);

  public:
    /* Constructor */
    explicit PhysicsWorld(std::map<int, std::vector<Mesh*>>* fragmentMeshes);

    /* Adds a piece to the dynamics world and collapse it
      \param piece Piece that you want to collapse: KING, QUEEN, BISHOP...
      \param position The position of the piece on the grid
    */
    void collapsePiece(int piece, sf::Vector2i position);

    /* Fragment pool, it contains each fragment actually in the dynamics world
      as a a pair of btRigidBody* and Mesh* */
    std::vector<std::pair<btRigidBody*, Mesh*>> fragmentPool;

    /* Simulate method, this will update the position of each fragment of the
      fragment pool */
    void simulate();

    /* Destructor */
    ~PhysicsWorld();
};

#endif
