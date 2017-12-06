varying float lightIntensity;

uniform vec4 pieceColor;

vec4 applyCelShading(vec4 color){
  float factor = 0.5;
  if(lightIntensity > 0.8) factor = 1.0;
  else if (lightIntensity > 0.0) factor = 0.9;
  else if (lightIntensity > -0.3) factor = 0.7;

  color *= vec4(factor, factor, factor, 1.0);

  return color;
}

void main(void){
  gl_FragColor = applyCelShading(pieceColor);
}
