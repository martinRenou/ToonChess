void main(void){
  vec4 deformedPosition = vec4(1.03, 1.03, 1.01, 1.0) * gl_Vertex;

  // The position of the vertex
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * deformedPosition;
}
