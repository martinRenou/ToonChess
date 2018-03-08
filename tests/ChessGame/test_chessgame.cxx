#include <gtest/gtest.h>

#include "../../src/ChessGame/ChessGame.hxx"
#include "../test_utils.hxx"


TEST(chess_game, initialization){
  MockPhysicsWorld physicsWorld;
  ChessGame* game = new ChessGame(&physicsWorld);
  game->start();

  EXPECT_EQ(game->oldSelectedPiecePosition.x, -1);
  EXPECT_EQ(game->oldSelectedPiecePosition.y, -1);

  EXPECT_EQ(game->selectedPiecePosition.x, -1);
  EXPECT_EQ(game->selectedPiecePosition.y, -1);

  // Perform nothing ! (nothing selected)
  game->perform();

  // Expected board after the move
  int expectedBoard[8][8] = {
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      EXPECT_EQ(game->board[x][y], expectedBoard[x][y]);

  delete game;
};

TEST(chess_game, unselect_piece){
  MockPhysicsWorld physicsWorld;
  ChessGame* game = new ChessGame(&physicsWorld);
  game->start();

  // Select PAWN (simulating click on a pawn)
  game->setNewSelectedPiecePosition({1, 1});

  // Select position where to move
  game->setNewSelectedPiecePosition({-1, -1});

  EXPECT_EQ(game->oldSelectedPiecePosition.x, 1);
  EXPECT_EQ(game->oldSelectedPiecePosition.y, 1);

  EXPECT_EQ(game->selectedPiecePosition.x, -1);
  EXPECT_EQ(game->selectedPiecePosition.y, -1);

  // Perform nothing ! (the piece has been unselected)
  game->perform();

  // Expected board after the move
  int expectedBoard[8][8] = {
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      EXPECT_EQ(game->board[x][y], expectedBoard[x][y]);

  delete game;
};

TEST(chess_game, pawn_move_1){
  MockPhysicsWorld physicsWorld;
  ChessGame* game = new ChessGame(&physicsWorld);
  game->start();

  // Select PAWN (simulating click on a pawn)
  game->setNewSelectedPiecePosition({1, 1});

  // Select position where to move
  game->setNewSelectedPiecePosition({1, 2});

  EXPECT_EQ(game->oldSelectedPiecePosition.x, 1);
  EXPECT_EQ(game->oldSelectedPiecePosition.y, 1);

  EXPECT_EQ(game->selectedPiecePosition.x, 1);
  EXPECT_EQ(game->selectedPiecePosition.y, 2);

  // Perform the move !
  game->perform();

  // Expected board after the move
  int expectedBoard[8][8] = {
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK},
    {KNIGHT, /**/EMPTY, PAWN/**/, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      EXPECT_EQ(game->board[x][y], expectedBoard[x][y]);

  delete game;
};

TEST(chess_game, pawn_move_2){
  MockPhysicsWorld physicsWorld;
  ChessGame* game = new ChessGame(&physicsWorld);
  game->start();

  // Select PAWN (simulating click on a pawn)
  game->setNewSelectedPiecePosition({1, 1});

  // Select position where to move
  game->setNewSelectedPiecePosition({1, 3});

  // Perform the move !
  game->perform();

  // Expected board after the move
  int expectedBoard[8][8] = {
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK},
    {KNIGHT, /**/EMPTY, EMPTY, PAWN/**/, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      EXPECT_EQ(game->board[x][y], expectedBoard[x][y]);

  delete game;
};

TEST(chess_game, pawn_forbiden_move){
  MockPhysicsWorld physicsWorld;
  ChessGame* game = new ChessGame(&physicsWorld);
  game->start();

  // Select PAWN (simulating click on a pawn)
  game->setNewSelectedPiecePosition({1, 1});

  // Select position where to move
  game->setNewSelectedPiecePosition({2, 3});

  // Perform nothing !
  game->perform();

  // Expected board after the move
  int expectedBoard[8][8] = {
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      EXPECT_EQ(game->board[x][y], expectedBoard[x][y]);

  delete game;
};

TEST(chess_game, knight_move_1){
  MockPhysicsWorld physicsWorld;
  ChessGame* game = new ChessGame(&physicsWorld);
  game->start();

  // Select KNIGHT (simulating click on a pawn)
  game->setNewSelectedPiecePosition({1, 0});

  // Select position where to move
  game->setNewSelectedPiecePosition({2, 2});

  // Perform the move !
  game->perform();

  // Expected board after the move
  int expectedBoard[8][8] = {
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK},
    {/**/EMPTY, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {BISHOP, PAWN, KNIGHT,/**/ EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      EXPECT_EQ(game->board[x][y], expectedBoard[x][y]);

  delete game;
};

TEST(chess_game, knight_move_2){
  MockPhysicsWorld physicsWorld;
  ChessGame* game = new ChessGame(&physicsWorld);
  game->start();

  // Select KNIGHT (simulating click on a pawn)
  game->setNewSelectedPiecePosition({1, 0});

  // Select position where to move
  game->setNewSelectedPiecePosition({0, 2});

  // Perform the move !
  game->perform();

  // Expected board after the move
  int expectedBoard[8][8] = {
    {ROOK, PAWN, KNIGHT,/**/ EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK},
    {/**/EMPTY, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      EXPECT_EQ(game->board[x][y], expectedBoard[x][y]);

  delete game;
};
