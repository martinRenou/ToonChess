#ifndef PHYSICSWORLD_HXX_
#define PHYSICSWORLD_HXX_

#include <vector>
#include <map>

#include <GL/gl.h>

#include "../mesh/Mesh.hxx"


class PhysicsWorld {
  private:
    /* Map of fragments, it contains the Mesh instances created from obj files
      "king_fragments.obj", "queen_fragments.obj", ...*/
    std::map<int, std::vector<Mesh*>> fragments;

  public:
    /* Constructor */
    PhysicsWorld();

    /* Fragment pool, it contains each fragment actually in the dynamic world as
      a Mesh*, and its placement as a movement matrix */
    std::vector<Mesh*, std::vector<GLfloat>> fragmentPool;

    /* Simulate method, this will update the position of each fragment of the
      fragment pool */
    void simulate();

    /* Destructor */
    ~PhysicsWorld();
};

#endif
