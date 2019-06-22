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
	float4	g_f4Param0;	//!< x:Normal�␳ y:Microgeometry�� z:Microgeometry���x w:Albedo Only.
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

	//�@���x�N�g������.
	output.Nrm = input.Nrm;

	//�ڐ��x�N�g������.
	output.Tan = input.Tan;

#else

	output.Pos = mul( float4(input.Pos, 1.0f), g_mWorld );
	output.Pos = mul( output.Pos, g_mView );
	output.Pos = mul( output.Pos, g_mProjection );
	output.Tex = input.Tex;

	//�@���x�N�g������.
	output.Nrm = mul( input.Nrm, (float3x3)g_mWorld );

	//�ڐ��x�N�g������.
	output.Tan = mul( input.Tan, (float3x3)g_mWorld );

#endif

	// �}�e���A���J���[.
	output.Def = input.Def;

	// �����x�N�g��.
	output.Vew = g_f4CameraPos - mul( float4( input.Pos, 1.0f ), g_mWorld );

	return output;
}

// �n���V�F�[�_�[�̃p�b�`�萔�t�F�[�Y�p�̏o�̓p�����[�^
struct CONSTANT_HS_OUT
{
	float Edges[3] : SV_TessFactor;        // �p�b�`�̃G�b�W�̃e�b�Z���[�V�����W��
	float Inside   : SV_InsideTessFactor;  // �p�b�`�����̃e�b�Z���[�V�����W��
	float3 conPos3 : POSITION3;            // �ǉ�����R���g���[�� �|�C���g
	float3 conPos4 : POSITION4;            // �ǉ�����R���g���[�� �|�C���g
	float3 conPos5 : POSITION5;            // �ǉ�����R���g���[�� �|�C���g
};

// �n���V�F�[�_�[�̃R���g���[�� �|�C���g �t�F�[�Y�p�̏o�̓p�����[�^
struct HS_OUT
{
	float3 Pos	: POSITION;
	float3 Nrm	: NORMAL;
	float3 Tan	: TANGENT;
	float2 Tex	: TEXCOORD0;
	float4 Def	: COLOR0;
};

// �h���C���V�F�[�_�[�̏o�̓p�����[�^
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

// �R���g���[���|�C���g���쐬����
float3 CreateControlPoint( VS_OUTPUT ip1, VS_OUTPUT ip2, float factor )
{
	float3 Out;

	// �R���g���[�� �|�C���g�̒��S�_�̍��W
	float3 center = ( ip1.Pos + ip2.Pos ) * 0.5f;

	// �@���x�N�g�����������A���K������
	float3 normal = normalize( ip1.Nrm + ip2.Nrm );

	// �@���x�N�g���̓��ς��v�Z���A�p�x���傫���Ȃ�قǒl���傫���Ȃ�悤�ɂ���
	float d = 1.0f - ( dot( ip1.Nrm, ip2.Nrm ) + 1.0f ) * 0.5f;

	// �R���g���[�� �|�C���g�̍��W���v�Z
	Out = center + normal * d * factor;

	return Out;
}

// *****************************************************************************************
// �n���V�F�[�_�[�̃p�b�`�萔�t�F�[�Y
// *****************************************************************************************
CONSTANT_HS_OUT ConstantsHS_Main( InputPatch<VS_OUTPUT, 3> ip, uint PatchID : SV_PrimitiveID )
{
	CONSTANT_HS_OUT Out = (CONSTANT_HS_OUT)0;

	float g_TessFactor = 1.f;

	// �萔�o�b�t�@�̒l�����̂܂ܓn��
	Out.Edges[0] = Out.Edges[1] = Out.Edges[2] = g_TessFactor;  // �p�b�`�̃G�b�W�̃e�b�Z���[�V�����W��
	Out.Inside   = g_TessFactor;  // �p�b�`�����̃e�b�Z���[�V�����W��

	// �R���g���[�� �|�C���g���R���_�ǉ�
	Out.conPos3 = CreateControlPoint( ip[0], ip[1], 1.0f );
	Out.conPos4 = CreateControlPoint( ip[1], ip[2], 1.0f );
	Out.conPos5 = CreateControlPoint( ip[2], ip[0], 1.0f );

	 return Out;
}

// *****************************************************************************************
// �n���V�F�[�_�[�̃R���g���[�� �|�C���g �t�F�[�Y
// *****************************************************************************************
[domain("tri")]                              // �e�b�Z���[�g���郁�b�V���̌`����w�肷��
[partitioning("integer")]                    // �p�b�`�̕����Ɏg�p����A���S���Y�����w�肷��
[outputtopology("triangle_ccw")]             // ���b�V���̏o�͕��@���w�肷��
[outputcontrolpoints(3)]                     // �n���V�F�[�_�[�̃R���g���[�� �|�C���g �t�F�[�Y���R�[��������
[patchconstantfunc("ConstantsHS_Main")]      // �Ή�����n���V�F�[�_�[�̃p�b�`�萔�t�F�[�Y�̃��C���֐�
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
// �h���C���V�F�[�_�[
// *****************************************************************************************
[domain("tri")]
DS_OUT DS( CONSTANT_HS_OUT In, float3 uvw : SV_DomainLocation, const OutputPatch<HS_OUT, 3> patch )
{
	DS_OUT Out = (DS_OUT)0;

	// ���_���W
	// �Q���x�W�F�Ȗʂɂ��ǉ��������_�̍��W���v�Z����
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

	// �@���x�N�g��
	float3 n1 = normalize( patch[2].Nrm * uvw.x + patch[1].Nrm * uvw.y + patch[0].Nrm * uvw.z );
	float3 t1 = normalize( patch[2].Tan * uvw.x + patch[1].Tan * uvw.y + patch[0].Tan * uvw.z );
	float4 d1 = normalize( patch[2].Def * uvw.x + patch[1].Def * uvw.y + patch[0].Def * uvw.z );
	// float color = max( 0.0f, dot( n1, -g_vecLight.xyz ) );
	// Out.color = float4( color, color, color, 1 );

	//�@���x�N�g������.
	Out.Nrm = mul( n1, (float3x3)g_mWorld );
	Out.Tan = mul( t1, (float3x3)g_mWorld );
	Out.Def = d1;

	// �e�N�Z��
	float2 tex1 = patch[2].Tex * uvw.x + patch[1].Tex * uvw.y + patch[0].Tex * uvw.z;
	Out.Tex = tex1;

 	// �����x�N�g��.
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

	// �@���v�Z.
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

	// ���𑗂�.
	output.out0 = float4( albedo, only );
	output.out1 = float4( tnrm, 0.0f );
	output.out2 = float4( depth, fIntensity, 1.0f, 0.0f );
	output.out3 = float4( rough, g_f4Param1.yz, 0.0f );

	return output;
}
