#version 130

attribute vec3 vertexPosition;
attribute vec4 centerSize;
attribute vec4 colorTexture;

varying vec2 UV;
varying vec3 _color;
varying float _textureIndex;

// View/Projection matrices
uniform mat4 VMatrix;
uniform mat4 PMatrix;

void main(void){
  vec3 particleCenter = centerSize.xyz;
  float particleSize = centerSize.w;

  _color = colorTexture.xyz;
  _textureIndex = colorTexture.w;

  vec3 camera_right = vec3(VMatrix[0][0], VMatrix[1][0], VMatrix[2][0]);
  vec3 camera_up = vec3(VMatrix[0][1], VMatrix[1][1], VMatrix[2][1]);

  vec3 position = particleCenter
    + camera_right * vertexPosition.x * particleSize
    + camera_up * vertexPosition.z * particleSize;

  // Compute UV coordinates
  UV = vertexPosition.xz + vec2(0.5, 0.5);
  UV.y = abs(UV.y - 1.0);

  // The position of the vertex
  gl_Position = PMatrix * VMatrix * vec4(position, 1.0);
}
