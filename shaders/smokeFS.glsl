varying vec2 UV;

uniform sampler2D smokeTexture;

void main(void){
  vec4 color = texture2D(smokeTexture, UV);
  if(color.a <= 0.01) discard;
  gl_FragColor = color;
}
