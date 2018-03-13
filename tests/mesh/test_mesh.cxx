#include <gtest/gtest.h>

#include "../../src/mesh/Mesh.hxx"
#include "../../src/mesh/loadObjFile.hxx"

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

TEST(mesh, vertices) {
  std::vector<Mesh *> meshes = loadObjFile("../tests/testFixtures/test.obj");

  EXPECT_EQ(0.0, meshes.at(0)->vertices.at(0));
  EXPECT_EQ(0.0, meshes.at(0)->vertices.at(1));
  EXPECT_EQ(0.0, meshes.at(0)->vertices.at(2));

  EXPECT_EQ(1.0, meshes.at(0)->vertices.at(3));
  EXPECT_EQ(1.0, meshes.at(0)->vertices.at(4));
  EXPECT_EQ(0.0, meshes.at(0)->vertices.at(5));

  EXPECT_EQ(0.0, meshes.at(0)->vertices.at(21));
  EXPECT_EQ(1.0, meshes.at(0)->vertices.at(22));
  EXPECT_EQ(1.0, meshes.at(0)->vertices.at(23));

  for(unsigned int i = 0; i < meshes.size(); i++)
      delete meshes.at(i);
  meshes.clear();
}

TEST(mesh, indices) {
  std::vector<Mesh *> meshes = loadObjFile("../tests/testFixtures/test.obj");

  EXPECT_EQ(0, meshes.at(0)->indices.at(0));
  EXPECT_EQ(1, meshes.at(0)->indices.at(1));
  EXPECT_EQ(2, meshes.at(0)->indices.at(2));

  EXPECT_EQ(3, meshes.at(0)->indices.at(3));
  EXPECT_EQ(4, meshes.at(0)->indices.at(4));
  EXPECT_EQ(5, meshes.at(0)->indices.at(5));

  EXPECT_EQ(33, meshes.at(0)->indices.at(33));
  EXPECT_EQ(34, meshes.at(0)->indices.at(34));
  EXPECT_EQ(35, meshes.at(0)->indices.at(35));

  EXPECT_EQ(meshes.at(0)->indices.size(), meshes.at(0)->vertices.size()/3);
  EXPECT_EQ(meshes.at(0)->indices.size(), meshes.at(0)->normals.size()/3);

  for(unsigned int i = 0; i < meshes.size(); i++)
    delete meshes.at(i);
  meshes.clear();
}

TEST(mesh, normals) {
  std::vector<Mesh *> meshes = loadObjFile("../tests/testFixtures/test.obj");

  EXPECT_EQ(0.0f, meshes.at(0)->normals.at(0));
  EXPECT_EQ(0.0f, meshes.at(0)->normals.at(1));
  EXPECT_EQ(-1.32f, meshes.at(0)->normals.at(2));

  EXPECT_EQ(-1.26f, meshes.at(0)->normals.at(18));
  EXPECT_EQ(0.0f, meshes.at(0)->normals.at(19));
  EXPECT_EQ(0.0f, meshes.at(0)->normals.at(20));

  for(unsigned int i = 0; i < meshes.size(); i++)
    delete meshes.at(i);
  meshes.clear();
}

TEST(mesh, second_mesh) {
  std::vector<Mesh*> meshes = loadObjFile("../tests/testFixtures/test.obj");

  EXPECT_NE(meshes.at(0), meshes.at(1));

  EXPECT_EQ(0.0f, meshes.at(1)->normals.at(0));
  EXPECT_EQ(0.0f, meshes.at(1)->normals.at(1));
  EXPECT_EQ(-1.36f, meshes.at(1)->normals.at(2));

  EXPECT_EQ(-1.52f, meshes.at(1)->normals.at(18));
  EXPECT_EQ(0.0f, meshes.at(1)->normals.at(19));
  EXPECT_EQ(0.0f, meshes.at(1)->normals.at(20));

  for(unsigned int i = 0; i < meshes.size(); i++)
    delete meshes.at(i);
  meshes.clear();
}
