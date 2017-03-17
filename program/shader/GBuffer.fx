/* GBuffer Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------
	Buffer
----------------------------------------------------------------------------------------------------*/

// Texture
Texture2D g_texDiffuse	: register( t0 );
Texture2D g_texNormal	: register( t1 );

// Sampler
SamplerState ColorSmpWorp : register( s0 );

// Cbuffer
cbuffer cbViewProjection : register( b0 )
{
	float4	g_f4TexSize;
	float4	g_f4CameraPos;
	matrix	g_mView;
	matrix	g_mProjection;
};

// GBuffer
cbuffer cbGBuffer : register( b1 )
{
	matrix	g_mWorld;
	float4	g_f4Param0;	//!< x:Normal補正.
	float4	g_f4Param1;	//!< x:Roughness y:Fresnel z:SSS.
};

/*----------------------------------------------------------------------------------------------------
	Struct
----------------------------------------------------------------------------------------------------*/

struct VS_INPUT
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Nrm : NORMAL0;
	float3 Tan : TANGENT0;
	float4 Def : COLOR0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Vew : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Nrm : NORMAL0;
	float3 Tan : TANGENT0;
	float4 Def : COLOR1;
};

struct PS_OUTPUT
{
	float4 out0 : SV_TARGET0;	//!< xyz:Albedo.
	float4 out1 : SV_TARGET1;	//!< xyz:Normal.
	float4 out2 : SV_TARGET2;	//!< x:Depth y:Draw.
	float4 out3 : SV_TARGET2;	//!< x:Roughness y:Fresnel z:SSS.
};

/*----------------------------------------------------------------------------------------------------
	Vertex Shader
----------------------------------------------------------------------------------------------------*/

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul( float4(input.Pos, 1.0f), g_mWorld );
	output.Pos = mul( output.Pos, g_mView );
	output.Pos = mul( output.Pos, g_mProjection );
	output.Tex = input.Tex;

	//法線ベクトル処理.
	output.Nrm = mul( input.Nrm, (float3x3)g_mWorld );

	//接線ベクトル処理.
	output.Tan = mul( input.Tan, (float3x3)g_mWorld );

	// マテリアルカラー.
	output.Def = input.Def;

	// 視線ベクトル.
	output.Vew = g_f4CameraPos - mul( float4( input.Pos, 1.0f ), g_mWorld );

	return output;
}

/*----------------------------------------------------------------------------------------------------
	Pixel Shader
----------------------------------------------------------------------------------------------------*/

PS_OUTPUT PS( PS_INPUT input) : SV_Target
{
	PS_OUTPUT output;

	// 法線計算.
	float3 tnrm;
	tnrm	= g_texNormal.Sample( ColorSmpWorp, input.Tex ).xyz;
	tnrm.xy	= tnrm.xy * (float2)2.0f - (float2)1.0f;
	tnrm.xy	*= float2( g_f4Param0.x, -1.0f );
	tnrm.z	= 1.0f;
	float3 f3Nrm = normalize( input.Nrm );
	float3 f3Tan = normalize( input.Tan );
	float3 f3Bin = normalize( cross( f3Tan, f3Nrm ) );
	tnrm = normalize( tnrm.x * f3Tan + tnrm.y * f3Bin + tnrm.z * f3Nrm );
	tnrm = ( tnrm + (float3)1.0f ) * (float3)0.5f;

	// albedo
	float3 albedo = g_texDiffuse.Sample( ColorSmpWorp, input.Tex ).xyz;

	// depth
	float depth = input.Pos.z / input.Pos.w;

	// 情報を送る.
	output.out0 = float4( albedo, 1.0f );
	output.out1 = float4( tnrm, 1.0f );
	output.out2 = float4( depth, 1.0f, 0.0f, 1.0f );
	output.out3 = float4( g_f4Param1.xyz, 1.0f );

	return output;
}
