#version 130
// It was expressed that some drivers required this next line to function properly
precision highp float;

in  vec2 ex_TexCoord;
out vec4 fragColor;

uniform sampler2D mySampler;
uniform vec4 color;
uniform vec2 kernelOffset;
 
void main(void) {
	vec4 c0 = texture(mySampler, ex_TexCoord + kernelOffset * -2.0f).xyzw;
	vec4 c1 = texture(mySampler, ex_TexCoord + kernelOffset * -1.0f).xyzw;
	vec4 c2 = texture(mySampler, ex_TexCoord).xyzw;
	vec4 c3 = texture(mySampler, ex_TexCoord + kernelOffset * 1.0f).xyzw;
	vec4 c4 = texture(mySampler, ex_TexCoord + kernelOffset * 2.0f).xyzw;

	float p0 = 0.4;
	float p1 = 0.6;
	float p2 = 1.0;
	float falloff = 0.95;

	float sum = p0 * 2.0 + p1 * 2.0 + p2;
	float scale = falloff / sum;

	float w0 = p0 * scale;
	float w1 = p1 * scale;
	float w2 = p2 * scale;
	float w3 = w1;
	float w4 = w0;

	vec4 v = c0 * w0 + c1 * w1 + c2 * w2 + c3 * w3 + c4 * w4;

	v = vec4(v.xyz * 1.05f, v.w * 1.01f);
	//float alpha = 0.0f;
	//float alpha = min(1.0, length(v.xyz));
	//v.w = alpha;
	
	fragColor = vec4(v) * color;
}