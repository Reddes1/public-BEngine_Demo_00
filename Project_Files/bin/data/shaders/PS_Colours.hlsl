#include "Shader_Include.hlsli"

float4 PS(PixelIn pixIn) : SV_Target0
{
	//Samples texture for original colour
	float4 c = Texture.Sample(samPointWrap, pixIn.texCoord);

	//If flagged, will greyscale the colour with strength multiplier at the end (lower = darker greys)
#ifdef GREYSCALE
	c.rgb = (.r + c.g + c.b) / 3.0f * 1.f;
	return c;
#endif
	//Sets and returns solid colour (with original alpha) based on flag
#ifdef RETURN_RED
	return float4(1.f, 0.f, 0.f, c.a);
#endif
#ifdef RETURN_GREEN
	return float4(0.f, 1.f, 0.0f, c.a);
#endif
#ifdef RETURN_BLUE
	return float4(0.f, 0.f, 1.f, c.a);
#endif
	//Custom colour with optional alpha adjustment (adjust as required)
#ifdef RETURN_CUSTOM
	return float4(0.f, 0.5f, 1.f, c.a * 1.f);
#endif

	//Returns sampled colour unaltered
	return c;
}