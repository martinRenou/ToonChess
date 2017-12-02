varying float depth;
vec4 position;
uniform bool selected;

void main(void){
  // If the mesh is selected, move it
  if(selected){
    position = gl_Vertex + vec4(0, 0, 0.6, 0);
  } else {
    position = gl_Vertex;
  }

  // The position of the vertex on the screen
  position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;

  // Get depth
  float zDepth = position.z/position.w;
  depth = 0.5 + zDepth * 0.5;

  gl_Position = position;
}
