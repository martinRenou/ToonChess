#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>

#include "Fragment.hxx"

Fragment::Fragment(Mesh* mesh) : mesh{mesh}{
  // Create a simplified version of the original mesh for optimization purppose
  btConvexHullShape* originalConvexHullShape = new btConvexHullShape();
  for(unsigned int i = 0; i < mesh->vertices.size() / 3; i ++){
    originalConvexHullShape->addPoint(btVector3(
      mesh->vertices.at(3 * i),
      mesh->vertices.at(3 * i + 1),
      mesh->vertices.at(3 * i + 2)
    ));
  }
  hull = new btShapeHull(originalConvexHullShape);
  btScalar margin = originalConvexHullShape->getMargin();
  hull->buildHull(margin);
  convexHullShape = new btConvexHullShape(
    (btScalar*)hull->getVertexPointer(), hull->numVertices());
  delete originalConvexHullShape;

  // Compute inertia of the shape
  mass = 1;
  inertia = btVector3(0, 0, 0);
  convexHullShape->calculateLocalInertia(1, inertia);
}

Fragment::~Fragment(){
  delete convexHullShape;
  delete hull;
}
