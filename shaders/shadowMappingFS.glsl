#version 130

varying float depth;

void main(void){
  gl_FragColor = vec4(depth, 0.0, 0.0, 1.0);
}
