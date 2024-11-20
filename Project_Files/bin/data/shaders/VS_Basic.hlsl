#include "Shader_Include.hlsli"

void VS(inout VertexIn vs)
{
    vs.position = mul(vs.position, MatrixTransform);
}