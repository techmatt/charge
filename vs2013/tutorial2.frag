#version 150
// It was expressed that some drivers required this next line to function properly
precision highp float;

in  vec2 ex_TexCoord;
out vec4 gl_FragColor;

uniform sampler2D sampler;
 
void main(void) {
     //gl_FragColor = vec4(ex_TexCoord, 0.0, 1.0);
	gl_FragColor = vec4(texture(sampler, ex_TexCoord).xyzw);
	
}