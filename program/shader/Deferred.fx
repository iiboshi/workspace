/* Deferred Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

// #define SSSSTEST

#define PI			( 3.14159265359f )
#define LIGNTNUM	2
#define WEIGHTNUM	8

/*----------------------------------------------------------------------------------------------------
	Buffer
----------------------------------------------------------------------------------------------------*/

// Texture
Texture2D	g_tex0 : register( t0 );	//!< Albedo.
Texture2D	g_tex1 : register( t1 );	//!< Normal.
Texture2D	g_tex2 : register( t2 );	//!< Depth.
Texture2D	g_tex3 : register( t3 );	//!< Param.
Texture2D	g_tex4 : register( t4 );	//!< Shadow.
Texture2D	g_tex5 : register( t5 );	//!< AO.
Texture2D	g_tex6 : register( t6 );	//!< None.
Texture2D	g_tex7 : register( t7 );	//!< None.

Texture2D	g_texBeck : register( t8 );	//!< Beckmann Texture.

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
	float4	g_f4AO;
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

float G1V( float dotNV, float k );										// V
float GGX( float3 N, float3 V, float3 L, float roughness, float F0 );	// GGX
float Lambert( float3 normal, float3 lightVec );						// Lambert
float HalfLambert( float3 normal, float3 lightVec );					// Half Lambert
float3 CalcDiffuse( float3 normal );									// Calc Diffuse.
float3 CalcSpecular( float3 normal, float rough, float F0, float sss );	// Calc Specular.

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

struct PS_OUTPUT
{
	float4 out0 : SV_TARGET0;	//!< Dst.
	float4 out1 : SV_TARGET1;	//!< SSSColor.
	float4 out2 : SV_TARGET2;	//!< Spec.
	float4 out3 : SV_TARGET2;	//!< SSS.
};

/*----------------------------------------------------------------------------------------------------
	Pixel Shader Function
----------------------------------------------------------------------------------------------------*/

PS_OUTPUT PS( PS_INPUT _in ) : SV_Target
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float4 ret = (float4)1.0f;

	// 情報の取得.
	float4 in0 = g_tex0.Sample( g_sampWorp, _in.uv );
	float4 in1 = g_tex1.Sample( g_sampWorp, _in.uv );
	float4 in2 = g_tex2.Sample( g_sampWorp, _in.uv );
	float4 in3 = g_tex3.Sample( g_sampWorp, _in.uv );
	float4 in4 = g_tex4.Sample( g_sampWorp, _in.uv );
	float4 in5 = g_tex5.Sample( g_sampWorp, _in.uv );
	float3 albedo	= in0.xyz;
	float only		= in0.w;
	float3 normal	= in1.xyz;
	float depth		= in2.x;
	float intensity	= in2.y;
	float draw		= in2.z;
	float rough		= in3.x;
	float fresnel	= in3.y;
	float sss		= in3.z;
	float shw		= in4.x;
	float3 ao		= in5.xyz;

	// Normal
	normal = ( normal * (float3)2.0f ) - (float3)1.0f;

	// Lambert
	float3 diff = CalcDiffuse( normal ) * (float3)shw;

	// AO Color Bleeding.
	diff += g_f4AO.w * lerp( ao, pow( ao, 1.0 - g_f4AO.xyz ), sss );

	// Albedo.
	ret.xyz = diff * albedo;

	// Specular
	float3 spec = (float3)CalcSpecular( normal, rough, fresnel, sss ) * (float3)shw;
	spec = lerp( 0.0f, spec, trunc( draw ) );	//!< アンチエリアス自の淵の白いライン制御.
	spec *= ( intensity * 2.0f );

	// w:Albedo Only.
	ret.xyz = lerp( ret.xyz, albedo, only );
	spec = lerp( spec, 0.0f, only );

	// 情報を送る.
	output.out0 = float4( ret.xyz, 1.0f );
	output.out1 = float4( 1.0f, 1.0f, 1.0f, 1.0f );
	output.out2 = float4( spec, 1.0f );
	output.out3 = float4( sss, depth, 0.0f, 1.0f );

	#if defined( SSSSTEST )
	if( _in.pos.x < g_f4TexSize.x * 0.5f ){
		output.out3 = float4( 0.0f, depth, 0.0f, 1.0f );
	}
	if( _in.pos.x > g_f4TexSize.x * 0.5f - 1.0f && _in.pos.x < g_f4TexSize.x * 0.5f + 1.0f ){
		output.out0 = float4( 1.0f, 1.0f, 1.0f, 1.0f );
	}
	#endif

	return output;
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

