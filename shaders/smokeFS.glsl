#version 130

varying vec2 UV;
varying vec3 _color;
varying float _textureIndex;

uniform sampler2D smokeTexture0;
uniform sampler2D smokeTexture1;
uniform sampler2D smokeTexture2;

void main(void){
  vec4 color;

  float index = floor(_textureIndex);
  if(index == 0.0) color = texture2D(smokeTexture0, UV);
  else if(index == 1.0) color = texture2D(smokeTexture1, UV);
  else color = texture2D(smokeTexture2, UV);

  if(color.a <= 0.01) discard;

  // Multiply by sprite color
  color *= vec4(_color, 1.0);

  gl_FragColor = color;
}
