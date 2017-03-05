/* SSSS Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

#define PI			( 3.14159265359f )
#define LIGNTNUM	2
#define WEIGHTNUM	8

/*----------------------------------------------------------------------------------------------------
	Buffer
----------------------------------------------------------------------------------------------------*/

// Texture
Texture2D	g_tex0 : register( t0 );	//< Albedo.
Texture2D	g_tex1 : register( t1 );	//< Normal.
Texture2D	g_tex2 : register( t2 );	//< Depth.
Texture2D	g_tex3 : register( t3 );	//< Param.
Texture2D	g_tex4 : register( t4 );	//< None.
Texture2D	g_tex5 : register( t5 );	//< None.
Texture2D	g_tex6 : register( t6 );	//< None.
Texture2D	g_tex7 : register( t7 );	//< None.

// Sampler
SamplerState g_sampWorp : register( s0 );
SamplerState g_sampMirr : register( s1 );

// Cbuffer
cbuffer cbViewProjection : register( b0 )
{
	float4	g_f4TexSize;
	float4	g_f4CameraPos;
	matrix	g_mView;
	matrix	g_mProjection;
};

// Deferred
cbuffer cbDeferred : register( b1 )
{
	float4	g_f4ViewVec;
	float4	g_f4MainCol;
	float4	g_f4LightVec[LIGNTNUM];
	float4	g_f4LightCol[LIGNTNUM];
	float4	g_f4Weight[WEIGHTNUM];
};

/*----------------------------------------------------------------------------------------------------
	Struct
----------------------------------------------------------------------------------------------------*/

struct VS_INPUT
{
	float3 pos	: POSITION;
	float2 uv	: TEXCOORD;
};

struct PS_INPUT
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

/*----------------------------------------------------------------------------------------------------
	Front Function
----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------
	Vertex Shader
----------------------------------------------------------------------------------------------------*/

PS_INPUT VS( VS_INPUT _in )
{
	PS_INPUT _out = (PS_INPUT)0;
	_out.pos = float4( _in.pos, 1.0f );
	_out.uv = _in.uv;
	return _out;
}

/*----------------------------------------------------------------------------------------------------
	Pixel Shader Function
----------------------------------------------------------------------------------------------------*/

float4 PS( PS_INPUT _in ) : SV_Target
{
	float4 ret = (float4)1.0f;

	// 情報の取得.
	float4 in0 = g_tex0.Sample( g_sampWorp, _in.uv );
	float4 in1 = g_tex1.Sample( g_sampWorp, _in.uv );
	float4 in2 = g_tex2.Sample( g_sampWorp, _in.uv );
	float4 in3 = g_tex3.Sample( g_sampWorp, _in.uv );

	ret.xyz = in0.xyz + in2.xyz;
	ret.xyz = pow( ret.xyz, (float3)2.0f );

	return ret;
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

