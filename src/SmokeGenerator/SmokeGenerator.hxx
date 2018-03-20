#ifndef SMOKEGENERATOR_HXX_
#define SMOKEGENERATOR_HXX_

#include <vector>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include "../shader/ShaderProgram.hxx"
#include "../GameInfo.hxx"

struct SmokeParticle {
  sf::Vector3f speed;
  sf::Vector3f position;
  float size;

  float lifetime;
  float remainingLife;

  sf::Vector3f initialColor;

  float textureIndex;
};


// cppcheck-suppress noCopyConstructor
class SmokeGenerator {
private:
  /* Max number of particles */
  static const int maxNbParticles = 500;

  /* Current number of particles */
  int nbParticles = 0;

  /* ID of the vertex buffer */
  GLuint vertexBufferId;

  /* The vertex buffer (same for each smoke particle) */
  const GLfloat vertexBuffer[12] = {
    -0.5f, 0.0f, -0.5f,
    0.5f, 0.0f, -0.5f,
    -0.5f, 0.0f, 0.5f,
    0.5f, 0.0f, 0.5f,
  };

  /* ID of the position/size buffer */
  GLuint positionSizeBufferId;

  /* The buffer containing the positions and sizes of smoke particles */
  GLfloat positionSizeBuffer[4 * maxNbParticles];

  /* ID of the colorTexture buffer */
  GLuint colorTextureBufferId;

  /* The buffer containing the color and texture indices of smoke particles */
  GLfloat colorTextureBuffer[4 * maxNbParticles];

  /* The array of smoke particles */
  std::vector<SmokeParticle*> smokeParticles;

  /* Random generator for the lifetime, speed, position and size */
  std::default_random_engine generator;

  /* Inner clock for updating particles */
  sf::Clock* innerClock;

  /* Smoke texture */
  sf::Texture* smokeTexture0;
  sf::Texture* smokeTexture1;
  sf::Texture* smokeTexture2;

  /* The shader program for displaying smoke */
  ShaderProgram* smokeShaderProgram;

public:
  /* Constructor */
  explicit SmokeGenerator();

  /* Initialization of the buffer objects */
  void initBuffers();

  /* Generate smoke at the given position
    \param position: Position where to generate the smoke
    \param numberParticles: Number of smoke particles to create
  */
  void generate(sf::Vector3f position, int numberParticles, sf::Vector3f color);

  /* Draw smoke in the currently bound framebuffer object */
  void draw(GameInfo* gameInfo);

  /* Destructor, this will remove the buffers from memory */
  ~SmokeGenerator();
};

#endif
