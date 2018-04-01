#ifndef SHADOWMAPPING_HXX_
#define SHADOWMAPPING_HXX_

#include <GL/gl.h>

#include <map>

#include "../Camera/Camera.hxx"
#include "../DirectionalLight.hxx"
#include "../mesh/Mesh.hxx"
#include "../shader/ShaderProgram.hxx"
#include "../ChessGame/ChessGame.hxx"

class ShadowMapping {
private:
  /* The identifier of the framebuffer object */
  GLuint fboId;

  /* The identifier of the shadow map texture */
  GLuint shadowMapId;

  /* The identifier of the renderbuffer for depth */
  GLuint depthRenderBufferId;

  /* Delete buffers from memory */
  void deleteBuffers();

public:
  /* Constructor */
  explicit ShadowMapping();

  /* Initialization of the buffer objects */
  void initBuffers();

  /* The resolution of the shadow map */
  GLuint resolution = SHADOWMAPPING_LOW;

  /* Render shadow map
    \param game The game instance
    \param meshes The map of piece meshes
    \param light The light
  */
  void renderShadowMap(
    ChessGame* game, std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    DirectionalLight* light, float elapsedTime);

  /* Get shadow map id
    \return The shadow map id
  */
  GLuint getShadowMap();

  /* Destructor, this will remove the buffers from memory */
  ~ShadowMapping();
};

#endif
