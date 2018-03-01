#include <vector>
#include <map>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include <btBulletDynamicsCommon.h>

#include "../mesh/Mesh.hxx"
#include "../mesh/meshes.hxx"
#include "../constants.hxx"

#include "PhysicsWorld.hxx"

PhysicsWorld::PhysicsWorld(){
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

  // Create the ground as a static plane with equation:
  // 0*X + 0*Y + 1*Z + 0 = 0 => Z = 0
  groundShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);
  groundMotionState = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
    0, groundMotionState, groundShape, btVector3(0, 0, 0));
  groundRigidBody = new btRigidBody(groundRigidBodyCI);
  dynamicsWorld->addRigidBody(groundRigidBody);

  // Load fragment meshes and initialize fragment of each chess piece
  fragmentMeshes = initFragmentMeshes();

  fragments.insert(std::pair<int, std::vector<Fragment*>>(
    KING, initFragments(KING)));
  fragments.insert(std::pair<int, std::vector<Fragment*>>(
    QUEEN, initFragments(QUEEN)));
  fragments.insert(std::pair<int, std::vector<Fragment*>>(
    BISHOP, initFragments(BISHOP)));
  fragments.insert(std::pair<int, std::vector<Fragment*>>(
    KNIGHT, initFragments(KNIGHT)));
  fragments.insert(std::pair<int, std::vector<Fragment*>>(
    ROOK, initFragments(ROOK)));
  fragments.insert(std::pair<int, std::vector<Fragment*>>(
    PAWN, initFragments(PAWN)));
};

std::vector<Fragment*> PhysicsWorld::initFragments(int piece){
  std::vector<Fragment*> pieceFragments;

  // Create a Fragment instance for each Mesh instance
  for(unsigned int i = 0; i < fragmentMeshes.at(piece).size(); i++){
    pieceFragments.push_back(new Fragment(fragmentMeshes.at(piece).at(i)));
  }

  return pieceFragments;
};

void PhysicsWorld::deleteFragments(int piece){
  if(fragments.at(piece).size() != 0)
    for(unsigned int i = fragments.at(piece).size() - 1; i > 0; i--)
      delete fragments.at(piece).at(i);
};

void PhysicsWorld::collapsePiece(int piece, sf::Vector2i position){
  // Create a rigid body for each fragment associated to the piece
  for(unsigned int i = 0; i < fragments.at(piece).size(); i++){
    btDefaultMotionState* fragmentMotionState = new btDefaultMotionState(
      btTransform(btQuaternion(0, 0, 0, 1),
      btVector3(position.x * 4 - 14, position.y * 4 - 14, 0))
    );

    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(
      fragments.at(piece).at(i)->mass,
      fragmentMotionState,
      fragments.at(piece).at(i)->convexHullShape,
      fragments.at(piece).at(i)->inertia
    );
    btRigidBody* fragmentRigidBody = new btRigidBody(fallRigidBodyCI);

    // Add it to the fragmentPool
    fragmentPool.push_back(fragmentRigidBody);

    // And add it to the world
    dynamicsWorld->addRigidBody(fragmentRigidBody);
  }
};

void PhysicsWorld::simulate(){
  // Simulate the dynamics world
  //TODO: Clock that computes the time since the last simlation call
  //TODO: Check what is needed for the second argument
  dynamicsWorld->stepSimulation(1 / 60.f, 10);
};

PhysicsWorld::~PhysicsWorld(){
  // Delete fragments
  deleteFragmentMeshes(&fragmentMeshes);

  // Delete fragments
  deleteFragments(KING);
  deleteFragments(QUEEN);
  deleteFragments(BISHOP);
  deleteFragments(KNIGHT);
  deleteFragments(ROOK);
  deleteFragments(PAWN);

  // Delete rigid bodies
  if(fragmentPool.size() != 0)
    for(unsigned int i = fragmentPool.size() - 1; i > 0; i--){
      delete fragmentPool.at(i)->getMotionState();
      delete fragmentPool.at(i);
    }

  // Delete ground
  delete groundShape;
  delete groundMotionState;
  delete groundRigidBody;

  // Delete dynamics world
  delete dynamicsWorld;
  delete solver;
  delete dispatcher;
  delete collisionConfiguration;
  delete broadphase;
}
