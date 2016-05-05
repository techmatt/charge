
cbuffer ConstantBuffer : register(b0)
{
	matrix worldViewProj;
	float4 modelColor;
}

Texture2D modelTexture : register(t0);
SamplerState modelSampler : register(s0);

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 worldPos : WORLDPOS;
};

VertexShaderOutput vertexShaderMain(
	float4 position : position,
	float4 color : color,
	float2 texCoord : texCoord)
{
	VertexShaderOutput output;
	output.position = mul(position, worldViewProj);
	output.texCoord = texCoord.xy;
	output.worldPos = position.xyz;
	return output;
}

float4 pixelShaderMain(VertexShaderOutput input) : SV_Target
{
	float4 texColor = modelTexture.Sample(modelSampler, float2(input.texCoord.x, input.texCoord.y));

	return float4(modelColor.xyzw * texColor.xyzw);
}
