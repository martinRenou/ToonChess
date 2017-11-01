#ifndef MESHES_HXX_
#define MESHES_HXX_

#include "Mesh.hxx"

#include <map>

/* Load obj files and create meshes
  \return A map with index of the mesh as key and Mesh instances as value
*/
std::map<int, Mesh*> initMeshes();

/* Delete meshes from the memory, should be called at the end of the program
  \param meshes The map of meshes created by initMeshes
*/
void deleteMeshes(std::map<int, Mesh*>* meshes);

#endif
