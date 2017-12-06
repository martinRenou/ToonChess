#include <gtest/gtest.h>

#include <GL/gl.h>

#include <vector>
#include <string>

#include "../src/utils.hxx"

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

TEST(matrixProduct, with_identity) {
  std::vector<GLfloat> in1 = getIdentityMatrix();

  std::vector<GLfloat> in2 = {
    3, 6, 5, 8,
    21.6, 21.5, 21.2, 21,
    56, 56, 56, 56,
    0, 93.3, 93.1, 93.2
  };

  std::vector<GLfloat> out = matrixProduct(&in1, &in2);

  EXPECT_EQ(3, out.at(0));
  EXPECT_EQ(6, out.at(1));
  EXPECT_EQ(5, out.at(2));
  EXPECT_EQ(8, out.at(3));

  EXPECT_EQ((GLfloat)21.6, out.at(4));
  EXPECT_EQ((GLfloat)21.5, out.at(5));
  EXPECT_EQ((GLfloat)21.2, out.at(6));
  EXPECT_EQ(21, out.at(7));

  EXPECT_EQ(56, out.at(8));
  EXPECT_EQ(56, out.at(9));
  EXPECT_EQ(56, out.at(10));
  EXPECT_EQ(56, out.at(11));

  EXPECT_EQ(0, out.at(12));
  EXPECT_EQ((GLfloat)93.3, out.at(13));
  EXPECT_EQ((GLfloat)93.1, out.at(14));
  EXPECT_EQ((GLfloat)93.2, out.at(15));
}

TEST(inverse, with_identity) {
  std::vector<GLfloat> in = getIdentityMatrix();

  std::vector<GLfloat> out = inverse(&in);

  EXPECT_EQ(1, out.at(0));
  EXPECT_EQ(0, out.at(1));
  EXPECT_EQ(0, out.at(2));
  EXPECT_EQ(0, out.at(3));

  EXPECT_EQ(0, out.at(4));
  EXPECT_EQ(1, out.at(5));
  EXPECT_EQ(0, out.at(6));
  EXPECT_EQ(0, out.at(7));

  EXPECT_EQ(0, out.at(8));
  EXPECT_EQ(0, out.at(9));
  EXPECT_EQ(1, out.at(10));
  EXPECT_EQ(0, out.at(11));

  EXPECT_EQ(0, out.at(12));
  EXPECT_EQ(0, out.at(13));
  EXPECT_EQ(0, out.at(14));
  EXPECT_EQ(1, out.at(15));
}

TEST(inverse, with_matrix) {
  std::vector<GLfloat> in = {
    1, 0, 2, 1,
    1, 3, 3, 0,
    1, 1, 1, 2,
    0, 2, 0, 1
  };

  std::vector<GLfloat> out = inverse(&in);

  EXPECT_EQ(-3, out.at(0));
  EXPECT_EQ(1, out.at(1));
  EXPECT_EQ(3, out.at(2));
  EXPECT_EQ(-3, out.at(3));

  EXPECT_EQ(-1./2., out.at(4));
  EXPECT_EQ(1./4., out.at(5));
  EXPECT_EQ(1./4., out.at(6));
  EXPECT_EQ(0, out.at(7));

  EXPECT_EQ(3./2., out.at(8));
  EXPECT_EQ(-1./4., out.at(9));
  EXPECT_EQ(-5./4., out.at(10));
  EXPECT_EQ(1, out.at(11));

  EXPECT_EQ(1, out.at(12));
  EXPECT_EQ(-1./2., out.at(13));
  EXPECT_EQ(-1./2., out.at(14));
  EXPECT_EQ(1, out.at(15));
}

TEST(transpose, with_matrix) {
  std::vector<GLfloat> in = {
    1, 0, 2, 1,
    1, 3, 3, 0,
    1, 1, 1, 2,
    0, 2, 0, 1
  };

  std::vector<GLfloat> out = transpose(&in);

  EXPECT_EQ(1, out.at(0));
  EXPECT_EQ(1, out.at(1));
  EXPECT_EQ(1, out.at(2));
  EXPECT_EQ(0, out.at(3));

  EXPECT_EQ(0, out.at(4));
  EXPECT_EQ(3, out.at(5));
  EXPECT_EQ(1, out.at(6));
  EXPECT_EQ(2, out.at(7));

  EXPECT_EQ(2, out.at(8));
  EXPECT_EQ(3, out.at(9));
  EXPECT_EQ(1, out.at(10));
  EXPECT_EQ(0, out.at(11));

  EXPECT_EQ(1, out.at(12));
  EXPECT_EQ(0, out.at(13));
  EXPECT_EQ(2, out.at(14));
  EXPECT_EQ(1, out.at(15));
}
