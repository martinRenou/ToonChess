varying vec2 UV;

uniform sampler2D smokeTexture;

void main(void){
  gl_FragColor = texture2D(smokeTexture, UV);
}
