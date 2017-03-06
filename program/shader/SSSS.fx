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
	float4 in3 = g_tex3.Sample( g_sampWorp, _in.uv );
	_out.uv0.xy = float2( -1.0f / g_f4TexSize.x, 0.0f );
	_out.uv1.xy = float2( -3.0f / g_f4TexSize.x, 0.0f );
	_out.uv2.xy = float2( -5.0f / g_f4TexSize.x, 0.0f );
	_out.uv3.xy = float2( -7.0f / g_f4TexSize.x, 0.0f );
	_out.uv4.xy = float2( -9.0f / g_f4TexSize.x, 0.0f );
	_out.uv5.xy = float2( -11.0f / g_f4TexSize.x, 0.0f );
	_out.uv6.xy = float2( -13.0f / g_f4TexSize.x, 0.0f );
	_out.uv7.xy = float2( -15.0f / g_f4TexSize.x, 0.0f );
	_out.uv0.zw = float2( 0.0f, -1.0f / g_f4TexSize.y );
	_out.uv1.zw = float2( 0.0f, -3.0f / g_f4TexSize.y );
	_out.uv2.zw = float2( 0.0f, -5.0f / g_f4TexSize.y );
	_out.uv3.zw = float2( 0.0f, -7.0f / g_f4TexSize.y );
	_out.uv4.zw = float2( 0.0f, -9.0f / g_f4TexSize.y );
	_out.uv5.zw = float2( 0.0f, -11.0f / g_f4TexSize.y );
	_out.uv6.zw = float2( 0.0f, -13.0f / g_f4TexSize.y );
	_out.uv7.zw = float2( 0.0f, -15.0f / g_f4TexSize.y );

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
	float3 spec	= in2.xyz;
	float sss	= in3.x;
	float depth	= in3.y;

	// Blur
	if( sss )
	{
		float4 f4X = (float4)0.0f;
		int offsetX = 0;
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv0.xy * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv7.x + offsetX, _in.uv7.y ) * depth ) ) * g_f4Weight[0];
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv1.xy * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv6.x + offsetX, _in.uv6.y ) * depth ) ) * g_f4Weight[1];
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv2.xy * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv5.x + offsetX, _in.uv5.y ) * depth ) ) * g_f4Weight[2];
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv3.xy * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv4.x + offsetX, _in.uv4.y ) * depth ) ) * g_f4Weight[3];
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv4.xy * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv3.x + offsetX, _in.uv3.y ) * depth ) ) * g_f4Weight[4];
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv5.xy * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv2.x + offsetX, _in.uv2.y ) * depth ) ) * g_f4Weight[5];
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv6.xy * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv1.x + offsetX, _in.uv1.y ) * depth ) ) * g_f4Weight[6];
		f4X += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv7.xy * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv0.x + offsetX, _in.uv0.y ) * depth ) ) * g_f4Weight[7];
		float4 f4Y = (float4)0.0f;
		int offsetY = 0;
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv0.zw * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv7.z, _in.uv7.w + offsetY ) * depth ) ) * g_f4Weight[0];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv1.zw * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv6.z, _in.uv6.w + offsetY ) * depth ) ) * g_f4Weight[1];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv2.zw * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv5.z, _in.uv5.w + offsetY ) * depth ) ) * g_f4Weight[2];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv3.zw * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv4.z, _in.uv4.w + offsetY ) * depth ) ) * g_f4Weight[3];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv4.zw * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv3.z, _in.uv3.w + offsetY ) * depth ) ) * g_f4Weight[4];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv5.zw * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv2.z, _in.uv2.w + offsetY ) * depth ) ) * g_f4Weight[5];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv6.zw * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv1.z, _in.uv1.w + offsetY ) * depth ) ) * g_f4Weight[6];
		f4Y += ( g_tex0.Sample( g_sampMirr, _in.uv + _in.uv7.zw * depth ) + g_tex0.Sample( g_sampMirr, _in.uv + float2( _in.uv0.z, _in.uv0.w + offsetY ) * depth ) ) * g_f4Weight[7];
		float4 f4Blur = f4X * 0.5f + f4Y * 0.5f;
		diff = lerp( diff, f4Blur.xyz, sss );
	}

	// Final
	ret.xyz = diff + spec;
	ret.xyz = pow( ret.xyz, (float3)2.0f );

	return ret;
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

