#version 130

varying float vLightIntensity;
varying vec3 vLightPosition;

uniform vec4 color;

uniform int shadowMapResolution;

uniform sampler2D shadowMap;

float getLightFactor(){
  float factor = 0.5;
  if(vLightIntensity > 0.8) factor = 1.0;
  else if (vLightIntensity > 0.3) factor = 0.9;
  else if (vLightIntensity > 0.0) factor = 0.7;

  return factor;
}

void main(void){
  // Percentage Close Filtering
  float sum = 0.;
  vec2 dxy;
  for(float x = -1.5; x <= 1.5; x += 1.) {
    for(float y = -1.5; y <= 1.5; y += 1.) {
      dxy = vec2(x/float(shadowMapResolution), y/float(shadowMapResolution));
      sum += texture2D(shadowMap, vLightPosition.xy + dxy).r;
    }
  }
  sum /= 16.;
  float shadowCoeff = smoothstep(0.001, 0.04, vLightPosition.z - sum);

  // Compute light factor
  float factor = getLightFactor();
  if(shadowCoeff > 0.5){
    factor = 0.5;
  }

  gl_FragColor = color * vec4(factor, factor, factor, 1.0);
}
