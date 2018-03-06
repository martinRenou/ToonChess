#include <vector>
#include <map>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include <btBulletDynamicsCommon.h>

#include "../mesh/Mesh.hxx"
#include "../mesh/meshes.hxx"
#include "../constants.hxx"

#include "PhysicsWorld.hxx"

PhysicsWorld::PhysicsWorld(std::map<int, std::vector<Mesh*>>* fragmentMeshes)
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

  // Create the ground as a static plane with equation:
  // 0*X + 0*Y + 1*Z + 0 = 0 => Z = 0
  groundShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);
  groundMotionState = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
    0, groundMotionState, groundShape, btVector3(0, 0, 0));
  groundRigidBody = new btRigidBody(groundRigidBodyCI);
  dynamicsWorld->addRigidBody(groundRigidBody);
};

void PhysicsWorld::collapsePiece(int piece, sf::Vector2i position){
  // Create a fragment for each fragmentMesh of the piece
  int absPiece = abs(piece);
  for(unsigned int i = 0; i < fragmentMeshes->at(absPiece).size(); i++){
    // Create Fragment instance
    Fragment* fragment = new Fragment(
      fragmentMeshes->at(absPiece).at(i), position);

    // Add it to the fragmentPool
    std::pair<int, Fragment*> pair(piece, fragment);
    fragmentPool.push_back(pair);

    // And add it to the world
    dynamicsWorld->addRigidBody(fragment->rigidBody);
  }
};

void PhysicsWorld::simulate(){
  // Simulate the dynamics world
  //TODO: Clock that computes the time since the last simlation call
  //TODO: Check what is needed for the second argument
  dynamicsWorld->stepSimulation(1 / 60.f, 10);
};

PhysicsWorld::~PhysicsWorld(){
  // Delete rigid bodies
  if(fragmentPool.size() != 0)
    for(unsigned int i = fragmentPool.size() - 1; i > 0; i--){
      delete fragmentPool.at(i).second;
    }
  fragmentPool.clear();

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
