#include <gtest/gtest.h>

#include "../../src/ChessGame/ChessGame.hxx"

TEST(chess_game, initialization){
  ChessGame* game = new ChessGame();
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
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      EXPECT_EQ(game->board[x][y], expectedBoard[x][y]);

  delete game;
};

TEST(chess_game, move_piece){
  ChessGame* game = new ChessGame();
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
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
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
  ChessGame* game = new ChessGame();
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
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      EXPECT_EQ(game->board[x][y], expectedBoard[x][y]);

  delete game;
};

TEST(chess_game, throw_exception){
  ChessGame* game = new ChessGame();
  game->start();

  // Select PAWN (simulating click on a pawn)
  game->setNewSelectedPiecePosition({1, 1});

  // Select position where to move (forbiden move)
  game->setNewSelectedPiecePosition({2, 7});

  // Perform forbiden move !
  game->perform();

  // Wait for the WAITING state to stop
  sleep(2);

  // Transition to IA_TURN state !
  game->perform();

  // Now an exception should be raised (The IA should try to move on user's
  // piece)
  EXPECT_ANY_THROW(game->perform());

  delete game;
};
