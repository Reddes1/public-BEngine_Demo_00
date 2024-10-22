//*********************************************************************************\\
// 
// Manager level defintions for resources. Template available in "Example_and_Templates".
// Free to modify as required.
//
//*********************************************************************************\\

#pragma once

#define FONT_MANIFEST_FP "data/files/Font_Manifests.json"
#define TEXTURE_MANIFEST_FP "data/files/Texture_Manifests.json"

namespace Definitions
{
	enum DescriptorHeapsDesc : unsigned
	{
		//Engine Baseline (Try not to replace)
		DEFAULT_SRV,
		//Custom Heaps below this point
		COUNT
	};

	enum SpritebatchDesc : unsigned
	{
		//Engine Baseline (Try not to replace)
		VANILLA,
		VANILLA_TRANSFORMED,
		COLOUR_SHADER,
		//Custom spritebatches below this point
		COUNT
	};

	enum SpritefontDesc : unsigned
	{
		CALABRI_12,
		CALABRI_14,
		CALABRI_16,
		CALABRI_18,
		CALABRI_20,
		CALABRI_22,
		COUNT
	};

	enum ModeDesc : unsigned
	{
		TEST_MODE,
		COUNT
	};
}
