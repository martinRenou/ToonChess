varying vec3 normal;

void main(void){
  // Compute the normal of the vertex
  normal = gl_NormalMatrix * gl_Normal;

  // The position of the vertex
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
