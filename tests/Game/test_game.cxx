#include <gtest/gtest.h>

#include <SFML/Graphics.hpp>

#include "../../src/Game/Game.hxx"

TEST(uciFormatToPosition, can_convert){
  std::string in = "a1";
  sf::Vector2i out = uciFormatToPosition(in);
  EXPECT_EQ(0, out.x);
  EXPECT_EQ(0, out.y);

  in = "b2";
  out = uciFormatToPosition(in);
  EXPECT_EQ(1, out.x);
  EXPECT_EQ(1, out.y);

  in = "e3";
  out = uciFormatToPosition(in);
  EXPECT_EQ(4, out.x);
  EXPECT_EQ(2, out.y);

  in = "h8";
  out = uciFormatToPosition(in);
  EXPECT_EQ(7, out.x);
  EXPECT_EQ(7, out.y);

  in = "h1";
  out = uciFormatToPosition(in);
  EXPECT_EQ(7, out.x);
  EXPECT_EQ(0, out.y);
}

TEST(positionToUciFormat, can_convert){
  std::string out = "a1";
  EXPECT_EQ(out, positionToUciFormat({0, 0}));

  out = "b2";
  EXPECT_EQ(out, positionToUciFormat({1, 1}));

  out = "e3";
  EXPECT_EQ(out, positionToUciFormat({4, 2}));

  out = "h8";
  EXPECT_EQ(out, positionToUciFormat({7, 7}));

  out = "h1";
  EXPECT_EQ(out, positionToUciFormat({7, 0}));
}
