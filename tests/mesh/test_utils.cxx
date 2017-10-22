#include <gtest/gtest.h>

#include <GL/gl.h>

#include <vector>
#include <string>

#include "../../src/utils.hxx"

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

TEST(extract_float_vec, can_extract) {
  std::vector<std::string> in = {"v", "1.2", "3.5", "6.2"};
  std::vector<GLfloat> out;

  extractFloatVec3(&in, &out);

  EXPECT_EQ(1.2f, out[0]);
  EXPECT_EQ(3.5f, out[1]);
  EXPECT_EQ(6.2f, out[2]);
}

TEST(extract_face, can_extract) {
  std::vector<std::string> in = {"f", "4/2/1", "1/12/21", "6/12/3"};
  std::vector<GLuint> out;

  extractFace(&in, &out);

  EXPECT_EQ(3, out[0]);
  EXPECT_EQ(0, out[1]);
  EXPECT_EQ(5, out[2]);
}

TEST(extract_normal, can_extract) {
  std::vector<std::string> in = {"f", "4/2/1", "1/12/4", "6/12/2"};
  std::vector<GLfloat> out;

  std::vector<GLfloat> unsortedNormals = {
    1.2f, 5.3f, 6.2f,
    4.2f, 6.3f, 3.2f,
    2.2f, 4.3f, 1.2f,
    -1.2f, 1.3f, 4.2f,
  };

  extractNormal(&in, &unsortedNormals, &out);

  EXPECT_EQ(1.2f, out[0]);
  EXPECT_EQ(5.3f, out[1]);
  EXPECT_EQ(6.2f, out[2]);

  EXPECT_EQ(-1.2f, out[3]);
  EXPECT_EQ(1.3f, out[4]);
  EXPECT_EQ(4.2f, out[5]);

  EXPECT_EQ(4.2f, out[6]);
  EXPECT_EQ(6.3f, out[7]);
  EXPECT_EQ(3.2f, out[8]);
}
