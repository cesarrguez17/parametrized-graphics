#define NUM_LIGHTS 3

//static const float PI = 3.14159265f;
Texture2D shaderTexture;
SamplerState SampleType{
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

cbuffer cbPerFrame : register( b0 )
{
	matrix g_mWorld;                   // World matrix for object
	matrix g_mView;					 // View matrix for object
	matrix g_mProj;					 // View matrix for object
	float4 g_vMouse;
	
};

cbuffer cbLightConstants : register(b1){
	

	float4 g_Ambient;
	float4 g_Diffuse;
	float4 g_Specular;
	float4  g_vLightPos[NUM_LIGHTS];
	float4  g_vLightColor[NUM_LIGHTS];
	float4  g_vEyePos;
}


//------------------------------------------------------------
struct VS_INPUT2
{
	float3 vPosition	:POSITION;
	//float2 vTexCoord	:TEXCOORD0;
	//float4 vColor		:COLOR0;
};

struct VS_OUTPUT
{
	float3 vPosition	:WORLDPOS;
	//float2 vTexCoord	:TEXCOORD0;
};
struct HS_OUTPUT
{
    float3 vPosition	: BEZIERPOS;
	//float2 vTexCoord	: TEXCOOR0;
};
struct DS_OUTPUT
{
	float4 vPosition	: SV_POSITION;
	float3 v3DPos		: WORLDPOS;
};
struct GS_OUTPUT
{
	float4 vPosition	: SV_POSITION;
	float4 vColor		:COLOR0;
	float3 v3DPos		:WORLDPOS;
	float3 vNormal		:NORMAL;
	//float2 vTexCoord	:TEXCOORD0;
};
struct PS_INPUT
{
	float4 vPosition	: SV_POSITION;
	float4 vColor		:COLOR0;
};


//--------------------------------------------------------------------------
//Vertex shader
//--------------------------------------------------------------------------
VS_OUTPUT SmoothVS(VS_INPUT2 In)
{	
	VS_OUTPUT result;
    result.vPosition=mul(float4(In.vPosition, 1.0f), g_mWorld);
    return result;  
}


//----------------------------------------------------------------------------------
//	Basic Hull Shader
//
//----------------------------------------------------------------------------------

struct HS_CONSTANT_DATA_OUTPUT
{
    float Edges[4]			: SV_TessFactor;
    float Inside[2]			: SV_InsideTessFactor;
};


HS_CONSTANT_DATA_OUTPUT MyConstantsHS( InputPatch<VS_OUTPUT, 16> ip,
                                                 uint PatchID : SV_PrimitiveID )
{	
    HS_CONSTANT_DATA_OUTPUT Output;
    
    float TessAmount = 16.0f;
    Output.Edges[0] = Output.Edges[1] = Output.Edges[2] = Output.Edges[3] = TessAmount;
    Output.Inside[0] = Output.Inside[1] = TessAmount;
   
    return Output;
}
//----------------------------------------------------------------------------------
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(16)]
[patchconstantfunc("MyConstantsHS")]
HS_OUTPUT SmoothHS( InputPatch<VS_OUTPUT, 16> p, 
                                     uint i : SV_OutputControlPointID,
                                     uint PatchID : SV_PrimitiveID )
{
    
    
    HS_OUTPUT Output;
    Output.vPosition=p[i].vPosition;
	//Output.vTexCoord=p[i].vTexCoord;
    return Output;
}



//--------------------------------------------------------------------
//Domain shader
//--------------------------------------------------------------------


float3 calculateFormula( float S, float T){
	float3 pnt = float3(0,0,0);

	float u = S*6.28;
	float v = (T-.5)*3.14;

	pnt.x = cos(v) * cos(u) ;
	pnt.y = cos(v) *sin(u) ;
	pnt.z = sin(v);

	return pnt;
}

[domain("quad")]
DS_OUTPUT SmoothDS( HS_CONSTANT_DATA_OUTPUT input, 
                        float2 UV : SV_DomainLocation,
                        const OutputPatch<HS_OUTPUT, 16> bezpatch )
{
    DS_OUTPUT Output;
	
	//float3 B1=(bezpatch[3].vPosition-bezpatch[0].vPosition)*UV.x+bezpatch[0].vPosition;
	//float3 B2=(bezpatch[12].vPosition-bezpatch[0].vPosition)*UV.x+bezpatch[12].vPosition;
	
	/*float3 WorldPos=(B2-B1)*UV.y+B1;*/

	//calculate bezier pos
	//calculateFormula(UV.x, UV.y);
	float3 WorldPos = calculateFormula(UV.x, UV.y);
	Output.v3DPos =WorldPos;


	Output.vPosition =mul(float4(WorldPos,1.0f), g_mView);	
	
    return Output;    
}

//--------------------------------------------------------------------
//Geometry shader
//--------------------------------------------------------------------


//---------------------------------------------------------------------------

[maxvertexcount(3)]
void SmoothGS( triangle DS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> triStream )
{
	float3 faceEdgeA = input[2].vPosition - input[0].vPosition;
    float3 faceEdgeB = input[1].vPosition - input[0].vPosition;
    float3 faceNormal =cross(faceEdgeA, faceEdgeB);
    faceNormal = normalize( faceNormal );
	GS_OUTPUT vP0 = (GS_OUTPUT)0;
	vP0.vNormal=faceNormal;
	vP0.vPosition = input[0].vPosition;
	vP0.vColor =float4(0,0,0,1);
	vP0.v3DPos =input[0].v3DPos;
	vP0.vNormal=faceNormal;
	//vP0.vTexCoord = input[0].vTexCoord;
    triStream.Append( vP0 );

	vP0.vPosition = input[1].vPosition;
	vP0.v3DPos =input[1].v3DPos;
	//vP0.vTexCoord = input[1].vTexCoord;
	triStream.Append( vP0 );

	vP0.vPosition = input[2].vPosition;
	vP0.v3DPos =input[2].v3DPos;
	//vP0.vTexCoord = input[2].vTexCoord;
	triStream.Append( vP0 );
	triStream.RestartStrip();
}

//--------------------------------------------------------------------
//Geometry shader
//--------------------------------------------------------------------

//---------------------------------------------

float4 calcPhongLighting(float4 vColor, float3 N, float3 Pos )
{

	float3 lightDir, Reflection, eyeV;
	float4 allLightColor = float4(0,0,0,1);
	float4 LColor;
	float lDistance;
	for (int i = 0;i < NUM_LIGHTS; i++){
		lightDir = g_vLightPos[i] - Pos;
		lDistance = length(lightDir);
		lDistance = (1.0 /(1.0 + (0.25 * lDistance * lDistance)));
		lightDir = normalize(lightDir);
		Reflection = reflect( lightDir, N);
		eyeV = normalize(Pos-g_vEyePos);
		LColor = g_vLightColor[i];
		float4 Id = g_Diffuse * saturate( dot(N,lightDir) ) * vColor;
		float4 Is = g_Specular * pow( saturate(dot(Reflection,eyeV)), 20 ) * vColor;
		allLightColor+= (Id*lDistance + Is) * LColor;
	}

    return g_Ambient + allLightColor;
}

float4 SmoothPS(GS_OUTPUT In) : SV_TARGET
{
	
	return calcPhongLighting(g_Ambient, normalize(In.vNormal), In.v3DPos); 
	//return float4(0,0,0,1);
}

