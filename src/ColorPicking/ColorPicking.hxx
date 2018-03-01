#ifndef COLORPICKING_HXX_
#define COLORPICKING_HXX_

#include <GL/gl.h>
#include <SFML/Graphics.hpp>

#include <map>

#include "../mesh/Mesh.hxx"
#include "../shader/ShaderProgram.hxx"
#include "../GameInfo.hxx"
#include "../ChessGame/ChessGame.hxx"

class ColorPicking {
private:
  /* The identifier of the framebuffer object */
  GLuint fboId;

  /* The identifier of the renderbuffer for color */
  GLuint colorRenderBufferId;

  /* The identifier of the renderbuffer for depth */
  GLuint depthRenderBufferId;

  /* The width of the renderbuffer */
  GLuint width;

  /* The height of the renderbuffer */
  GLuint height;

  /* Delete buffers from memory */
  void deleteBuffers();

public:
  /* Constructor
    \param width The screen width
    \param height The screen height
  */
  ColorPicking(GLuint width, GLuint height);

  /* Initialization of the buffer objects */
  void initBuffers();

  /* Function that must be called when resizing the screen
    \param width The screen width
    \param height The screen height
  */
  void resizeBuffers(GLuint newWidth, GLuint newHeight);

  /* Get the cliked piece position according to the clicked pixel on the screen.
    This will perform a rendering in the framebuffer
    \param clickedPixelPosition The position of the clicked pixel on the screen
    \param game The game instance
    \param gameInfo The current game informations
    \param meshes The map of piece meshes
    \param programs The map of shader programs
    \return The position of the clicked chess piece
  */
  sf::Vector2i getClickedPiecePosition(
    sf::Vector2i clickedPixelPosition,
    ChessGame* game,
    GameInfo* gameInfo, std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs);

  /* Destructor, this will remove the buffers from memory */
  ~ColorPicking();
};

#endif
