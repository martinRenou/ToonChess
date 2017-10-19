#include <gtest/gtest.h>

#include <vector>
#include <string>

#include "../../src/mesh/utils.hxx"

TEST(SplitFunctionTest1, CanSplitWithSpaces) {
  std::vector<std::string> out = split("v 1.0 2.5 3.2", ' ');

  EXPECT_EQ("v", out[0]);
  EXPECT_EQ("1.0", out[1]);
  EXPECT_EQ("2.5", out[2]);
  EXPECT_EQ("3.2", out[3]);
}

TEST(SplitFunctionTest2, CanSplitWithDots) {
  std::vector<std::string> out = split("v 1.0 2.5 3.2", '.');

  EXPECT_EQ("v 1", out[0]);
  EXPECT_EQ("0 2", out[1]);
  EXPECT_EQ("5 3", out[2]);
  EXPECT_EQ("2", out[3]);
}
