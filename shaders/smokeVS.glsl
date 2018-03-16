attribute vec3 vertexPosition;
attribute vec4 center_size;

varying vec2 UV;

// View/Projection matrices
uniform mat4 VMatrix;
uniform mat4 PMatrix;

void main(void){
  vec3 particleCenter = center_size.xyz;
  float particleSize = center_size.w;

  vec3 position = particleCenter + vec3(
    vertexPosition.x * particleSize, 0.0, vertexPosition.z * particleSize);

  // Compute UV coordinates
  UV = vertexPosition.xz + vec2(0.5, 0.5);

  // The position of the vertex
  gl_Position = PMatrix * VMatrix * vec4(position, 1.0);
}
