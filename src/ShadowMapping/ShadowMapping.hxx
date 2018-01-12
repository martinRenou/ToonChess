#ifndef SHADOWMAPPING_HXX_
#define SHADOWMAPPING_HXX_

#include <GL/gl.h>

#include <map>

#include "../mesh/Mesh.hxx"
#include "../shader/ShaderProgram.hxx"
#include "../GameInfo.hxx"

class ShadowMapping {
private:
  /* The identifier of the framebuffer object */
  GLuint fboId;

  /* The identifier of the shadow map texture */
  GLuint shadowMapId;

  /* The identifier of the renderbuffer for depth */
  GLuint depthRenderBufferId;

  /* The resolution of the shadow map */
  GLuint resolution;

  /* Delete buffers from memory */
  void deleteBuffers();

public:
  /* Constructor
    \param resolution The resolution of the shadow map
  */
  ShadowMapping(GLuint resolution);

  /* Initialization of the buffer objects */
  void initBuffers();

  /* Render and return the shadow map id
    \param gameInfo The current game informations
    \param meshes The map of piece meshes
    \param programs The map of shader programs
    \return The id of the shadowMap
  */
  GLuint getShadowMap(
    GameInfo* gameInfo, std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs);

  /* Destructor, this will remove the buffers from memory */
  ~ShadowMapping();
};

#endif
