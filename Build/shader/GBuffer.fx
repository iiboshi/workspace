/* GBuffer Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

#define MICROGEOMETRY

/*----------------------------------------------------------------------------------------------------
	Buffer
----------------------------------------------------------------------------------------------------*/

// Texture
Texture2D	g_texDiffuse	: register( t0 );
Texture2D	g_texNormal		: register( t1 );
Texture2D	g_texMicro		: register( t2 );
TextureCube	g_texCube		: register( t3 );

// Sampler
SamplerState g_sampWorp		: register( s0 );
SamplerState g_sampMirr		: register( s1 );
SamplerState g_sampClamp	: register( s2 );

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
	float4	g_f4Param0;	//!< x:Normal補正 y:Microgeometry回数 z:Microgeometry強度 w:Albedo Only.
	float4	g_f4Param1;	//!< x:Roughness y:Fresnel z:SSS w:Use Roughness Tex.
	float4	g_f4Param2;	//!< x:Cube.
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

	// microgeometry.
	float fIntensity = 0.5f;
	#if defined( MICROGEOMETRY )
	float4 f4Micro = g_texMicro.Sample( g_sampWorp, input.Tex * (float2)g_f4Param0.y );
	f4Micro.xy = f4Micro.xy * (float2)2.0f - (float2)1.0f;
	f4Micro.x *= -1;
	fIntensity = f4Micro.w;
	float fMicroMask = 1.0f - f4Micro.w;
	#endif

	// albedo
	float4 albedoAndMask = g_texDiffuse.Sample( g_sampWorp, input.Tex );
	float3 albedo = albedoAndMask.xyz;
	#if defined( MICROGEOMETRY )
	float mask = lerp( 1.0f, albedoAndMask.w, g_f4Param2.y );
	fMicroMask *= mask;
	albedo = lerp( albedo, albedo * albedo, fMicroMask );
	#endif

	// 法線計算.
	float3 tnrm;
	tnrm	= g_texNormal.Sample( g_sampWorp, input.Tex ).xyz;
	tnrm.xy	= tnrm.xy * (float2)2.0f - (float2)1.0f;
	tnrm.xy	*= float2( g_f4Param0.x, -1.0f );
	tnrm.z	= 1.0f;
	#if defined( MICROGEOMETRY )
	tnrm.xy = lerp( tnrm.xy, tnrm.xy + f4Micro.xy, g_f4Param0.z * fMicroMask );
	tnrm	= normalize( tnrm );
	#endif
	float3 f3Nrm = normalize( input.Nrm );
	float3 f3Tan = normalize( input.Tan );
	float3 f3Bin = normalize( cross( f3Tan, f3Nrm ) );
	tnrm = normalize( tnrm.x * f3Tan + tnrm.y * f3Bin + tnrm.z * f3Nrm );
	tnrm = ( tnrm + (float3)1.0f ) * (float3)0.5f;

	// depth
	float depth = input.Pos.z / input.Pos.w;

	// Roughness.
	float rough		= g_f4Param1.x;
	float roughTex	= 1.0f - g_texNormal.Sample( g_sampWorp, input.Tex ).w;
	rough = lerp( rough, roughTex * rough, g_f4Param1.w );

	// w:Albedo Only.
	float only = g_f4Param0.w;

	// Cube.
	float3 ref = reflect( normalize( input.Vew ).xyz, normalize( input.Nrm ) );
	float3 cube = g_texCube.Sample( g_sampWorp,  normalize( input.Nrm ) ).xyz;
	albedo = lerp( albedo, cube, g_f4Param2.x );

	// 情報を送る.
	output.out0 = float4( albedo, only );
	output.out1 = float4( tnrm, 0.0f );
	output.out2 = float4( depth, fIntensity, 1.0f, 0.0f );
	output.out3 = float4( rough, g_f4Param1.yz, 0.0f );

	return output;
}
