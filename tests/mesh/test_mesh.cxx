#include <gtest/gtest.h>

#include "../../src/mesh/Mesh.hxx"

TEST(mesh, init) {
  Mesh* test = new Mesh("../tests/testFixtures/test.obj");

  EXPECT_EQ("../tests/testFixtures/test.obj", test->filePath);
}

TEST(mesh, vertices) {
  Mesh* test = new Mesh("../tests/testFixtures/test.obj");

  test->initBuffers();

  EXPECT_EQ(0.0, test->vertices.at(0));
  EXPECT_EQ(0.0, test->vertices.at(1));
  EXPECT_EQ(0.0, test->vertices.at(2));

  EXPECT_EQ(0.0, test->vertices.at(3));
  EXPECT_EQ(0.0, test->vertices.at(4));
  EXPECT_EQ(1.0, test->vertices.at(5));

  EXPECT_EQ(1.0, test->vertices.at(21));
  EXPECT_EQ(1.0, test->vertices.at(22));
  EXPECT_EQ(1.0, test->vertices.at(23));
}

TEST(mesh, indices) {
  Mesh* test = new Mesh("../tests/testFixtures/test.obj");

  test->initBuffers();

  EXPECT_EQ(0, test->indices.at(0));
  EXPECT_EQ(6, test->indices.at(1));
  EXPECT_EQ(4, test->indices.at(2));

  EXPECT_EQ(0, test->indices.at(3));
  EXPECT_EQ(2, test->indices.at(4));
  EXPECT_EQ(6, test->indices.at(5));

  EXPECT_EQ(1, test->indices.at(33));
  EXPECT_EQ(7, test->indices.at(34));
  EXPECT_EQ(3, test->indices.at(35));
}
