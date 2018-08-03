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

	// SSAO�T���v�����B 0�`16�A�����sampleSphere�ɂ�16�̗v�f�����Ȃ�����.
	static const unsigned int samples = 16; 
	
	static const float samplerRadius	= 0.03f;	// SSAO���a				0.00005f
	static const float strength			= 3.0f;		// �X�e�b�v������̋���	1.0f
	static const float totalStrength	= 9.0f;		// �����x				3.0f
	static const float falloffMin		= 0.02f;	// �[�������ʂ̍ŏ��l	0.00001f
	static const float falloffMax		= 0.2f;		// �[�������ʂ̍ő�l	0.006f
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

	// �����_���ȃT���v���J�[�l��.
	float4 texAOMap = g_texAOMap.Sample(g_sampWorp, input.tex);
	float3 randNor = randomNormal(input.tex);						// �����_���m�[�}�����擾����.
	float depth = 1.0f- texAOMap.w;									// ���̐[�����擾����.
	float3 normal = texAOMap.xyz;									// �r���[��Ԃ�ʏ�ɂ���.
	normal = normal * (float3)2.0f - (float3)1.0f;
	
	// ��������2D��Ԃō�Ƃ��Ă��܂����A��������ɂ��Ĕ��a���X�P�[�����O����K�v������܂��B�[�݂̂���V�[���̍��o���ێ�����B
	float radius = samplerRadius / depth; 
	
	float3 centerPos = float3(input.tex, depth); 
	float occ = 0.0f; 

	[unroll]
	for( unsigned int i = 0; i < samples; ++i ) 
	{ 
		// ������Y����ɔ��]������B�e�N�X�`�����W�͏ォ�牺�Ɍ������đ������A�r���[�̖@���̔��΂̓_�́A
		// ���̓����̔����̓_�𔽓]�����邱�ƂȂ��A�������A�ʏ�̃o�b�t�@��y���𔽓]���邱�Ƃ��ł��܂��B
		float3 offset = reflect(sampleSphere[i], randNor);	// �T���v���������_���ȕ����ɔ��f���܂��B
		offset = sign(dot(offset, normal))*offset;			// �����ɕϊ�����B
		offset.y = -offset.y; 
	
		// Ray�͌��݂̕�����Ƃ̑��ΓI�ȑ傫���ŁASSAO���a���g���ăX�P�[�����O����܂��B
		float3 ray = centerPos + radius*offset; 

		// ��ʊO�̃��C���X�L�b�v����.
		if((saturate(ray.x) != ray.x)||(saturate(ray.y) != ray.y)) 
			continue;

		float4 texAOMapWork = g_texAOMap.Sample(g_sampWorp, ray.xy);

		// ray.xy�̐��̐[�����擾����.
		float occDepth = 1.0f - texAOMapWork.w; 

		// ray.xy�̃r���[��Ԗ@�����擾���܂��B
		float3 occNormal = texAOMapWork.xyz; 
		occNormal = occNormal * (float3)2.0f - (float3)1.0f;
		
		// �[���ƃI�N���[�_�̐[���̍��B 
		float depthDifference = (centerPos.z - occDepth); 

		// �m�[�}���ƃI�N���[�W�����̖@���Ƃ̍��i�p�x�j�B
		float normalDifference = dot(occNormal, normal); 

		// �I�N���[�W�����͖@���Ԃ̊p�x�Ɉˑ����܂����A�p�x���������قǃI�N���[�W�����������Ȃ�܂��B
		float normalOcc = (1.0f - saturate(normalDifference)); 

		// �I�N���[�W�����͐[�x���Ɉˑ����AfalloffMin��falloffMax���g�p���Đ�������܂��B
		// ���ȕǂƃn���[�A�[�e�B�t�@�N�g�����炷�̂ɖ𗧂��܂��B falloffMin / Max�ŏ��������Ă݂Ă��������B
		float depthOcc = step(falloffMin, depthDifference)* 
		(1.0f - smoothstep(falloffMin, falloffMax, depthDifference)); 

		// ���ׂẴI�N���[�W�����t�@�N�^���܂Ƃ߂āA�X�e�b�v���Ƃ̋����ŃX�P�[�����O���܂��B
		occ += saturate(depthOcc*normalOcc*strength); 
	}

	// ���σI�N���[�W�����𓾂邽�߂ɃT���v�����ŏ��Z���܂��B
	occ /= samples;

	// ���ʂ𔽓]���A�S�̂̋������g�p���ċ������܂��B
	return saturate(pow(1.0f - occ, totalStrength)); 
}

/*----------------------------------------------------------------------------------------------------
	Function
----------------------------------------------------------------------------------------------------*/

