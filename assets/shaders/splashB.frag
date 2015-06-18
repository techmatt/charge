#version 150
// It was expressed that some drivers required this next line to function properly
precision highp float;

in  vec2 ex_TexCoord;
out vec4 gl_FragColor;

uniform sampler2D sampler;
uniform vec2 kernelOffset;
 
void main(void) {
	vec4 c0 = texture(sampler, ex_TexCoord + kernelOffset * -3.0f).xyzw;
	vec4 c1 = texture(sampler, ex_TexCoord + kernelOffset * -2.0f).xyzw;
	vec4 c2 = texture(sampler, ex_TexCoord + kernelOffset * -1.0f).xyzw;
	vec4 c3 = texture(sampler, ex_TexCoord).xyzw;
	vec4 c4 = texture(sampler, ex_TexCoord + kernelOffset * 1.0f).xyzw;
	vec4 c5 = texture(sampler, ex_TexCoord + kernelOffset * 2.0f).xyzw;
	vec4 c6 = texture(sampler, ex_TexCoord + kernelOffset * 3.0f).xyzw;

	float p0 = 0.1;
	float p1 = 0.2;
	float p2 = 0.3;
	float p3 = 0.4;

	float w0 = p0;
	float w1 = p1;
	float w2 = p2;
	float w3 = p3;
	float w4 = w2;
	float w5 = w1;
	float w6 = w0;

	vec4 v = c0 * w0 + c1 * w1 + c2 * w2 + c3 * w3 + c4 * w4 + c5 * w5 + c6 * w6;

	gl_FragColor = vec4(v);
}