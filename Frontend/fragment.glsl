precision highp float;

in vec2 tc;
uniform sampler2D samp;

void main() {


    vec4 out_c = texture(samp,tc);

    if(out_c.a==0.0){
      
      // discard;
    }

    gl_FragColor = out_c;
}