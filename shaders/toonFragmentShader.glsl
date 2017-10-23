varying vec3 normal;

vec3 lightDir = vec3(0.0, 1.0, 1.0);

vec4 applyCelShading(vec4 color){
  float intensity = dot(lightDir, normalize(normal));

  color *= vec4(intensity, intensity, intensity, 1.0);

  return color;
}

void main(void){
  vec4 color = vec4(0.82, 0.62, 0.16, 1.0);

  gl_FragColor = applyCelShading(color);
}
