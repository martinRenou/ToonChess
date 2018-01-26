uniform bool elevated;

uniform mat4 VMatrix;
uniform mat4 MMatrix;
uniform mat4 PMatrix;

vec4 position;

void main(void){
  // If the mesh should be elevated
  if(elevated){
    position = gl_Vertex + vec4(0, 0, 0.6, 0);
  } else {
    position = gl_Vertex;
  }

  float factor = 0.1;
  vec4 deformation = vec4(factor, factor, factor, 0.0) * normalize(position);
  vec4 deformedPosition = deformation + position;

  // The position of the vertex
  gl_Position = PMatrix * VMatrix * MMatrix * deformedPosition;
}
