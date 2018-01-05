#include <gtest/gtest.h>

#include <vector>
#include <string>

#include "../../src/utils/utils.hxx"

TEST(load_file, txt) {
  std::string out = loadFile("../tests/testFixtures/test.txt");

  EXPECT_EQ("Hello\nWorld !\n!\n", out);
}

TEST(split, with_spaces) {
  std::vector<std::string> out = split("v 1.0 2.5 3.2", ' ');

  EXPECT_EQ("v", out[0]);
  EXPECT_EQ("1.0", out[1]);
  EXPECT_EQ("2.5", out[2]);
  EXPECT_EQ("3.2", out[3]);
}

TEST(split, with_dots) {
  std::vector<std::string> out = split("v 1.0 2.5 3.2", '.');

  EXPECT_EQ("v 1", out[0]);
  EXPECT_EQ("0 2", out[1]);
  EXPECT_EQ("5 3", out[2]);
  EXPECT_EQ("2", out[3]);
}
