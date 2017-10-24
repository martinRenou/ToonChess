varying vec3 normal;

vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0));

vec4 applyCelShading(vec4 color){
  float intensity = - dot(lightDir, normalize(normal));

  float factor = 0.5;
  if(intensity > 0.8) factor = 1.2;
  else if (intensity > 0.0) factor = 1.0;
  else if (intensity > -0.3) factor = 0.7;

  color *= vec4(factor, factor, factor, 1.0);

  return color;
}

void main(void){
  vec4 color = vec4(0.45, 0.66, 0.91, 1.0);

  gl_FragColor = applyCelShading(color);
}
