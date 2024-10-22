//*********************************************************************************\\
//  
// Collection of common config values, static data, enums and defines that are
// generally used through-out the game in some form or fashion that don't belong
// in any particular/sensible other home.
// 
// This typically includes default values, fixed filepaths, and most notably
// enums to describe various different resources like manager internal resources.
// 
// Free to modify as required.
//
//*********************************************************************************\\

#pragma once

//================================================================================\\
//General Configs
//================================================================================\\

//Default window dims
static int g_DEFAULT_WIN_X = 1920;
static int g_DEFAULT_WIN_Y = 1080;

//Default number of frame resources (circular array of resources used to keep CPU/GPU non-idle)
static const unsigned g_NUM_FRAME_RESOURCES = 3;

//================================================================================\\
//Manifest Filepaths
//================================================================================\\

#define BE_TEXTURE_MANIFEST_FP "../../BEngine/Resources/Manifests/Texture_Manifest.json"
#define BE_FONT_MANIFEST_FP "../../BEngine/Resources/Manifests/Font_Manifest.json"

//================================================================================\\
//Manager Enums (Accessed via BE_ManagerEnums)
//================================================================================\\

namespace BE_ManagerEnums
{
	//================================================================================\\
	// Mgr_Graphics Resources
	// 
	// Each resource is accessable via index and should be loaded in the order of
	// index description noted here to prevent unintended behaviour through
	// incorrect resource access. 
	//================================================================================\\

	enum class RootSigIndexes : unsigned
	{
		DEFAULT,
		COUNT
	};

	enum class SpritebatchIndexes : unsigned
	{
		MAIN_SCENE,
		MAIN_SCENE_TRANSFORMED,
		COLOUR_REPLACEMENT,
		CUSTOM_OUTLINE_GLOW,
		COUNT
	};

	enum class SpritefontIndexes : unsigned
	{
		CALABRI_12,
		CALABRI_14,
		CALABRI_16,
		CALABRI_18,
		CALABRI_20,
		CALABRI_22,
		COUNT
	};

	//================================================================================\\
	// Mgr_TextureResources
	//   
	// Each resource is accessable via index and should be loaded in the order of
	// index description noted here to prevent unintended behaviour through
	// incorrect resource access. 
	//================================================================================\\

	enum class DescHeapIndexes : unsigned
	{
		DEFAULT_SRV,
		COUNT
	};

	//================================================================================\\
	// Mgr_Modes Resources
	//   
	// Each resource is accessable via index and should be loaded in the order of
	// index description noted here to prevent unintended behaviour through
	// incorrect resource access. 
	//================================================================================\\

	enum class ModeIndexes : unsigned
	{
		TEST_MODE,
		COUNT
	};

	//================================================================================\\
	// Game Resources
	//================================================================================\\
	
	//Defines different configurations for render function trio (Pre/Post/Resize)
	enum class RenderConfigDescriptions : unsigned
	{
		DEFAULT,
		COUNT
	};
}

//================================================================================\\
//Actor Enums (Accessed via BE_ActorEnums)
//================================================================================\\

namespace BE_Actor_Enums
{
	/*
		These are broad category IDs that describe what the object. This should directly relate
		to Actor2D_Interface ID "m_CategoryID"
	*/
	enum class CategoryIDs : unsigned
	{
		//Terrain Types

		STATIC_TERRAIN_TYPE_A,
		PLATFORM_TYPE_A,

		//Object Types

		PROP_TYPE_A,		//Rect-based,
		PROP_TYPE_B			//Circle-based objects
	};
}