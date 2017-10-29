#include <gtest/gtest.h>

#include "../../src/chessBoard/chessBoard.hxx"

TEST(uciFormatToPosition, can_convert){
  std::string in = "a1";
  std::vector<int> out = uciFormatToPosition(in);
  EXPECT_EQ(0, out[0]);
  EXPECT_EQ(0, out[1]);

  in = "b2";
  out = uciFormatToPosition(in);
  EXPECT_EQ(1, out[0]);
  EXPECT_EQ(1, out[1]);

  in = "e3";
  out = uciFormatToPosition(in);
  EXPECT_EQ(4, out[0]);
  EXPECT_EQ(2, out[1]);

  in = "h8";
  out = uciFormatToPosition(in);
  EXPECT_EQ(7, out[0]);
  EXPECT_EQ(7, out[1]);

  in = "h1";
  out = uciFormatToPosition(in);
  EXPECT_EQ(7, out[0]);
  EXPECT_EQ(0, out[1]);
}

TEST(positionToUciFormat, can_convert){
  std::string out = "a1";
  EXPECT_EQ(out, positionToUciFormat(0, 0));

  out = "b2";
  EXPECT_EQ(out, positionToUciFormat(1, 1));

  out = "e3";
  EXPECT_EQ(out, positionToUciFormat(4, 2));

  out = "h8";
  EXPECT_EQ(out, positionToUciFormat(7, 7));

  out = "h1";
  EXPECT_EQ(out, positionToUciFormat(7, 0));
}
