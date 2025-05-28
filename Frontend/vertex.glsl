precision highp float;

uniform sampler2D samp;

uniform mat4 trans_mat;

out vec2 tc;

void main(){

vec4 pos_v = vec4(position,1.0);

pos_v = trans_mat * pos_v;

gl_Position = projectionMatrix * viewMatrix * modelMatrix * pos_v;

tc = uv;

}