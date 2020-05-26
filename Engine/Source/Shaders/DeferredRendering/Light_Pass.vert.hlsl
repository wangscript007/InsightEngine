
struct VS_INPUT
{
	float3 position	 : POSITION;
	float2 texCoords : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 sv_position	 : SV_POSITION;
	float2 texCoords	 : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT vs_in)
{
	VS_OUTPUT vs_out;
	
	vs_out.sv_position = float4(vs_in.position, 1.0);
	vs_out.texCoords = vs_in.texCoords;
	
	return vs_out;
}
