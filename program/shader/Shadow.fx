/* Shadow Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------
	Buffer
----------------------------------------------------------------------------------------------------*/

// Cbuffer
cbuffer cbViewProjection : register( b0 )
{
	matrix g_mView;
	matrix g_mProjection;
	matrix g_mWorld;
};

cbuffer cbShadowViewProjection : register( b1 )
{
	matrix g_mShadow;
};

// Texture
Texture2D	g_texShadowMap : register( t0 );	//!< ShadowMap.

// Sampler
SamplerState g_sampWorp		: register( s0 );
SamplerState g_sampMirr		: register( s1 );
SamplerState g_sampClamp	: register( s2 );

/*----------------------------------------------------------------------------------------------------
	Struct
----------------------------------------------------------------------------------------------------*/

struct VS_INPUT
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 nrm : NORMAL0;
	float3 tan : TANGENT0;
	float4 col : COLOR0;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 nrm : NORMAL0;
	float3 tan : TANGENT0;
	float4 col : COLOR1;
	float4 shw : SHADOW;
};

/*----------------------------------------------------------------------------------------------------
	Front Function
----------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------
	Vertex Shader
----------------------------------------------------------------------------------------------------*/

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.pos = mul( float4( input.pos, 1.0f ), g_mWorld );
	output.pos = mul( output.pos, g_mView );
	output.pos = mul( output.pos, g_mProjection );
	output.tex = input.tex;
	output.shw = mul( float4( input.pos, 1.0f ), g_mWorld );
	output.shw = mul( output.shw, g_mShadow );
	return output;
}

/*----------------------------------------------------------------------------------------------------
	Pixel Shader Function
----------------------------------------------------------------------------------------------------*/

float4 PS( PS_INPUT input) : SV_Target
{
	float4 ret = (float4)1.0f;

	// シャドウマップの深度値と比較する.
	float3 shadowCoord = input.shw.xyz / input.shw.w;

	// 最大深度傾斜を求める.
	float maxDepthSlope = max( abs( ddx( shadowCoord.z ) ), abs( ddy( shadowCoord.z ) ) );

	float shadowThreshold	= 1.0f;		// シャドウにするかどうかの閾値です.
	float bias				= 0.004f;	// 固定バイアスです.
	float slopeScaledBias	= 0.01f;	// 深度傾斜.
	float depthBiasClamp	= 0.1f;		// バイアスクランプ値.

	float shadowBias = bias + slopeScaledBias * maxDepthSlope;
	shadowBias = min( shadowBias, depthBiasClamp );

	float3 shadowColor	= float3( 0.0f, 0.0f, 0.0f );
	shadowThreshold 	= g_texShadowMap.SampleLevel( g_sampWorp, shadowCoord.xy, 0 ).x;
	shadowThreshold		= ( shadowThreshold > ( shadowCoord.z - shadowBias ) ); 
	shadowColor			= lerp( shadowColor, (float3)1.0f, shadowThreshold );

	ret = float4( shadowColor.xy, input.pos.z / input.pos.w, 1.0f );
	return ret;
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

