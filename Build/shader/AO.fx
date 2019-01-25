/* AO Shader */

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

cbuffer cbAOViewProjection : register( b1 )
{
	matrix g_mAO;
};

// Texture
Texture2D	g_texAOMap : register( t0 );	//!< AO Map.

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
	float4 aoc : AO;
};

/*----------------------------------------------------------------------------------------------------
	Front Function
----------------------------------------------------------------------------------------------------*/

float3 randomNormal(float2 tex) 
{ 
	float noiseX = (frac(sin(dot(tex, float2(15.8989f, 76.132f)*1.0f))*46336.23745f)); 
	float noiseY = (frac(sin(dot(tex, float2(11.9899f, 62.223f)*2.0f))*34748.34744f)); 
	float noiseZ = (frac(sin(dot(tex, float2(13.3238f, 63.122f)*3.0f))*59998.47362f)); 
	return normalize(float3(noiseX, noiseY, noiseZ)); 
}

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
	output.aoc = mul( float4( input.pos, 1.0f ), g_mWorld );
	output.aoc = mul( output.aoc, g_mAO );
	return output;
}

/*----------------------------------------------------------------------------------------------------
	Pixel Shader Function
----------------------------------------------------------------------------------------------------*/

float4 PS( PS_INPUT input) : SV_Target
{
	// Projection UV.
	input.tex = input.aoc.xy / input.aoc.w;

	// SSAOサンプル数。 0～16、これはsampleSphereには16個の要素しかないため.
	static const unsigned int samples = 16; 
	
	static const float samplerRadius	= 0.03f;	// SSAO半径				0.00005f
	static const float strength			= 3.0f;		// ステップ当たりの強さ	1.0f
	static const float totalStrength	= 9.0f;		// 総強度				3.0f
	static const float falloffMin		= 0.02f;	// 深さ減衰量の最小値	0.00001f
	static const float falloffMax		= 0.2f;		// 深さ減衰量の最大値	0.006f
	static const float3 sampleSphere[] = { 
		float3( 0.2024537f, 0.841204f, -0.9060141f), 
		float3(-0.2200423f, 0.6282339f,-0.8275437f), 
		float3( 0.3677659f, 0.1086345f,-0.4466777f), 
		float3( 0.8775856f, 0.4617546f,-0.6427765f), 
		float3( 0.7867433f,-0.141479f, -0.1567597f), 
		float3( 0.4839356f,-0.8253108f,-0.1563844f), 
		float3( 0.4401554f,-0.4228428f,-0.3300118f), 
		float3( 0.0019193f,-0.8048455f, 0.0726584f), 
		float3(-0.7578573f,-0.5583301f, 0.2347527f), 
		float3(-0.4540417f,-0.252365f, 0.0694318f), 
		float3(-0.0483353f,-0.2527294f, 0.5924745f), 
		float3(-0.4192392f, 0.2084218f,-0.3672943f), 
		float3(-0.8433938f, 0.1451271f, 0.2202872f), 
		float3(-0.4037157f,-0.8263387f, 0.4698132f), 
		float3(-0.6657394f, 0.6298575f, 0.6342437f), 
		float3(-0.0001783f, 0.2834622f, 0.8343929f), }; 

	// ランダムなサンプルカーネル.
	float4 texAOMap = g_texAOMap.Sample(g_sampWorp, input.tex);
	float3 randNor = randomNormal(input.tex);						// ランダムノーマルを取得する.
	float depth = 1.0f- texAOMap.w;									// 線の深さを取得する.
	float3 normal = texAOMap.xyz;									// ビュー空間を通常にする.
	normal = normal * (float3)2.0f - (float3)1.0f;
	
	// 私たちは2D空間で作業していますが、距離を基準にして半径をスケーリングする必要があります。深みのあるシーンの錯覚を維持する。
	float radius = samplerRadius / depth; 
	
	float3 centerPos = float3(input.tex, depth); 
	float occ = 0.0f; 

	[unroll]
	for( unsigned int i = 0; i < samples; ++i ) 
	{ 
		// 半球をY軸上に反転させる。テクスチャ座標は上から下に向かって増加し、ビューの法線の反対の点は、
		// 床の内側の半球の点を反転させることなく、もちろん、通常のバッファのy軸を反転することもできます。
		float3 offset = reflect(sampleSphere[i], randNor);	// サンプルをランダムな方向に反映します。
		offset = sign(dot(offset, normal))*offset;			// 半球に変換する。
		offset.y = -offset.y; 
	
		// Rayは現在の文字列との相対的な大きさで、SSAO半径を使ってスケーリングされます。
		float3 ray = centerPos + radius*offset; 

		// 画面外のレイをスキップする.
		if((saturate(ray.x) != ray.x)||(saturate(ray.y) != ray.y)) 
			continue;

		float4 texAOMapWork = g_texAOMap.Sample(g_sampWorp, ray.xy);

		// ray.xyの線の深さを取得する.
		float occDepth = 1.0f - texAOMapWork.w; 

		// ray.xyのビュー空間法線を取得します。
		float3 occNormal = texAOMapWork.xyz; 
		occNormal = occNormal * (float3)2.0f - (float3)1.0f;
		
		// 深さとオクルーダの深さの差。 
		float depthDifference = (centerPos.z - occDepth); 

		// ノーマルとオクルージョンの法線との差（角度）。
		float normalDifference = dot(occNormal, normal); 

		// オクルージョンは法線間の角度に依存しますが、角度が小さいほどオクルージョンが多くなります。
		float normalOcc = (1.0f - saturate(normalDifference)); 

		// オクルージョンは深度差に依存し、falloffMinとfalloffMaxを使用して制限されます。
		// 自己閉塞とハローアーティファクトを減らすのに役立ちます。 falloffMin / Maxで少し試してみてください。
		float depthOcc = step(falloffMin, depthDifference)* 
		(1.0f - smoothstep(falloffMin, falloffMax, depthDifference)); 

		// すべてのオクルージョンファクタをまとめて、ステップごとの強さでスケーリングします。
		occ += saturate(depthOcc*normalOcc*strength); 
	}

	// 平均オクルージョンを得るためにサンプル数で除算します。
	occ /= samples;

	// 結果を反転し、全体の強さを使用して強化します。
	return saturate(pow(1.0f - occ, totalStrength)); 
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

