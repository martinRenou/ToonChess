#include <map>
#include <vector>
#include <string>

#include "loadObjFile.hxx"
#include "Mesh.hxx"
#include "../constants.hxx"
#include "../get_share_path.hxx"

#include "meshes.hxx"

std::map<int, Mesh*> initPieces(){
  std::string share_path = get_share_path();

  // Create and load meshes
  Mesh* king = loadObjFile(share_path + "assets/king.obj").at(0);
  Mesh* queen = loadObjFile(share_path + "assets/queen.obj").at(0);
  Mesh* bishop = loadObjFile(share_path + "assets/bishop.obj").at(0);
  Mesh* rook = loadObjFile(share_path + "assets/rook.obj").at(0);
  Mesh* knight = loadObjFile(share_path + "assets/knight.obj").at(0);
  Mesh* pawn = loadObjFile(share_path + "assets/pawn.obj").at(0);
  Mesh* boardCell = loadObjFile(share_path + "assets/boardCell.obj").at(0);

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

std::map<int, std::vector<Mesh*>> initFragmentMeshes(){
  std::string share_path = get_share_path();

  // Create and load meshes
  std::vector<Mesh*> king_fragments = loadObjFile(share_path + "assets/king_fragments.cobj");
  std::vector<Mesh*> queen_fragments = \
    loadObjFile(share_path + "assets/queen_fragments.cobj");
  std::vector<Mesh*> bishop_fragments = \
    loadObjFile(share_path + "assets/bishop_fragments.cobj");
  std::vector<Mesh*> rook_fragments = \
    loadObjFile(share_path + "assets/rook_fragments.cobj");
  std::vector<Mesh*> knight_fragments = \
    loadObjFile(share_path + "assets/knight_fragments.cobj");
  std::vector<Mesh*> pawn_fragments = \
    loadObjFile(share_path + "assets/pawn_fragments.cobj");

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

void _deleteFragmentMeshes(std::vector<Mesh*>* fragments){
  for(unsigned int i = 0; i < fragments->size(); i++)
    delete fragments->at(i);
}

void deleteFragmentMeshes(std::map<int, std::vector<Mesh*>>* fragments){
  _deleteFragmentMeshes(&fragments->at(KING));
  _deleteFragmentMeshes(&fragments->at(QUEEN));
  _deleteFragmentMeshes(&fragments->at(BISHOP));
  _deleteFragmentMeshes(&fragments->at(ROOK));
  _deleteFragmentMeshes(&fragments->at(KNIGHT));
  _deleteFragmentMeshes(&fragments->at(PAWN));

  fragments->clear();
};
