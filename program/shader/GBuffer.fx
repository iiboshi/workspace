/* GBuffer Shader */

/*----------------------------------------------------------------------------------------------------
	Define
----------------------------------------------------------------------------------------------------*/

#define MICROGEOMETRY
// #define TESSELLATION

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
	float3 Pos		: POSITION;
	float2 Tex		: TEXCOORD0;
	float3 Nrm		: NORMAL0;
	float3 Tan		: TANGENT0;
	float4 Def		: COLOR0;
};

struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	float4 Vew		: POSITION;
	float2 Tex		: TEXCOORD0;
	float3 Nrm		: NORMAL0;
	float3 Tan		: TANGENT0;
	float4 Def		: COLOR1;
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

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

#if defined( TESSELLATION )

	output.Pos = float4( input.Pos, 1.f );
	output.Tex = input.Tex;

	//法線ベクトル処理.
	output.Nrm = input.Nrm;

	//接線ベクトル処理.
	output.Tan = input.Tan;

#else

	output.Pos = mul( float4(input.Pos, 1.0f), g_mWorld );
	output.Pos = mul( output.Pos, g_mView );
	output.Pos = mul( output.Pos, g_mProjection );
	output.Tex = input.Tex;

	//法線ベクトル処理.
	output.Nrm = mul( input.Nrm, (float3x3)g_mWorld );

	//接線ベクトル処理.
	output.Tan = mul( input.Tan, (float3x3)g_mWorld );

#endif

	// マテリアルカラー.
	output.Def = input.Def;

	// 視線ベクトル.
	output.Vew = g_f4CameraPos - mul( float4( input.Pos, 1.0f ), g_mWorld );

	return output;
}

// ハルシェーダーのパッチ定数フェーズ用の出力パラメータ
struct CONSTANT_HS_OUT
{
	float Edges[3] : SV_TessFactor;        // パッチのエッジのテッセレーション係数
	float Inside   : SV_InsideTessFactor;  // パッチ内部のテッセレーション係数
	float3 conPos3 : POSITION3;            // 追加するコントロール ポイント
	float3 conPos4 : POSITION4;            // 追加するコントロール ポイント
	float3 conPos5 : POSITION5;            // 追加するコントロール ポイント
};

// ハルシェーダーのコントロール ポイント フェーズ用の出力パラメータ
struct HS_OUT
{
	float3 Pos	: POSITION;
	float3 Nrm	: NORMAL;
	float3 Tan	: TANGENT;
	float2 Tex	: TEXCOORD0;
	float4 Def	: COLOR0;
};

// ドメインシェーダーの出力パラメータ
struct DS_OUT
{
	float4 Pos	: SV_POSITION;
	float3 Nrm	: NORMAL;
	float3 Tan	: TANGENT;
	float2 Tex	: TEXCOORD0;
	float4 Vew	: POSITION6;
	float4 Def	: COLOR0;

	// float4 color : COLOR;
};

// コントロールポイントを作成する
float3 CreateControlPoint( VS_OUTPUT ip1, VS_OUTPUT ip2, float factor )
{
	float3 Out;

	// コントロール ポイントの中心点の座標
	float3 center = ( ip1.Pos + ip2.Pos ) * 0.5f;

	// 法線ベクトルを合成し、正規化する
	float3 normal = normalize( ip1.Nrm + ip2.Nrm );

	// 法線ベクトルの内積を計算し、角度が大きくなるほど値が大きくなるようにする
	float d = 1.0f - ( dot( ip1.Nrm, ip2.Nrm ) + 1.0f ) * 0.5f;

	// コントロール ポイントの座標を計算
	Out = center + normal * d * factor;

	return Out;
}

