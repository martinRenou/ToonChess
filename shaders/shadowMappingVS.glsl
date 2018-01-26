varying float depth;

uniform bool elevated;
uniform mat4 MMatrix;
uniform mat4 VMatrix;
uniform mat4 PMatrix;

vec4 position;

void main(void){
  // If the mesh should be elevated
  if(elevated){
    position = gl_Vertex + vec4(0, 0, 0.6, 0);
  } else {
    position = gl_Vertex;
  }

  // The position of the vertex on the screen
  position = PMatrix * VMatrix * MMatrix * position;

  // Get depth
  float zDepth = position.z/position.w;
  depth = 0.5 + zDepth * 0.5;

  gl_Position = position;
}
