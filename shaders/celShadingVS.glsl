varying vec3 normal;
varying float lightIntensity;

uniform bool selected;
uniform mat4 VMatrix;
uniform mat4 MMatrix;
uniform mat4 PMatrix;

vec4 position;
vec3 lightDir = normalize(vec3(-1.0, 0.0, -1.0));

void main(void){
  // Compute the normal of the vertex and the light intensity
  //TODO normal = transpose(inverse(MMatrix)) * gl_Normal;
  normal = gl_Normal;
  lightDir = lightDir;

  lightIntensity = - dot(lightDir, normalize(normal));

  // If the mesh is selected, move it
  if(selected){
    position = gl_Vertex + vec4(0, 0, 0.6, 0);
  } else {
    position = gl_Vertex;
  }

  // The position of the vertex
  gl_Position = PMatrix * VMatrix * MMatrix * position;
}
