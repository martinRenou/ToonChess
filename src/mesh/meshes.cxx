#include "meshes.hxx"

#include "Mesh.hxx"
#include "../chessBoard/chessBoard.hxx"

#include <map>

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

  std::map<int, Mesh*> meshes = {
    {KING, king},
    {QUEEN, queen},
    {BISHOP, bishop},
    {ROOK, rook},
    {KNIGHT, knight},
    {PAWN, pawn},
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

    meshes->clear();
};
