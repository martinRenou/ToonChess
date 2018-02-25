#include <map>
#include <vector>

#include "loadObjFile.hxx"
#include "Mesh.hxx"
#include "../constants.hxx"

#include "meshes.hxx"

std::map<int, Mesh*> initMeshes(){
  // Create and load meshes
  Mesh* king = loadObjFile("../assets/king.obj").at(0);
  std::vector<Mesh*> king_parts = loadObjFile("../assets/king_parts.obj");

  Mesh* queen = loadObjFile("../assets/queen.obj").at(0);
  std::vector<Mesh*> queen_parts = loadObjFile("../assets/queen_parts.obj");

  Mesh* bishop = loadObjFile("../assets/bishop.obj").at(0);
  std::vector<Mesh*> bishop_parts = loadObjFile("../assets/bishop_parts.obj");

  Mesh* rook = loadObjFile("../assets/rook.obj").at(0);
  std::vector<Mesh*> rook_parts = loadObjFile("../assets/rook_parts.obj");

  Mesh* knight = loadObjFile("../assets/knight.obj").at(0);
  std::vector<Mesh*> knight_parts = loadObjFile("../assets/knight_parts.obj");

  Mesh* pawn = loadObjFile("../assets/pawn.obj").at(0);
  std::vector<Mesh*> pawn_parts = loadObjFile("../assets/pawn_parts.obj");

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
