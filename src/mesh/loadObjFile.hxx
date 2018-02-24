#ifndef LOADMESHES_HXX_
#define LOADMESHES_HXX_

#include <vector>
#include <string>

#include "Mesh.hxx"

/* Extract a vector of floats from a line of the obj file
  \param line The line to extract in the following format:
    ["v", "0.5", "1.0", "9.3"]
  \param vector The vector in which to push the extracted 3D vector
*/
void extractFloatVec3(std::vector<std::string> *line, std::vector<GLfloat> *vector);

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

/* Extract a list of meshes from an obj file
  \param filePath The path to the file to be loaded
  \return The list of Mesh instances as a vector<Mesh*>
*/
std::vector<Mesh *> loadObjFile(const std::string& filePath);

#endif
