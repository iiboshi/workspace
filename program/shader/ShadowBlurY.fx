/* Blur Y Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

#define PI			( 3.14159265359f )
#define LIGNTNUM	2
#define WEIGHTNUM	8

#define POW 0.5f	//<! ブラー強度.

#define WCOL0 float4( 0.233f,	0.455f,	0.649f,	1.0f )
#define WCOL1 float4( 0.1f,		0.336f,	0.344f,	1.0f )
#define WCOL2 float4( 0.118f,	0.198f,	0.0f,	1.0f )
#define WCOL3 float4( 0.113f,	0.007f,	0.007f,	1.0f )
#define WCOL4 float4( 0.358f,	0.004f,	0.0f,	1.0f )
#define WCOL5 float4( 0.078f,	0.0f,	0.0f,	1.0f )

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

	float2 uv0	: TEXCOORD1;
	float2 uv1	: TEXCOORD2;
	float2 uv2	: TEXCOORD3;
	float2 uv3	: TEXCOORD4;
	float2 uv4	: TEXCOORD5;
	float2 uv5	: TEXCOORD6;
	float2 uv6	: TEXCOORD7;
	float2 uv7	: TEXCOORD8;
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
	_out.uv0 = _in.uv + float2( 0.0f, -1.0f * POW	/ g_f4TexSize.y );
	_out.uv1 = _in.uv + float2( 0.0f, -3.0f * POW	/ g_f4TexSize.y );
	_out.uv2 = _in.uv + float2( 0.0f, -5.0f * POW	/ g_f4TexSize.y );
	_out.uv3 = _in.uv + float2( 0.0f, -7.0f * POW	/ g_f4TexSize.y );
	_out.uv4 = _in.uv + float2( 0.0f, -9.0f * POW	/ g_f4TexSize.y );
	_out.uv5 = _in.uv + float2( 0.0f, -11.0f * POW	/ g_f4TexSize.y );
	_out.uv6 = _in.uv + float2( 0.0f, -13.0f * POW	/ g_f4TexSize.y );
	_out.uv7 = _in.uv + float2( 0.0f, -15.0f * POW	/ g_f4TexSize.y );

	return _out;
}

/*----------------------------------------------------------------------------------------------------
	Pixel Shader Function
----------------------------------------------------------------------------------------------------*/

float4 PS( PS_INPUT _in ) : SV_Target
{
	// Blur	
	float ret = 0.0f;
	int offset = 16.0f / g_f4TexSize.y;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv0 ).x + g_tex0.Sample( g_sampMirr, float2( _in.uv7.x, _in.uv7.y + offset ) ).x ) * g_f4Weight[0].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv1 ).x + g_tex0.Sample( g_sampMirr, float2( _in.uv6.x, _in.uv6.y + offset ) ).x ) * g_f4Weight[1].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv2 ).x + g_tex0.Sample( g_sampMirr, float2( _in.uv5.x, _in.uv5.y + offset ) ).x ) * g_f4Weight[2].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv3 ).x + g_tex0.Sample( g_sampMirr, float2( _in.uv4.x, _in.uv4.y + offset ) ).x ) * g_f4Weight[3].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv4 ).x + g_tex0.Sample( g_sampMirr, float2( _in.uv3.x, _in.uv3.y + offset ) ).x ) * g_f4Weight[4].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv5 ).x + g_tex0.Sample( g_sampMirr, float2( _in.uv2.x, _in.uv2.y + offset ) ).x ) * g_f4Weight[5].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv6 ).x + g_tex0.Sample( g_sampMirr, float2( _in.uv1.x, _in.uv1.y + offset ) ).x ) * g_f4Weight[6].x;
	ret += ( g_tex0.Sample( g_sampMirr, _in.uv7 ).x + g_tex0.Sample( g_sampMirr, float2( _in.uv0.x, _in.uv0.y + offset ) ).x ) * g_f4Weight[7].x;
	return float4( (float3)ret, 1.0f );
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

