#version 150
// It was expressed that some drivers required this next line to function properly
precision highp float;

in  vec2 ex_TexCoord;
out vec4 gl_FragColor;

uniform sampler2D sampler;
uniform vec4 color;
 
void main(void) {
	gl_FragColor = texture(sampler, ex_TexCoord).xyzw * color;
}