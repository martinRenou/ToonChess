varying float depth;

uniform bool selected;
uniform mat4 MMatrix;
uniform mat4 VMatrix;
uniform mat4 PMatrix;

vec4 position;

void main(void){
  // If the mesh is selected, move it
  if(selected){
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
