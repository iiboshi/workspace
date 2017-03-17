/* Blur X Shader */

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

	float4 uv0	: TEXCOORD1;
	float4 uv1	: TEXCOORD2;
	float4 uv2	: TEXCOORD3;
	float4 uv3	: TEXCOORD4;
	float4 uv4	: TEXCOORD5;
	float4 uv5	: TEXCOORD6;
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
	float pix = 8.0f / g_f4TexSize.x;
	_out.uv0.xy = float2( pix * 0.0064f,	0.0f );
	_out.uv1.xy = float2( pix * 0.0484f,	0.0f );
	_out.uv2.xy = float2( pix * 0.187f,		0.0f );
	_out.uv3.xy = float2( pix * 0.567f,		0.0f );
	_out.uv4.xy = float2( pix * 1.99f,		0.0f );
	_out.uv5.xy = float2( pix * 7.41f,		0.0f );

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
	float sss	= in3.x;
	float depth	= in3.y;

	// Blur
	if( sss )
	{
		float4 f4X = (float4)0.0f;
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv0.xy ) * depth ) + g_tex0.Sample( g_sampMirr, _in.uv - float2( _in.uv0.xy ) * depth ) ) * WCOL0 * 0.5f;
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv1.xy ) * depth ) + g_tex0.Sample( g_sampMirr, _in.uv - float2( _in.uv1.xy ) * depth ) ) * WCOL1 * 0.5f;
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv2.xy ) * depth ) + g_tex0.Sample( g_sampMirr, _in.uv - float2( _in.uv2.xy ) * depth ) ) * WCOL2 * 0.5f;
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv3.xy ) * depth ) + g_tex0.Sample( g_sampMirr, _in.uv - float2( _in.uv3.xy ) * depth ) ) * WCOL3 * 0.5f;
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv4.xy ) * depth ) + g_tex0.Sample( g_sampMirr, _in.uv - float2( _in.uv4.xy ) * depth ) ) * WCOL4 * 0.5f;
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv5.xy ) * depth ) + g_tex0.Sample( g_sampMirr, _in.uv - float2( _in.uv5.xy ) * depth ) ) * WCOL5 * 0.5f;
		diff = lerp( diff, f4X.xyz, sss );
	}

	// Final
	ret.xyz = diff;

	return ret;
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

