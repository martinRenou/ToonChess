#ifndef PHYSICSWORLD_HXX_
#define PHYSICSWORLD_HXX_

#include <vector>
#include <map>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include <btBulletDynamicsCommon.h>

#include "Fragment.hxx"
#include "../mesh/Mesh.hxx"
#include "../ChessGame/ChessGame.hxx"


// cppcheck-suppress noCopyConstructor
class PhysicsWorld{
  private:
    /* Map of fragments, it contains the Mesh instances associated to each
      piece */
    std::map<int, std::vector<Mesh*>>* fragmentMeshes;

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

    /* Limit ground rigid body */
    btCollisionShape* limitGroundShape;
    btDefaultMotionState* limitGroundMotionState;
    btRigidBody* limitGroundRigidBody;

    /* Piece rigid body (used for every standing pieces) */
    btCollisionShape* pieceShape;
    std::vector<btDefaultMotionState*> pieceMotionStates;
    btRigidBody* pieceRigidBodies[8][8];

    /* Inner clock for stepping simulation */
    sf::Clock* innerClock;

    /* Random generator for the fragments lifetime */
    std::default_random_engine generator;

  public:
    /* Constructor */
    explicit PhysicsWorld(
      std::map<int, std::vector<Mesh*>>* fragmentMeshes, ChessGame* game);

    /* Update a piece position when it's moving
      \param currentPosition The current position of the moving rigid body
      \param startPosition The start position of the movement
    */
    void updatePiecePosition(
      sf::Vector2i startPosition, sf::Vector2f currentPosition);

    /* Move a piece directly to its end position
      \param startPosition The start position of the movement
      \param endPosition The end position of the movement
    */
    void movePiece(
      sf::Vector2i startPosition, sf::Vector2i endPosition);

    /* Adds a piece to the dynamics world and collapse it
      \param piece Piece that you want to collapse: KING, QUEEN, BISHOP...
      \param position The position of the piece on the grid
    */
    void collapsePiece(int piece, sf::Vector2i position);

    /* Fragment pool, it contains each fragment actually in the dynamics world
      as a a pair of int (PAWN, KING...) and Fragment* */
    std::vector<std::pair<int, Fragment*>> fragmentPool;

    /* Simulate method, this will update the position of each fragment of the
      fragment pool and add new fragments when needed */
    void simulate();

    /* Destructor */
    ~PhysicsWorld();
};

#endif
