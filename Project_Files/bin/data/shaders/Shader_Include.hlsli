
//
//Definitions
//


//
//Layouts
//

struct VertexIn
{
    float4 color : COLOR0;
    float2 texCoord : TEXCOORD0;
    float4 position : SV_Position;
};

struct PixelIn
{
    float4 color    : COLOR0;
    float2 texCoord : TEXCOORD0;
};


//
//Constant Buffers (bX)
//

//Mirrors SB basic parameters
cbuffer Parameters : register(b0)
{
    row_major float4x4 MatrixTransform;
};

//Custom per-frame buffer test
cbuffer PerFrame : register(b1)
{
    float m_GameTime;
    float m_DeltaTime;
}

//
//Samplers (sX)
//

SamplerState samPointWrap        : register(s0);
SamplerState samPointClamp       : register(s1);
SamplerState samLinearWrap       : register(s2);
SamplerState samLinearClamp      : register(s3);
SamplerState samAnisotropicWrap  : register(s4);
SamplerState samAnisotropicClamp : register(s5);

//
//SRVs (tX)
//

Texture2D<float4> Texture : register(t0);
