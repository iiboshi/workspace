/* Blur Y Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

#define PI			( 3.14159265359f )
#define LIGNTNUM	2
#define WEIGHTNUM	8

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
Texture2D	g_tex0 : register( t0 );	//!< Albedo.
Texture2D	g_tex1 : register( t1 );	//!< Normal.
Texture2D	g_tex2 : register( t2 );	//!< Depth.
Texture2D	g_tex3 : register( t3 );	//!< Param.
Texture2D	g_tex4 : register( t4 );	//!< None.
Texture2D	g_tex5 : register( t5 );	//!< None.
Texture2D	g_tex6 : register( t6 );	//!< None.
Texture2D	g_tex7 : register( t7 );	//!< None.

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

	float4 uv0	: TEXCOORD1;
	float4 uv1	: TEXCOORD2;
	float4 uv2	: TEXCOORD3;
	float4 uv3	: TEXCOORD4;
	float4 uv4	: TEXCOORD5;
	float4 uv5	: TEXCOORD6;
	float4 uv6	: TEXCOORD7;
	float4 uv7	: TEXCOORD8;
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

	// ˆÊ’uÀ•W
	_out.uv0.xy = _in.uv + float2(0.0f, -1.0f / g_f4TexSize.y);
	_out.uv1.xy = _in.uv + float2(0.0f, -3.0f / g_f4TexSize.y);
	_out.uv2.xy = _in.uv + float2(0.0f, -5.0f / g_f4TexSize.y);
	_out.uv3.xy = _in.uv + float2(0.0f, -7.0f / g_f4TexSize.y);
	_out.uv4.xy = _in.uv + float2(0.0f, -9.0f / g_f4TexSize.y);
	_out.uv5.xy = _in.uv + float2(0.0f, -11.0f / g_f4TexSize.y);
	_out.uv6.xy = _in.uv + float2(0.0f, -13.0f / g_f4TexSize.y);
	_out.uv7.xy = _in.uv + float2( 0.0f, -15.0f / g_f4TexSize.y );

	return _out;
}

/*----------------------------------------------------------------------------------------------------
	Pixel Shader Function
----------------------------------------------------------------------------------------------------*/

float4 PS( PS_INPUT _in ) : SV_Target
{
	float4 ret = (float4)0.0f;

	// î•ñ‚ÌŽæ“¾.
	float4 in0 = g_tex0.Sample( g_sampWorp, _in.uv );
	float4 in1 = g_tex1.Sample( g_sampWorp, _in.uv );
	float4 in2 = g_tex2.Sample( g_sampWorp, _in.uv );
	float4 in3 = g_tex3.Sample( g_sampWorp, _in.uv );
	float3 diff	= in0.xyz;
	float blur = in3.x;

	// Blur
	if( blur )
	{
		float4 f4Y = (float4)0.0f;
		int offsetY = 16.0f / g_f4TexSize.y;
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv0.xy ) + g_tex0.Sample( g_sampMirr, float2( _in.uv7.x, _in.uv7.y + offsetY ) ) ) * g_f4Weight[0];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv1.xy ) + g_tex0.Sample( g_sampMirr, float2( _in.uv6.x, _in.uv6.y + offsetY ) ) ) * g_f4Weight[1];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv2.xy ) + g_tex0.Sample( g_sampMirr, float2( _in.uv5.x, _in.uv5.y + offsetY ) ) ) * g_f4Weight[2];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv3.xy ) + g_tex0.Sample( g_sampMirr, float2( _in.uv4.x, _in.uv4.y + offsetY ) ) ) * g_f4Weight[3];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv4.xy ) + g_tex0.Sample( g_sampMirr, float2( _in.uv3.x, _in.uv3.y + offsetY ) ) ) * g_f4Weight[4];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv5.xy ) + g_tex0.Sample( g_sampMirr, float2( _in.uv2.x, _in.uv2.y + offsetY ) ) ) * g_f4Weight[5];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv6.xy ) + g_tex0.Sample( g_sampMirr, float2( _in.uv1.x, _in.uv1.y + offsetY ) ) ) * g_f4Weight[6];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv7.xy ) + g_tex0.Sample( g_sampMirr, float2( _in.uv0.x, _in.uv0.y + offsetY ) ) ) * g_f4Weight[7];
		diff = lerp( diff, f4Y.xyz, blur );
	}

	// Final
	ret.xyz = diff;

	return ret;
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

