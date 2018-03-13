#include <vector>
#include <map>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include <btBulletDynamicsCommon.h>

#include "../mesh/Mesh.hxx"
#include "../mesh/meshes.hxx"
#include "../constants.hxx"

#include "PhysicsWorld.hxx"

PhysicsWorld::PhysicsWorld(
    std::map<int, std::vector<Mesh*>>* fragmentMeshes, ChessGame* game)
    : fragmentMeshes{fragmentMeshes}{
  // Create dynamics world
  broadphase = new btDbvtBroadphase();
  collisionConfiguration = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collisionConfiguration);
  solver = new btSequentialImpulseConstraintSolver;
  dynamicsWorld = new btDiscreteDynamicsWorld(
    dispatcher,
    broadphase,
    solver,
    collisionConfiguration
  );

  // Set gravity
  dynamicsWorld->setGravity(btVector3(0, 0, -9.81));

  // Create the ground as a box:
  groundShape = new btBoxShape(btVector3(16, 16, 0.2));
  groundMotionState = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -0.2)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
    0, groundMotionState, groundShape, btVector3(0, 0, 0));
  groundRigidBody = new btRigidBody(groundRigidBodyCI);
  dynamicsWorld->addRigidBody(groundRigidBody);

  // Create another ground as a 2D plane:
  limitGroundShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);
  limitGroundMotionState = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -40)));
  btRigidBody::btRigidBodyConstructionInfo limitGroundRigidBodyCI(
    0, limitGroundMotionState, limitGroundShape, btVector3(0, 0, 0));
  limitGroundRigidBody = new btRigidBody(limitGroundRigidBodyCI);
  dynamicsWorld->addRigidBody(limitGroundRigidBody);

  // Create a cylinder rigid body for each piece on the board
  pieceShape = new btCylinderShapeZ(btVector3(1.6, 1.6, 7.5));
  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      if(game->boardAt(x, y) != EMPTY){
        btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
          btQuaternion(0, 0, 0, 1),
          btVector3(x * 4 - 14, y * 4 - 14, 3.75)
        ));

        pieceMotionStates.push_back(motionState);

        btRigidBody::btRigidBodyConstructionInfo pieceRigidBodyCI(
          0, motionState, pieceShape, btVector3(0, 0, 0));
        pieceRigidBodies[x][y] = new btRigidBody(pieceRigidBodyCI);
        dynamicsWorld->addRigidBody(pieceRigidBodies[x][y]);
      } else {
        pieceRigidBodies[x][y] = NULL;
      }
    }
  }
  movingRigidBody = NULL;

  // Start the innerClock
  innerClock = new sf::Clock();
};

void PhysicsWorld::collapsePiece(int piece, sf::Vector2i position){
  // Create a fragment for each fragmentMesh of the piece
  int absPiece = abs(piece);
  for(unsigned int i = 0; i < fragmentMeshes->at(absPiece).size(); i++){
    // Create Fragment instance
    GLfloat rotation = piece > 0 ? -90 : 90;
    Fragment* fragment = new Fragment(
      fragmentMeshes->at(absPiece).at(i), position, rotation);

    // Add it to the fragmentPool
    std::pair<int, Fragment*> pair(piece, fragment);
    fragmentPool.push_back(pair);

    // And add it to the world
    dynamicsWorld->addRigidBody(fragment->rigidBody);
  }
};

void PhysicsWorld::simulate(ChessGame* game){
  // If a piece is moving on the board, move its rigid body in the dynamics world
  if(game->movingPiece != EMPTY){
    sf::Vector2i startPosition = game->movingPieceStartPosition;
    movingRigidBodyEndPosition = {
      game->movingPieceEndPosition.x, game->movingPieceEndPosition.y};

    // Get the rigid body of the moving piece
    if(!movingRigidBody){
      movingRigidBody = pieceRigidBodies[startPosition.x][startPosition.y];
      pieceRigidBodies[startPosition.x][startPosition.y] = NULL;
    }

    // Move rigid body in the dynamics world
    btTransform transform(
      btQuaternion(0, 0, 0, 1),
      btVector3(
        game->movingPiecePosition.x * 4 - 14,
        game->movingPiecePosition.y * 4 - 14,
        3.75
      )
    );
    movingRigidBody->setWorldTransform(transform);

    int pieceAtEndPosition = game->boardAt(
      movingRigidBodyEndPosition.x, movingRigidBodyEndPosition.y);

    // It the piece is taking another one, remove the last and collapse it
    if(pieceAtEndPosition != EMPTY and pieceAtEndPosition != OUT_OF_BOUND){
      // Remove it from the dynamics world
      dynamicsWorld->removeRigidBody(
        pieceRigidBodies[movingRigidBodyEndPosition.x]
                        [movingRigidBodyEndPosition.y]
      );
      delete pieceRigidBodies[movingRigidBodyEndPosition.x]
                             [movingRigidBodyEndPosition.y];
      pieceRigidBodies[movingRigidBodyEndPosition.x]
                      [movingRigidBodyEndPosition.y] = NULL;

      // Remove it from the board
      game->board[movingRigidBodyEndPosition.x][movingRigidBodyEndPosition.y] = EMPTY;

      // Collapse it
      collapsePiece(pieceAtEndPosition, game->movingPieceEndPosition);
    }
  }
  else if(movingRigidBody){
    pieceRigidBodies[movingRigidBodyEndPosition.x][movingRigidBodyEndPosition.y] = movingRigidBody;

    // Move the rigid body to its final position
    btTransform transform(
      btQuaternion(0, 0, 0, 1),
      btVector3(
        movingRigidBodyEndPosition.x * 4 - 14,
        movingRigidBodyEndPosition.y * 4 - 14,
        3.75
      )
    );
    movingRigidBody->setWorldTransform(transform);

    // And stop moving
    movingRigidBody = NULL;
  }

  // Simulate the dynamics world
  dynamicsWorld->stepSimulation(innerClock->getElapsedTime().asSeconds(), 7);
  innerClock->restart();
};

PhysicsWorld::~PhysicsWorld(){
  // Delete rigid bodies
  for(unsigned int i = 0; i < fragmentPool.size(); i++)
    delete fragmentPool.at(i).second;
  fragmentPool.clear();

  // Delete ground
  delete groundShape;
  delete groundMotionState;
  delete groundRigidBody;

  // Delete cylinders
  delete pieceShape;
  for(unsigned int i = 0; i < pieceMotionStates.size(); i++)
    delete pieceMotionStates.at(i);
  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      if(pieceRigidBodies[x][y]) delete pieceRigidBodies[x][y];

  // Delete dynamics world
  delete dynamicsWorld;
  delete solver;
  delete dispatcher;
  delete collisionConfiguration;
  delete broadphase;

  // Delete clock
  delete innerClock;
}
