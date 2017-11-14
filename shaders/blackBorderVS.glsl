void main(void){
  float factor = 0.1;
  vec4 deformation = vec4(factor, factor, factor, 0.0) * normalize(gl_Vertex);
  vec4 deformedPosition = deformation + gl_Vertex;

  // The position of the vertex
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * deformedPosition;
}