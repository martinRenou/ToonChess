#version 130

varying float depth;

uniform mat4 MMatrix;
uniform mat4 VMatrix;
uniform mat4 PMatrix;

vec4 position;

void main(void){
  // The position of the vertex on the screen
  position = PMatrix * VMatrix * MMatrix * gl_Vertex;

  // Get depth
  float zDepth = position.z/position.w;
  depth = 0.5 + zDepth * 0.5;

  gl_Position = position;
}
