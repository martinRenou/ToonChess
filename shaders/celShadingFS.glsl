varying float vLightIntensity;
varying vec3 vLightPosition;

uniform vec4 pieceColor;

uniform sampler2D shadowMap;

float getLightFactor(){
  float factor = 0.5;
  if(vLightIntensity > 0.8) factor = 1.0;
  else if (vLightIntensity > 0.0) factor = 0.9;
  else if (vLightIntensity > -0.3) factor = 0.7;

  return factor;
}

void main(void){
  vec4 shadowMapColor = texture2D(shadowMap, vLightPosition.xy);
  float zShadowMap = shadowMapColor.r;

  float factor = getLightFactor();

  if(zShadowMap + 0.005 < vLightPosition.z){
    factor = 0.5;
  }

  vec4 color = pieceColor * vec4(factor, factor, factor, 1.0);

  gl_FragColor = color;
}
