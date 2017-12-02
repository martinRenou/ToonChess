varying vec3 normal;

uniform bool selected;
vec4 position;

void main(void){
  // Compute the normal of the vertex
  normal = gl_NormalMatrix * gl_Normal;

  // If the mesh is selected, move it
  if(selected){
    position = gl_Vertex + vec4(0, 0, 0.6, 0);
  } else {
    position = gl_Vertex;
  }

  // The position of the vertex
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;
}
