#ifndef MESHES_HXX_
#define MESHES_HXX_

#include "Mesh.hxx"

#include <map>
#include <vector>

/* Load obj files and create piece meshes
  \return A map with index of the mesh as key and Mesh instances as value
*/
std::map<int, Mesh*> initPieces();

/* Delete piece meshes from the memory, should be called at the end of the
    program
  \param meshes The map of meshes created by initPieces
*/
void deletePieces(std::map<int, Mesh*>* pieces);

/* Load obj files and create fragment meshes
  \return A map with index of the mesh as key and Mesh instances as value
*/
std::map<int, std::vector<Mesh*>> initFragmentMeshes();

/* Delete fragment meshes from the memory, should be called at the end of the
    program
  \param meshes The map of meshes created by initFragments
*/
void deleteFragmentMeshes(std::map<int, std::vector<Mesh*>>* fragments);


#endif
