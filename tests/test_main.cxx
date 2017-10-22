#include <gtest/gtest.h>

#include "./test_utils.cxx"

#include "./mesh/test_mesh.cxx"

int main(int argc, char **argv) {::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
