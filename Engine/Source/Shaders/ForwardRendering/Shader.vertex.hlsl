
struct VS_INPUT
{
	float3 position  : POSITION;
	float2 texCoords : TEXCOORD;
	float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
	float3 biTangent : BITANGENT;
};

struct VS_OUTPUT
{
	float4 position  : SV_POSITION;
	float2 texCoords : TEXCOORD;
	float3 normal    : NORMAL;
	float3 tangent   : TANGENT;
	float3 biTangent : BITANGENT;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

cbuffer cbPerFrame : register(b1)
{
	float3 cameraPosition;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
	VS_OUTPUT vs_out;

	float4x4 worldSpace = mul(mul(world, view), projection);
	vs_out.position = mul(float4(vs_in.position, 1.0f), worldSpace);
	
	vs_out.texCoords = vs_in.texCoords;
	vs_out.normal = vs_in.normal;
	vs_out.tangent = vs_in.tangent;
	vs_out.biTangent = vs_in.biTangent;

	return vs_out;
}