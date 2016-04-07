#version 130
// It was expressed that some drivers required this next line to function properly
precision highp float;

in  vec2 ex_TexCoord;
out vec4 fragColor;

uniform sampler2D mySampler;
uniform vec4 color;
 
void main(void) {
	fragColor = texture(mySampler, ex_TexCoord).xyzw * color;
	//gl_FragColor = vec4(texture(mySampler, ex_TexCoord).xyz, 0.5f) * color;
}