// V
float G1V( float dotNV, float k )
{
	return 1.0f / ( dotNV * ( 1.0f - k ) + k );
}

// GGX
float GGX( float3 N, float3 V, float3 L, float roughness, float F0 )
{
	// α = ラフネス^2
	float alpha = roughness * roughness;
	// ハーフベクトル
	float3 H = normalize( V + L );
	// ベクトルの内積関連
	float dotNL = saturate( dot( N , L ) );
	float dotNV = saturate( dot( N , V ) );
	float dotNH = saturate( dot( N , H ) );
	float dotLH = saturate( dot( L , H ) );
	float F, D, vis;
	// D
	float alphaSqr	= alpha * alpha;
	float pi		= 3.14159f;
	float denom		= dotNH * dotNH * ( alphaSqr - 1.0 ) + 1.0f;
	D				= alphaSqr / ( pi * denom * denom );
	// F
	float dotLH5	= pow( 1.0f - dotLH, 5 );
	F				= F0 + ( 1.0 - F0 ) * ( dotLH5 );
	// V
	float k			= alpha / 2.0f;
	vis				= G1V( dotNL, k ) * G1V( dotNV, k);
	return dotNL * D * F * vis;
}

 float fresnelReflectance( float3 H, float3 V, float F0 )
{
	float base = 1.0f - dot( V, H );
	float exponential = pow( base, 5.0f );
	return exponential + F0 * ( 1.0f - exponential );
}

float KS_Skin_Specular(
	float3 N,	// Bumped surface normal
	float3 V,	// Points to eye
	float3 L,	// Points to light
	float m,	// Roughness
	float F0 )	// Fresnal
{  
	float result = 0.0f;
	float ndotl = dot( N, L );
	if( ndotl > 0.0f )
	{  
		float3 h = L + V;	// Unnormalized half-way vector
		float3 H = normalize( h );
		float ndoth = max( 0.0f, dot( N, H ) );
		float PH = pow( 2.0f* g_texBeck.Sample( g_sampClamp,float2(ndoth,m)).x, 10.0f );
		float F = fresnelReflectance( H, V, F0 );
		float frSpec = max( PH * F / dot( h, h ), 0.0f );
		result = ndotl * frSpec;	// BRDF * dot(N,L)
	 }
	 return result;
}

// Lambert
float Lambert( float3 normal, float3 lightVec )
{
	return max( 0.0f, dot( normal, lightVec ) ) * ( 1.0f / PI );
}

// Half Lambert
float HalfLambert( float3 normal, float3 lightVec )
{
	float light = max( 0.0f, dot( normal, lightVec ) );
	light = light * 0.5f + 0.5f;
	light = light * light;
	return light * ( 1.0f / PI );
}

// Diffuse.
float3 CalcDiffuse( float3 normal )
{
	float3 ret =	(float3)Lambert( normal, normalize( g_f4ViewVec.xyz ) ) * g_f4MainCol.xyz +
					(float3)Lambert( normal, normalize( g_f4LightVec[0].xyz ) ) * g_f4LightCol[0].xyz +
					(float3)Lambert( normal, normalize( g_f4LightVec[1].xyz ) ) * g_f4LightCol[1].xyz;
	return ret;
}

// Diffuse.
float3 CalcSpecular( float3 normal, float rough, float F0, float sss )
{
	float3 view = normalize( g_f4ViewVec.xyz );
	float retGGX =	GGX( normal, view, view, rough, F0 ) * g_f4MainCol.w +
					GGX( normal, view, normalize( g_f4LightVec[0].xyz ), rough, F0 ) * g_f4LightCol[0].w +
					GGX( normal, view, normalize( g_f4LightVec[1].xyz ), rough, F0 ) * g_f4LightCol[1].w;

	float retKSB =	KS_Skin_Specular( normal, view, view, rough, F0 ) * g_f4MainCol.w +
					KS_Skin_Specular( normal, view, normalize( g_f4LightVec[0].xyz ), rough, F0 ) * g_f4LightCol[0].w +
					KS_Skin_Specular( normal, view, normalize( g_f4LightVec[1].xyz ), rough, F0 ) * g_f4LightCol[1].w;

	float ret = lerp( retGGX, retKSB, sss );

	return (float3)ret;
}

