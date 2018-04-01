#include <vector>
#include <map>
#include <algorithm>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include <btBulletDynamicsCommon.h>

#include "../mesh/Mesh.hxx"
#include "../mesh/meshes.hxx"
#include "../Event/Event.hxx"
#include "../Event/EventStack.hxx"
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

  // Start the innerClock
  innerClock = new sf::Clock();
};

void PhysicsWorld::updatePiecePosition(
    sf::Vector2i startPosition, sf::Vector2f currentPosition){
  // Get rigidBody
  btRigidBody* movingRigidBody = pieceRigidBodies[
    startPosition.x][startPosition.y];

  // Move rigid body in the dynamics world
  btTransform transform(
    btQuaternion(0, 0, 0, 1),
    btVector3(
      currentPosition.x * 4 - 14,
      currentPosition.y * 4 - 14,
      3.75
    )
  );
  movingRigidBody->setWorldTransform(transform);
};

void PhysicsWorld::movePiece(
    sf::Vector2i startPosition, sf::Vector2i endPosition){
  // Move the piece to its end position
  updatePiecePosition(
    startPosition, {float(endPosition.x), float(endPosition.y)});

  // And update the rigid bodies grid
  btRigidBody* movingRigidBody = pieceRigidBodies[
    startPosition.x][startPosition.y];
  pieceRigidBodies[startPosition.x][startPosition.y] = NULL;
  pieceRigidBodies[endPosition.x][endPosition.y] = movingRigidBody;
};

void PhysicsWorld::collapsePiece(int piece, sf::Vector2i position){
  // Remove it from the dynamics world
  dynamicsWorld->removeRigidBody(pieceRigidBodies[position.x][position.y]);
  delete pieceRigidBodies[position.x][position.y];
  pieceRigidBodies[position.x][position.y] = NULL;

  // Create a fragment for each fragmentMesh of the piece
  int absPiece = abs(piece);
  for(unsigned int i = 0; i < fragmentMeshes->at(absPiece).size(); i++){
    // Create Fragment instance
    GLfloat rotation = piece > 0 ? -90 : 90;

    std::uniform_real_distribution<double> distribution(2.0, 3.0);
    GLfloat lifetime = distribution(generator);

    Fragment* fragment = new Fragment(
      fragmentMeshes->at(absPiece).at(i), position, rotation, lifetime);

    // Add it to the fragmentPool
    std::pair<int, Fragment*> pair(piece, fragment);
    fragmentPool.push_back(pair);

    // And add it to the world
    dynamicsWorld->addRigidBody(fragment->rigidBody);
  }
};

void PhysicsWorld::simulate(){
  float timeSinceLastCall = innerClock->getElapsedTime().asSeconds();

  // Take into account fragments lifetime
  for(unsigned int i = 0; i < fragmentPool.size(); i++){
    Fragment* fragment = fragmentPool.at(i).second;
    fragment->lifetime -= timeSinceLastCall;

    if(fragment->lifetime <= 0.0){
      // Generate smoke particles where the fragment disapeared
      btTransform trans;
      fragment->rigidBody->getMotionState()->getWorldTransform(trans);

      // Trigger fragment disappears event
      Event event;
      event.type = Event::FragmentDisappearsEvent;
      event.fragment.position = {
        trans.getOrigin().getX(),
        trans.getOrigin().getY(),
        trans.getOrigin().getZ()
      };
      event.fragment.volume = fragment->mass;
      event.fragment.piece = fragmentPool.at(i).first;
      EventStack::pushEvent(event);

      dynamicsWorld->removeRigidBody(fragment->rigidBody);
      delete fragment;
      fragmentPool.at(i).second = NULL;
    }
  }

  // Remove fragments which lifetime is over from the fragment pool
  fragmentPool.erase(
    std::remove_if(
      fragmentPool.begin(), fragmentPool.end(),
      [](std::pair<int, Fragment*> x){
        return x.second == NULL;
      }
    ),
    fragmentPool.end()
  );

  // Simulate the dynamics world
  dynamicsWorld->stepSimulation(timeSinceLastCall, 7);
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