// *****************************************************************************************
// ハルシェーダーのパッチ定数フェーズ
// *****************************************************************************************
CONSTANT_HS_OUT ConstantsHS_Main( InputPatch<VS_OUTPUT, 3> ip, uint PatchID : SV_PrimitiveID )
{
	CONSTANT_HS_OUT Out = (CONSTANT_HS_OUT)0;

	float g_TessFactor = 1.f;

	// 定数バッファの値をそのまま渡す
	Out.Edges[0] = Out.Edges[1] = Out.Edges[2] = g_TessFactor;  // パッチのエッジのテッセレーション係数
	Out.Inside   = g_TessFactor;  // パッチ内部のテッセレーション係数

	// コントロール ポイントを３頂点追加
	Out.conPos3 = CreateControlPoint( ip[0], ip[1], 1.0f );
	Out.conPos4 = CreateControlPoint( ip[1], ip[2], 1.0f );
	Out.conPos5 = CreateControlPoint( ip[2], ip[0], 1.0f );

	 return Out;
}

// *****************************************************************************************
// ハルシェーダーのコントロール ポイント フェーズ
// *****************************************************************************************
[domain("tri")]                              // テッセレートするメッシュの形状を指定する
[partitioning("integer")]                    // パッチの分割に使用するアルゴリズムを指定する
[outputtopology("triangle_ccw")]             // メッシュの出力方法を指定する
[outputcontrolpoints(3)]                     // ハルシェーダーのコントロール ポイント フェーズがコールされる回数
[patchconstantfunc("ConstantsHS_Main")]      // 対応するハルシェーダーのパッチ定数フェーズのメイン関数
HS_OUT HS( InputPatch<VS_OUTPUT, 3> In, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
	HS_OUT Out = (HS_OUT)0;

	Out.Pos = In[i].Pos;
	Out.Nrm = In[i].Nrm;
	Out.Tex = In[i].Tex;
	Out.Tan = In[i].Tan;
	Out.Def = In[i].Def;
	return Out;
}

// *****************************************************************************************
// ドメインシェーダー
// *****************************************************************************************
[domain("tri")]
DS_OUT DS( CONSTANT_HS_OUT In, float3 uvw : SV_DomainLocation, const OutputPatch<HS_OUT, 3> patch )
{
	DS_OUT Out = (DS_OUT)0;

	// 頂点座標
	// ２次ベジェ曲面により追加した頂点の座標を計算する
	float3 p1 = patch[2].Pos * pow( uvw.x, 2.0f ) + 
				patch[1].Pos * pow( uvw.y, 2.0f ) + 
				patch[0].Pos * pow( uvw.z, 2.0f ) + 
				2.0f * In.conPos5 * uvw.x * uvw.z + 
				2.0f * In.conPos4 * uvw.y * uvw.x + 
				2.0f * In.conPos3 * uvw.z * uvw.y;

	//Out.Pos =  mul( float4( p1, 1.0f ), g_matWVP );
	Out.Pos = mul( float4(p1, 1.0f), g_mWorld );
	Out.Pos = mul( Out.Pos, g_mView );
	Out.Pos = mul( Out.Pos, g_mProjection );

	// 法線ベクトル
	float3 n1 = normalize( patch[2].Nrm * uvw.x + patch[1].Nrm * uvw.y + patch[0].Nrm * uvw.z );
	float3 t1 = normalize( patch[2].Tan * uvw.x + patch[1].Tan * uvw.y + patch[0].Tan * uvw.z );
	float4 d1 = normalize( patch[2].Def * uvw.x + patch[1].Def * uvw.y + patch[0].Def * uvw.z );
	// float color = max( 0.0f, dot( n1, -g_vecLight.xyz ) );
	// Out.color = float4( color, color, color, 1 );

	//法線ベクトル処理.
	Out.Nrm = mul( n1, (float3x3)g_mWorld );
	Out.Tan = mul( t1, (float3x3)g_mWorld );
	Out.Def = d1;

	// テクセル
	float2 tex1 = patch[2].Tex * uvw.x + patch[1].Tex * uvw.y + patch[0].Tex * uvw.z;
	Out.Tex = tex1;

 	// 視線ベクトル.
	Out.Vew = g_f4CameraPos - mul( float4( p1, 1.0f ), g_mWorld );
 
	return Out;
}

/*----------------------------------------------------------------------------------------------------
	Pixel Shader
----------------------------------------------------------------------------------------------------*/

#if defined( TESSELLATION )
PS_OUTPUT PS( DS_OUT input) : SV_Target
#else
PS_OUTPUT PS( VS_OUTPUT input) : SV_Target
#endif
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
