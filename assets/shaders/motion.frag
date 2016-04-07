#version 130
// It was expressed that some drivers required this next line to function properly
precision highp float;

in  vec2 ex_TexCoord;
out vec4 fragColor;

uniform sampler2D mySampler;
uniform vec4 color;
 
void main(void) {
	fragColor = vec4(texture(mySampler, ex_TexCoord).xyz * color.xyz, color.w);
}
