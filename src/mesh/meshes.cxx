#include <map>

#include "Mesh.hxx"
#include "../constants.hxx"

#include "meshes.hxx"

std::map<int, Mesh*> initMeshes(){
  // Create and load meshes
  Mesh* king = new Mesh("../assets/king.obj");
  king->initBuffers();

  Mesh* queen = new Mesh("../assets/queen.obj");
  queen->initBuffers();

  Mesh* bishop = new Mesh("../assets/bishop.obj");
  bishop->initBuffers();

  Mesh* rook = new Mesh("../assets/rook.obj");
  rook->initBuffers();

  Mesh* knight = new Mesh("../assets/knight.obj");
  knight->initBuffers();

  Mesh* pawn = new Mesh("../assets/pawn.obj");
  pawn->initBuffers();

  Mesh* boardCell = new Mesh("../assets/boardCell.obj");
  boardCell->initBuffers();

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
