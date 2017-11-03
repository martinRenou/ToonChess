varying vec3 normal;

uniform vec4 pieceColor;
varying vec4 color;

void main(void){
  // Compute the normal of the vertex
  normal = gl_NormalMatrix * gl_Normal;

  // Set color of the chess piece
  color = pieceColor;

  // The position of the vertex
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
