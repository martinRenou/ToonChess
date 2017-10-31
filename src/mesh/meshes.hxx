#ifndef MESHES_HXX_
#define MESHES_HXX_

#include "Mesh.hxx"

#include <map>

const int EMPTY = 0;
const int KING = 1;
const int QUEEN = 2;
const int BISHOP = 3;
const int KNIGHT = 4;
const int ROOK = 5;
const int PAWN = 6;

const int USER = 1;
const int AI = -1;

/* Load obj files and create meshes
  \return A map with index of the mesh as key and Mesh instances as value
*/
std::map<int, Mesh*> initMeshes();

/* Delete meshes from the memory, should be called at the end of the program
  \param meshes The map of meshes created by initMeshes
*/
void deleteMeshes(std::map<int, Mesh*>* meshes);

#endif
