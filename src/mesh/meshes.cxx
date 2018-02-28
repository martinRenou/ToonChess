#include <map>
#include <vector>

#include "loadObjFile.hxx"
#include "Mesh.hxx"
#include "../constants.hxx"

#include "meshes.hxx"

std::map<int, Mesh*> initPieces(){
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

void deletePieces(std::map<int, Mesh*>* meshes){
  delete meshes->at(KING);
  delete meshes->at(QUEEN);
  delete meshes->at(BISHOP);
  delete meshes->at(ROOK);
  delete meshes->at(KNIGHT);
  delete meshes->at(PAWN);
  delete meshes->at(BOARDCELL);

  meshes->clear();
};

std::map<int, std::vector<Mesh*>> initFragments(){
  // Create and load meshes
  std::vector<Mesh*> king_fragments = loadObjFile("../assets/king_fragments.obj");
  std::vector<Mesh*> queen_fragments = \
    loadObjFile("../assets/queen_fragments.obj");
  std::vector<Mesh*> bishop_fragments = \
    loadObjFile("../assets/bishop_fragments.obj");
  std::vector<Mesh*> rook_fragments = \
    loadObjFile("../assets/rook_fragments.obj");
  std::vector<Mesh*> knight_fragments = \
    loadObjFile("../assets/knight_fragments.obj");
  std::vector<Mesh*> pawn_fragments = \
    loadObjFile("../assets/pawn_fragments.obj");

  std::map<int, std::vector<Mesh*>> meshes = {
    {KING, king_fragments},
    {QUEEN, queen_fragments},
    {BISHOP, bishop_fragments},
    {ROOK, rook_fragments},
    {KNIGHT, knight_fragments},
    {PAWN, pawn_fragments},
  };

  return meshes;
};

void _deleteFragments(std::vector<Mesh*>* fragments){
  if(fragments->size() != 0)
    for(unsigned int i = fragments->size() - 1; i > 0; i--)
      delete fragments->at(i);
}

void deleteFragments(std::map<int, std::vector<Mesh*>>* fragments){
  _deleteFragments(&fragments->at(KING));
  _deleteFragments(&fragments->at(QUEEN));
  _deleteFragments(&fragments->at(BISHOP));
  _deleteFragments(&fragments->at(ROOK));
  _deleteFragments(&fragments->at(KNIGHT));
  _deleteFragments(&fragments->at(PAWN));

  fragments->clear();
};
