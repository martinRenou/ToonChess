#include <gtest/gtest.h>

#include "../../src/mesh/Mesh.hxx"

TEST(mesh, init) {
  Mesh* test = new Mesh("../tests/testFixtures/test.obj");

  EXPECT_EQ("../tests/testFixtures/test.obj", test->filePath);

  delete test;
}

TEST(mesh, vertices) {
  Mesh* test = new Mesh("../tests/testFixtures/test.obj");

  test->initBuffers();

  EXPECT_EQ(0.0, test->vertices.at(0));
  EXPECT_EQ(0.0, test->vertices.at(1));
  EXPECT_EQ(0.0, test->vertices.at(2));

  EXPECT_EQ(1.0, test->vertices.at(3));
  EXPECT_EQ(1.0, test->vertices.at(4));
  EXPECT_EQ(0.0, test->vertices.at(5));

  EXPECT_EQ(0.0, test->vertices.at(21));
  EXPECT_EQ(1.0, test->vertices.at(22));
  EXPECT_EQ(1.0, test->vertices.at(23));

  delete test;
}

TEST(mesh, indices) {
  Mesh* test = new Mesh("../tests/testFixtures/test.obj");

  test->initBuffers();

  EXPECT_EQ(0, test->indices.at(0));
  EXPECT_EQ(1, test->indices.at(1));
  EXPECT_EQ(2, test->indices.at(2));

  EXPECT_EQ(3, test->indices.at(3));
  EXPECT_EQ(4, test->indices.at(4));
  EXPECT_EQ(5, test->indices.at(5));

  EXPECT_EQ(33, test->indices.at(33));
  EXPECT_EQ(34, test->indices.at(34));
  EXPECT_EQ(35, test->indices.at(35));

  EXPECT_EQ(test->indices.size(), test->vertices.size()/3);
  EXPECT_EQ(test->indices.size(), test->normals.size()/3);

  delete test;
}

TEST(mesh, normals) {
  Mesh* test = new Mesh("../tests/testFixtures/test.obj");

  test->initBuffers();

  EXPECT_EQ(0.0f, test->normals.at(0));
  EXPECT_EQ(0.0f, test->normals.at(1));
  EXPECT_EQ(-1.32f, test->normals.at(2));

  EXPECT_EQ(-1.26f, test->normals.at(18));
  EXPECT_EQ(0.0f, test->normals.at(19));
  EXPECT_EQ(0.0f, test->normals.at(20));

  delete test;
}
