#include "Shader_Include.hlsli"

float4 glowColour = float4(1.f, 0.f, 0.f, 1.f);
float alphaThreshold = 0.2f;
float glowIntensity = 1.f;

float4 PS(PixelIn pixIn) : SV_Target0
{
    // Samples texture for original color
    float4 c = Texture.Sample(samPointWrap, pixIn.texCoord);

    // Detect edges by alpha testing with a lower threshold
    float edgeFactor = smoothstep(alphaThreshold - 0.1f, alphaThreshold + 0.1f, c.a);

    // Calculate glow
    float4 glow = glowColour * edgeFactor * glowIntensity;

    // Combine original color with glow
    float4 finalColour = c + glow;

    // Keep original alpha
    finalColour.a = c.a;

    float speedMod = 3.f;
    float aTime = sin(m_GameTime * speedMod);
    float nAlpha = 0.5f * (aTime + 1.f);
    finalColour.a *= nAlpha;

    // Return final color
    return finalColour;
}