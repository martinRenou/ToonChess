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

  std::vector<std::string> expectedOut = {"v", "1.0", "2.5", "3.2"};

  for(int i = 0; i < 4; i++){
    EXPECT_EQ(expectedOut.at(i), out.at(i));
  }
}

TEST(split, with_dots) {
  std::vector<std::string> out = split("v 1.0 2.5 3.2", '.');

  std::vector<std::string> expectedOut = {"v 1", "0 2", "5 3", "2"};

  for(int i = 0; i < 4; i++){
    EXPECT_EQ(expectedOut.at(i), out.at(i));
  }
}
