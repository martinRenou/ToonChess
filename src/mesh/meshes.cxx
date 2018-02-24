#include <map>

#include "loadObjFile.hxx"
#include "Mesh.hxx"
#include "../constants.hxx"

#include "meshes.hxx"

std::map<int, Mesh*> initMeshes(){
  // Create and load meshes
  Mesh* king = loadObjFile("../assets/king.obj").at(0);

  Mesh* queen = loadObjFile("../assets/queen.obj").at(0);

  Mesh* bishop = loadObjFile("../assets/bishop.obj").at(0);

  Mesh* rook = loadObjFile("../assets/rook.obj").at(0);

  Mesh* knight = loadObjFile("../assets/knight.obj").at(0);

  Mesh* pawn = loadObjFile("../assets/pawn.obj").at(0);

  Mesh* boardCell = loadObjFile("../assets/boardCell.obj").at(0);

  std::map<int, Mesh*> meshes = {
    {KING, king},
    {QUEEN, queen},
    {BISHOP, bishop},
    {ROOK, rook},
    {KNIGHT, knight},
    {PAWN, pawn},
    {BOARDCELL, boardCell},
  };

  return meshes;
};

void deleteMeshes(std::map<int, Mesh*>* meshes){
    delete meshes->at(KING);
    delete meshes->at(QUEEN);
    delete meshes->at(BISHOP);
    delete meshes->at(ROOK);
    delete meshes->at(KNIGHT);
    delete meshes->at(PAWN);
    delete meshes->at(BOARDCELL);

    meshes->clear();
};
