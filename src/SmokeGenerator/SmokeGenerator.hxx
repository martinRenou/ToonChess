#ifndef SMOKEGENERATOR_HXX_
#define SMOKEGENERATOR_HXX_

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

class SmokeGenerator {
private:
  /* Max number of particles */
  const int maxNbParticles = 50;

  /* Number of particles */
  int nbParticles = 0;

  /* ID of the vertex buffer */
  GLuint vertexBufferId;

  /* ID of the position/size buffer */
  GLuint positionSizeBufferId;

  /* Delete buffers from memory */
  void deleteBuffers();

public:
  /* Constructor */
  explicit SmokeGenerator();

  /* Initialization of the buffer objects */
  void initBuffers();

  /* Generate smoke at the given position
    \param position: Position where to generate the smoke
  */
  void generate(sf::Vector3f position);

  /* Draw smoke in the currently bound framebuffer object */
  void draw();

  /* Destructor, this will remove the buffers from memory */
  ~SmokeGenerator();
};

#endif
