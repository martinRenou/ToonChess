#include <btShapeHull.h>
#include <btConvexHullShape.h>

#include "Fragment.hxx"

Fragment::Fragment(Mesh* mesh) : mesh{mesh}{
  // Create a simplified version of the original mesh for optimization purppose
  btConvexHullShape* originalConvexHullShape = new btConvexHullShape();
  for(unsigned int i = 0; i < mesh->vertices.size(); i++){
    originalConvexHullShape->addPoint(btVector3(
      mesh->vertices.at(i),
      mesh->vertices.at(i + 1),
      mesh->vertices.at(i + 2)
    ));
  }
  btShapeHull* hull = new btShapeHull(originalConvexHullShape);
  btScalar margin = originalConvexHullShape->getMargin();
  hull->buildHull(margin);
  convexHullShape = new btConvexHullShape(
    (btScalar*)hull->getVertexPointer(), hull->numVertices());

  // Compute inertia of the shape
  mass = 1;
  inertia = btVector3(0, 0, 0);
  convexHullShape->calculateLocalInertia(1, inertia);
}

Fragment::~Fragment(){
  delete convexHullShape;
}
