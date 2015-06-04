#version 150
// It was expressed that some drivers required this next line to function properly
precision highp float;

in  vec2 ex_TexCoord;
out vec4 gl_FragColor;

uniform sampler2D sampler;
uniform vec4 color;
uniform vec2 kernelOffset;
 
void main(void) {
	vec3 c0 = texture(sampler, ex_TexCoord + kernelOffset * -2.0f).xyz;
	vec3 c1 = texture(sampler, ex_TexCoord + kernelOffset * -1.0f).xyz;
	vec3 c2 = texture(sampler, ex_TexCoord).xyz;
	vec3 c3 = texture(sampler, ex_TexCoord + kernelOffset * 1.0f).xyz;
	vec3 c4 = texture(sampler, ex_TexCoord + kernelOffset * 2.0f).xyz;

	float p0 = 1.0;
	float p1 = 0.5;
	float p2 = -0.1;
	float falloff = 1.0;

	float sum = p0 * 2.0 + p1 * 2.0 + p2;
	float scale = falloff / sum;

	float w0 = p0 * scale;
	float w1 = p1 * scale;
	float w2 = p2 * scale;
	float w3 = w1;
	float w4 = w0;

	gl_FragColor = vec4(c0 * w0 + c1 * w1 + c2 * w2 + c3 * w3 + c4 * w4, 0.75) * color;
}