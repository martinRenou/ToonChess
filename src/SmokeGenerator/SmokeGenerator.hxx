#ifndef SMOKEGENERATOR_HXX_
#define SMOKEGENERATOR_HXX_

#include <vector>
#include <random>

#include <GLFW/glfw3.h>

#include "../shader/ShaderProgram.hxx"
#include "../Camera/Camera.hxx"
#include "../Clock/Clock.hxx"

struct SmokeParticle {
  Vector3f speed;
  Vector3f position;
  float size;

  float lifetime;
  float remainingLife;

  Vector3f initialColor;

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
  Clock* innerClock;

  /* Smoke textures */
  GLuint smokeTexture0;
  GLuint smokeTexture1;
  GLuint smokeTexture2;

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
    \param color: Color of particles
    \param sizeFactor: Size factor of particles
  */
  void generate(
    Vector3f position,
    int numberParticles,
    Vector4f color,
    float sizeFactor);

  /* Draw smoke in the currently bound framebuffer object */
  void draw(Camera* camera);

  /* Destructor, this will remove the buffers from memory */
  ~SmokeGenerator();
};

#endif
