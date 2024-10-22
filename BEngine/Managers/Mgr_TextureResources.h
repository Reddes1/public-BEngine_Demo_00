//*********************************************************************************\\
// 
// Manager class for managing texture resources and SRV resources. Creates, stores, 
// and enables access to textures and associated data.
//
//*********************************************************************************\\

#pragma once
#include <unordered_map>
#include "ResourceUploadBatch.h"
#include "DescriptorHeap.h"
#include "SpriteFont.h"

#include "Types/BE_SharedTypes.h"		//SpriteTexture type

class Mgr_TextureResources
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////
	
	static constexpr size_t HEAP_RESERVE = 8;
	static constexpr size_t DEFAULT_SRV_SIZE = 256;

	//Texture map typedef
	typedef std::unordered_map<std::string, std::shared_ptr<SpriteTexture>> TextureResourceMap;
	/*
		Pairs descriptor heap and active tracking of the amount of bound resources together.
		Tracking value allows for proper sequencing of resources automatically when loading resources.
	*/
	struct SRVData
	{
		std::unique_ptr<DirectX::DescriptorHeap> m_ResourceDescriptors;
		size_t m_Count = 0;
	};

	////////////////////
	/// Constructors ///
	////////////////////

	Mgr_TextureResources();
	~Mgr_TextureResources();

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Texture Resource Loading Methods
	//	

	/*
		Loads all texture indexes form a given manifest index in target manifest file. File must be standard engine format (See Texture_Manifest_Example.json).
		Textures frame JSON must be produced/match TexturePacker output format, with Animation format following standard engine format (See Texture_Animation_Example.json).
	*/
	bool LoadTexturesFromManifest(std::string& manifestFP, unsigned manifestIndex, unsigned targetHeapIndex, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload);
	//Loads and generates texture resource, and loads frame information. Uses JSON array file output from TexturePacker.
	//Heap index is automatically allocated during creation and stored in SpriteTexture data.

	//Like LoadTexturesFromManifest, but only the one texture and its resources at a time.
	bool LoadSingleTexture(std::string& textureName, std::wstring& textureFP, std::string& framesFP,
		unsigned targetHeapIndex, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload);
	//Alternate call that provides animation data as well
	bool LoadSingleTexture(std::string& textureName, std::wstring& textureFP, std::string& framesFP,
		std::string& animsFP, unsigned targetHeapIndex, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload);

	/*
		To-Do:
		- Load from texture manifest that needs - ManifestFP which provides - TextureFP, FramesFP and NEW AnimationsFP
	
	*/


	/*
		Loading fonts works a little differently to regular textures (as it needs to be bound to font and heap at the same time), so use this call to bind a font
		file to the target heap, returning a complete spritefont resource that should be held elsewhere (See Mgr_Graphics).
		Internal heap location is managed similarly to other texture resources (automatically assigned heap index).
	*/
	std::unique_ptr<DirectX::SpriteFont> CreateNewFont(std::wstring& fontFP, unsigned targetHeapIndex, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload);

	//
	//Setup
	//

	//Creates a new heap at target location.
	//Will override any existing heap at that location.
	bool CreateNewHeap(ID3D12Device* d3dDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS flags, unsigned heapSize = DEFAULT_SRV_SIZE);

	/////////////////
	/// Accessors ///
	/////////////////

	//Get SRV data at index
	const SRVData& GetHeapAtIndex(unsigned index);

	//Searches for texture data and returns it found (nullptr if not)
	SpriteTexture* FindTextureData(std::string& texName);

private:

	//////////////////
	/// Operations ///
	//////////////////

	//Attempts to load texture into given resource from file
	bool LoadTexture(std::unique_ptr<SpriteTexture>& data, SRVData& srvData, std::wstring& textureFP, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload);
	//Attepts to load frame information about a texture from file
	bool LoadFrameData(std::unique_ptr<SpriteTexture>& data, std::string& framesFP);
	//Optional aspect of the loading process that loads any animation data relating to the frame data previously loaded
	bool LoadAnimationData(std::unique_ptr<SpriteTexture>& data, std::string& animFP);

	////////////
	/// Data ///
	////////////

	//Texture map
	TextureResourceMap m_SprTexResources;

	//Container of SRV Heaps
	std::vector<SRVData> m_SRVHeaps;
};