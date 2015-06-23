#version 150
// It was expressed that some drivers required this next line to function properly
precision highp float;

in  vec2 ex_TexCoord;
out vec4 fragColor;

uniform sampler2D mySampler;
uniform vec3 focusColorA;
uniform vec3 focusColorB;
uniform vec2 pulse;
 
float f(float x)
{
	return x;
}

void main(void)
{
	vec3 color = texture(mySampler, ex_TexCoord).xyz;
	
	float threshold = 0.15;

	float scaleA = f(max(0.0, 1.0 - length(color - focusColorA) / threshold)) * pulse.x;
	float scaleB = f(max(0.0, 1.0 - length(color - focusColorB) / threshold)) * pulse.y;

	fragColor = vec4((scaleA + scaleB) * color, 1.0);
}
