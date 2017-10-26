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

TEST(extract_float_vec, can_extract) {
  std::vector<std::string> in = {"v", "1.2", "3.5", "6.2"};
  std::vector<GLfloat> out;

  extractFloatVec3(&in, &out);

  EXPECT_EQ(1.2f, out[0]);
  EXPECT_EQ(3.5f, out[1]);
  EXPECT_EQ(6.2f, out[2]);
}

TEST(extract_vertices, can_extract) {
  std::vector<std::string> in = {"f", "4/2/1", "1/12/21", "6/12/3"};
  std::vector<GLfloat> out;

  std::vector<GLfloat> unsortedVertices = {
    1.2f, 5.3f, 6.2f,
    0.2f, 0.3f, 0.2f,
    56.2f, 56.3f, 56.2f,
    -1.2f, 1.3f, 4.2f,
    221.2f, 222.3f, 223.2f,
    4.2f, 6.3f, 3.2f
  };

  extractVertices(&in, &unsortedVertices, &out);

  EXPECT_EQ(-1.2f, out[0]);
  EXPECT_EQ(1.3f, out[1]);
  EXPECT_EQ(4.2f, out[2]);

  EXPECT_EQ(1.2f, out[3]);
  EXPECT_EQ(5.3f, out[4]);
  EXPECT_EQ(6.2f, out[5]);

  EXPECT_EQ(4.2f, out[6]);
  EXPECT_EQ(6.3f, out[7]);
  EXPECT_EQ(3.2f, out[8]);
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

  extractNormals(&in, &unsortedNormals, &out);

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
