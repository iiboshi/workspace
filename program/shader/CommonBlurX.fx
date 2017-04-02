/* Common Blur X Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

#define PI			( 3.14159265359f )
#define LIGNTNUM	2
#define WEIGHTNUM	8

#define POW 0.5f	//<! ブラー強度.

/*----------------------------------------------------------------------------------------------------
	Buffer
----------------------------------------------------------------------------------------------------*/

// Texture
Texture2D	g_tex0 : register( t0 );
Texture2D	g_tex1 : register( t1 );

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

// Deferred
cbuffer cbDeferred : register( b1 )
{
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

	float2 uv0	: TEXCOORD1;
	float2 uv1	: TEXCOORD2;
	float2 uv2	: TEXCOORD3;
	float2 uv3	: TEXCOORD4;
	float2 uv4	: TEXCOORD5;
	float2 uv5	: TEXCOORD6;
	float2 uv6	: TEXCOORD7;
	float2 uv7	: TEXCOORD8;

	float2 uv8	: TEXCOORD9;
	float2 uv9	: TEXCOORD10;
	float2 uv10	: TEXCOORD11;
	float2 uv11	: TEXCOORD12;
	float2 uv12	: TEXCOORD13;
	float2 uv13	: TEXCOORD14;
	float2 uv14	: TEXCOORD15;
	float2 uv15	: TEXCOORD16;
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

	// 位置座標
	_out.uv0 = _in.uv + float2( -1.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv1 = _in.uv + float2( -3.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv2 = _in.uv + float2( -5.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv3 = _in.uv + float2( -7.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv4 = _in.uv + float2( -9.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv5 = _in.uv + float2( -11.0f * POW	/ g_f4TexSize.x, 0.0f );
	_out.uv6 = _in.uv + float2( -13.0f * POW	/ g_f4TexSize.x, 0.0f );
	_out.uv7 = _in.uv + float2( -15.0f * POW	/ g_f4TexSize.x, 0.0f );

	_out.uv8 = _in.uv + float2( 1.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv9 = _in.uv + float2( 3.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv10 = _in.uv + float2( 5.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv11 = _in.uv + float2( 7.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv12 = _in.uv + float2( 9.0f * POW		/ g_f4TexSize.x, 0.0f );
	_out.uv13 = _in.uv + float2( 11.0f * POW	/ g_f4TexSize.x, 0.0f );
	_out.uv14 = _in.uv + float2( 13.0f * POW	/ g_f4TexSize.x, 0.0f );
	_out.uv15 = _in.uv + float2( 15.0f * POW	/ g_f4TexSize.x, 0.0f );

	return _out;
}

/*----------------------------------------------------------------------------------------------------
	Pixel Shader Function
----------------------------------------------------------------------------------------------------*/

float4 PS( PS_INPUT _in ) : SV_Target
{
	float ret = 0.0f;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv0 ).x + g_tex0.Sample( g_sampMirr, _in.uv15 ).x ) * g_f4Weight[0].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv1 ).x + g_tex0.Sample( g_sampMirr, _in.uv14 ).x ) * g_f4Weight[1].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv2 ).x + g_tex0.Sample( g_sampMirr, _in.uv13 ).x ) * g_f4Weight[2].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv3 ).x + g_tex0.Sample( g_sampMirr, _in.uv12 ).x ) * g_f4Weight[3].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv4 ).x + g_tex0.Sample( g_sampMirr, _in.uv11 ).x ) * g_f4Weight[4].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv5 ).x + g_tex0.Sample( g_sampMirr, _in.uv10 ).x ) * g_f4Weight[5].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv6 ).x + g_tex0.Sample( g_sampMirr, _in.uv9 ).x ) * g_f4Weight[6].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv7 ).x + g_tex0.Sample( g_sampMirr, _in.uv8 ).x ) * g_f4Weight[7].x;
	return float4( (float3)ret, 1.0f );
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

