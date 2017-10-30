#ifndef MESH_HXX_
#define MESH_HXX_

#include <GL/gl.h>

#include <vector>
#include <string>

/* Extract a vector of floats from a line of the obj file
  \param line The line to extract in the following format:
    ["v", "0.5", "1.0", "9.3"]
  \param vector The vector in which to push the extracted 3D vector
*/
void extractFloatVec3(
    std::vector<std::string> *line, std::vector<GLfloat> *vector);

/* Extract vertices from a line representing a face in the obj file
  \param line The line in the following format:
    ["f", "56//32", "12//33", "44//32"]
  \param unsortedVertices The list of vertices coordinates already extracted
    from the file
  \param vertices The vector of vertices in which to put the extracted vertices.
    In this example we will extract the coordinates of the 55th, 11th and 43th
    vertices coming from unsortedVertices
*/
void extractVertices(
    std::vector<std::string> *line,
    std::vector<GLfloat> *unsortedVertices,
    std::vector<GLfloat> *vertices);

/* Extract normals from a line representing a face in the obj file
  \param line The line in the following format:
    ["f", "56//32", "12//33", "44//32"]
  \param unsortedNormals The list of normals coordinates already extracted
    from the file
  \param normals The vector of normals in which to put the extracted normals.
    In this example we will extract the coordinates of the 31th, 32th and 31th
    normals coming from unsortedNormals
*/
void extractNormals(
    std::vector<std::string> *line,
    std::vector<GLfloat> *unsortedNormals,
    std::vector<GLfloat> *normals);

class Mesh {
  public:
    /* The path of the .obj file containing the mesh */
    std::string filePath;

    /* Vector of vertices defining the mesh */
    std::vector<GLfloat> vertices;
    /* Vector of normals defining the mesh */
    std::vector<GLfloat> normals;
    /* Vector of indices defining the faces of the mesh */
    std::vector<GLuint> indices;

    /* ID of the vertex buffer */
    GLuint vertexBufferId;
    /* ID of the normal buffer */
    GLuint normalBufferId;
    /* ID of the indices buffer */
    GLuint indexBufferId;

    /* Constructor
      \param filePath The path of the .obj file containing the mesh
    */
    Mesh(std::string filePath);

    /* Initialization of the buffer objects */
    void initBuffers();

    /* Draw the mesh in the 3D scene */
    void draw();

    /* Destructor, this will remove the buffers from memory */
    ~Mesh();
};

#endif
