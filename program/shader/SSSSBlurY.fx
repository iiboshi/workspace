/* Blur Y Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

#define PI			( 3.14159265359f )
#define LIGNTNUM	2
#define WEIGHTNUM	8

#define WCOL0 ( float3( 0.233f,	0.455f,	0.649f	) * 0.5f )
#define WCOL1 ( float3( 0.1f,	0.336f,	0.344f	) * 0.5f )
#define WCOL2 ( float3( 0.118f,	0.198f,	0.0f	) * 0.5f )
#define WCOL3 ( float3( 0.113f,	0.007f,	0.007f	) * 0.5f )
#define WCOL4 ( float3( 0.358f,	0.004f,	0.0f	) * 0.5f )
#define WCOL5 ( float3( 0.078f,	0.0f,	0.0f	) * 0.5f )

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
	float pix = 10.0f / g_f4TexSize.y;
	_out.uv0 = float2( 0.0f, pix * 0.0064f	);
	_out.uv1 = float2( 0.0f, pix * 0.0484f	);
	_out.uv2 = float2( 0.0f, pix * 0.187f	);
	_out.uv3 = float2( 0.0f, pix * 0.567f	);
	_out.uv4 = float2( 0.0f, pix * 1.99f	);
	_out.uv5 = float2( 0.0f, pix * 7.41f	);

	return _out;
}

/*----------------------------------------------------------------------------------------------------
	Pixel Shader Function
----------------------------------------------------------------------------------------------------*/

float4 PS( PS_INPUT _in ) : SV_Target
{
	// î•ñ‚ÌŽæ“¾.
	float4 ret = float4( g_tex0.Sample( g_sampWorp, _in.uv ).xyz, 1.0f );
	float2 in3 = g_tex3.Sample( g_sampWorp, _in.uv ).xy;	//!< sss, depth.

	// Blur
	if( in3.x ) {
		_in.uv0 *= in3.y;
		_in.uv1 *= in3.y;
		_in.uv2 *= in3.y;
		_in.uv3 *= in3.y;
		_in.uv4 *= in3.y;
		_in.uv5 *= in3.y;
		ret.xyz = (float3)0.0f;
		ret.xyz += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv0 ).xyz + g_tex0.Sample( g_sampMirr, _in.uv - _in.uv0 ).xyz ) * WCOL0;
		ret.xyz += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv1 ).xyz + g_tex0.Sample( g_sampMirr, _in.uv - _in.uv1 ).xyz ) * WCOL1;
		ret.xyz += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv2 ).xyz + g_tex0.Sample( g_sampMirr, _in.uv - _in.uv2 ).xyz ) * WCOL2;
		ret.xyz += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv3 ).xyz + g_tex0.Sample( g_sampMirr, _in.uv - _in.uv3 ).xyz ) * WCOL3;
		ret.xyz += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv4 ).xyz + g_tex0.Sample( g_sampMirr, _in.uv - _in.uv4 ).xyz ) * WCOL4;
		ret.xyz += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv5 ).xyz + g_tex0.Sample( g_sampMirr, _in.uv - _in.uv5 ).xyz ) * WCOL5;
	}

	return ret;
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

