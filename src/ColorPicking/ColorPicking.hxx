#ifndef COLORPICKING_HXX_
#define COLORPICKING_HXX_

#include <GLFW/glfw3.h>

#include <map>
#include <cmath>

#include "../Camera/Camera.hxx"
#include "../utils/math.hxx"
#include "../mesh/Mesh.hxx"
#include "../shader/ShaderProgram.hxx"
#include "../ChessGame/ChessGame.hxx"

class ColorPicking
{
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
        \param meshes The map of piece meshes
        \param programs The map of shader programs
        \param camera The camera
        \return The position of the clicked chess piece
    */
    Vector2i getClickedPiecePosition(
        const Vector2i& clickedPixelPosition,
        const ChessGame& game,
        const std::map<int, Mesh>& meshes,
        const std::map<int, ShaderProgram>& programs,
        const Camera& camera, float elapsedTime);

    /* Destructor, this will remove the buffers from memory */
    ~ColorPicking();

private:

    /* The identifier of the framebuffer object */
    GLuint fboId;

    /* The identifier of the renderbuffer for color */
    GLuint colorRenderBufferId;

    /* The identifier of the renderbuffer for depth */
    GLuint depthRenderBufferId;

    /* Size of the renderbuffer */
    GLuint width;
    GLuint height;

    /* Delete buffers from memory */
    void deleteBuffers();
};

#endif
