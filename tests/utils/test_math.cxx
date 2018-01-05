#include <gtest/gtest.h>

#include <vector>

#include <GL/gl.h>

#include "../../src/utils/math.hxx"

TEST(matrixProduct, with_identity) {
  std::vector<GLfloat> in1 = getIdentityMatrix();

  std::vector<GLfloat> in2 = {
    3, 6, 5, 8,
    21.6, 21.5, 21.2, 21,
    56, 56, 56, 56,
    0, 93.3, 93.1, 93.2
  };

  std::vector<GLfloat> out = matrixProduct(&in1, &in2);

  std::vector<GLfloat> expectedOut = {
    3, 6, 5, 8,
    (GLfloat)21.6, (GLfloat)21.5, (GLfloat)21.2, 21,
    56, 56, 56, 56,
    0, (GLfloat)93.3, (GLfloat)93.1, (GLfloat)93.2
  };

  for(int i = 0; i < 16; i++){
    EXPECT_EQ(expectedOut.at(i), out.at(i));
  }
}

TEST(inverse, with_identity) {
  std::vector<GLfloat> in = getIdentityMatrix();

  std::vector<GLfloat> out = inverse(&in);

  std::vector<GLfloat> expectedOut = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };

  for(int i = 0; i < 16; i++){
    EXPECT_EQ(expectedOut.at(i), out.at(i));
  }
}

TEST(inverse, with_matrix) {
  std::vector<GLfloat> in = {
    1, 0, 2, 1,
    1, 3, 3, 0,
    1, 1, 1, 2,
    0, 2, 0, 1
  };

  std::vector<GLfloat> out = inverse(&in);

  std::vector<GLfloat> expectedOut = {
    -3, 1, 3, -3,
    -1./2., 1./4., 1./4., 0,
    3./2., -1./4., -5./4., 1,
    1, -1./2., -1./2., 1
  };

  for(int i = 0; i < 16; i++){
    EXPECT_EQ(expectedOut.at(i), out.at(i));
  }
}

TEST(transpose, with_matrix) {
  std::vector<GLfloat> in = {
    1, 0, 2, 1,
    1, 3, 3, 0,
    1, 1, 1, 2,
    0, 2, 0, 1
  };

  std::vector<GLfloat> out = transpose(&in);

  std::vector<GLfloat> expectedOut = {
    1, 1, 1, 0,
    0, 3, 1, 2,
    2, 3, 1, 0,
    1, 0, 2, 1
  };

  for(int i = 0; i < 16; i++){
    EXPECT_EQ(expectedOut.at(i), out.at(i));
  }
}
