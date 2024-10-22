#include "Mgr_TextureResources.h"

#include "DDSTextureLoader.h"		//Texture Loading
#include "DirectXHelpers.h"			//SRV Creation

#include "Utils/Utils_General.h"
#include "Utils/Utils_Debug.h"
#include "Utils/Utils_D3D_Debug.h"
#include "Utils/Utils_RapidJSON.h"

Mgr_TextureResources::Mgr_TextureResources()
{
	//Reserve safe amount of space for SRVs
	m_SRVHeaps.reserve(HEAP_RESERVE);
}

Mgr_TextureResources::~Mgr_TextureResources()
{
	//Clear map
	m_SprTexResources.clear();
	//Release heaps
	m_SRVHeaps.clear();
}

bool Mgr_TextureResources::LoadTexturesFromManifest(std::string& manifestFP, unsigned manifestIndex, unsigned targetHeapIndex, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload)
{
	msg_assert(targetHeapIndex < m_SRVHeaps.size(), "LoadTexturesFromManifest(): Heap Index OOR!");

	//Load manifest document
	rapidjson::Document manifestDoc;
	ParseNewJSONDocument(manifestDoc, manifestFP);

	//Get target heap data for this manifest
	SRVData& srvData = m_SRVHeaps[targetHeapIndex];

	//Get the texture array at target manifest index
	rapidjson::Value& tArr = manifestDoc["Manifests"][manifestIndex]["Textures"].GetArray();

	//Cycle through the array, loading its texture and associated resources
	for (unsigned i(0); i < tArr.Size(); ++i)
	{
		//Create new SpriteTexture resource to load data into
		std::unique_ptr<SpriteTexture> newST = std::make_unique<SpriteTexture>();

		//Get resource data
		newST->m_Name = tArr[i]["Texture_Name"].GetString();

		//Get texture filepath and attempt to load
		std::wstring texFP = StringtoWString(tArr[i]["Texture_Filepath"].GetString());
		if (!(LoadTexture(newST, srvData, texFP, d3dDevice, resourceUpload)))
		{
			//Failed to load
			newST.release();
			return false;
		}
		//Get frames filepath and attempt to load
		std::string fp = tArr[i]["Frames_Filepath"].GetString();
		if (!(LoadFrameData(newST, fp)))
		{
			//Failed to load
			newST.release();
			return false;
		}
		//If animation filepath found, attempt to load from it
		if (tArr[i].HasMember("Animations_Filepath"))
		{
			fp = tArr[i]["Animations_Filepath"].GetString();
			if (!(LoadAnimationData(newST, fp)))
			{
				//Failed to load
				newST.release();
				return false;
			}
		}

		//All loading stages done, load into resource map
		m_SprTexResources[newST->m_Name] = std::move(newST);
	}

	//Loading Done
	return true;
}

bool Mgr_TextureResources::LoadSingleTexture(std::string& textureName, std::wstring& textureFP, std::string& framesFP, unsigned targetHeapIndex, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload)
{
	msg_assert(targetHeapIndex < m_SRVHeaps.size(), "LoadTextureFromFile(): Heap Index OOR!");

	//Get target heap for brevity
	SRVData& srvData = m_SRVHeaps[targetHeapIndex];

	//Create new SpriteTexture resource to load data into
	std::unique_ptr<SpriteTexture> newST = std::make_unique<SpriteTexture>();

	//Store name
	newST->m_Name = textureName;

	//Attempt to load from texture filepath
	if (!(LoadTexture(newST, srvData, textureFP, d3dDevice, resourceUpload)))
	{
		//Failed to load
		newST.release();
		return false;
	}
	//Attempt to load from frame filepath
	if (!(LoadFrameData(newST, framesFP)))
	{
		//Failed to load
		newST.release();
		return false;
	}

	//All loading stages done, load into resource map
	m_SprTexResources[newST->m_Name] = std::move(newST);

	//Loading Done
	return true;
}

bool Mgr_TextureResources::LoadSingleTexture(std::string& textureName, std::wstring& textureFP, std::string& framesFP, std::string& animsFP, unsigned targetHeapIndex, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload)
{
	msg_assert(targetHeapIndex < m_SRVHeaps.size(), "LoadTextureFromFile(): Heap Index OOR!");

	//Get target heap for brevity
	SRVData& srvData = m_SRVHeaps[targetHeapIndex];

	//Create new SpriteTexture resource to load data into
	std::unique_ptr<SpriteTexture> newST = std::make_unique<SpriteTexture>();

	//Store name
	newST->m_Name = textureName;

	//Attempt to load from texture filepath
	if (!(LoadTexture(newST, srvData, textureFP, d3dDevice, resourceUpload)))
	{
		//Failed to load
		newST.release();
		return false;
	}
	//Attempt to load from frame filepath
	if (!(LoadFrameData(newST, framesFP)))
	{
		//Failed to load
		newST.release();
		return false;
	}
	if (!(LoadAnimationData(newST, animsFP)))
	{
		//Failed to load
		newST.release();
		return false;
	}

	//All loading stages done, load into resource map
	m_SprTexResources[newST->m_Name] = std::move(newST);

	//Loading Done
	return true;
}

std::unique_ptr<DirectX::SpriteFont> Mgr_TextureResources::CreateNewFont(std::wstring& fontFP, unsigned targetHeapIndex, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload)
{
	msg_assert(targetHeapIndex < m_SRVHeaps.size(), "LoadFontTexture(): Heap Index OOR!");

	//Get target heap for brevity
	SRVData& data = m_SRVHeaps[targetHeapIndex];

	//If no SRV space left, return nullptr
	if (data.m_Count >= data.m_ResourceDescriptors->Count() - 1)
	{
		msg_assert(false, "CreateNewFont(): Not enough descriptor space left in target heap!");
		return nullptr;
	}

	//Pre-increment count and fix in the return code
	++data.m_Count;
	//Create and return completed resource
	return std::make_unique<DirectX::SpriteFont>(
		d3dDevice,
		resourceUpload,
		fontFP.c_str(),
		data.m_ResourceDescriptors->GetCpuHandle(data.m_Count-1), //Re-align count
		data.m_ResourceDescriptors->GetGpuHandle(data.m_Count-1)  //Re-align count
	);
}

const Mgr_TextureResources::SRVData& Mgr_TextureResources::GetHeapAtIndex(unsigned index)
{
	msg_assert(index < m_SRVHeaps.size(), "GetResourceHeap(): Heap Index OOR!");
	return m_SRVHeaps[index];
}

SpriteTexture* Mgr_TextureResources::FindTextureData(std::string& texName)
{
	//Iterate through cotainer, and return texture if found, else return nullptr
	TextureResourceMap::iterator it = m_SprTexResources.find(texName);
	if (it != m_SprTexResources.end())
		return it->second.get();
	else
		return nullptr;
}

bool Mgr_TextureResources::CreateNewHeap(ID3D12Device* d3dDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, D3D12_DESCRIPTOR_HEAP_FLAGS flags, unsigned heapSize)
{
	//Get target index (correct after inserting new heap)
	size_t index = m_SRVHeaps.size();
	//Insert new container
	m_SRVHeaps.push_back(SRVData());
	//Setup heap at target index
	m_SRVHeaps[index].m_ResourceDescriptors = std::make_unique<DirectX::DescriptorHeap>(
		d3dDevice,
		heapType,
		flags,
		heapSize
	);

	//Job done
	return true;
}

bool Mgr_TextureResources::LoadTexture(std::unique_ptr<SpriteTexture>& data, SRVData& srvData, std::wstring& textureFP, ID3D12Device* d3dDevice, DirectX::ResourceUploadBatch& resourceUpload)
{
	//Check if the maximum amount of descriptors as been hit before trying to insert a new resource
	if (srvData.m_Count >= srvData.m_ResourceDescriptors->Count() - 1)
	{
		//No space to dont try to create new resource
		msg_assert(false, "LoadTextureFromFile(): Not enough descriptor space left in target heap!");
		return false;
	}

	//Attempt to create texture resource from target file location
	ThrowIfFailed(CreateDDSTextureFromFile(
		d3dDevice,
		resourceUpload,
		static_cast<const wchar_t*>(textureFP.c_str()),
		data->m_TextureResource.ReleaseAndGetAddressOf())
	);

	//Attempt to create a new SRV entry at target heap and index
	DirectX::CreateShaderResourceView(
		d3dDevice,
		data->m_TextureResource.Get(),
		srvData.m_ResourceDescriptors->GetCpuHandle(srvData.m_Count)
	);

	//Store heap address
	data->m_Heap = srvData.m_ResourceDescriptors.get();
	//Store index of texture in the heap (post incrementing for next possible resource)
	data->m_HeapIndex = (unsigned)srvData.m_Count++;

	//Texture loaded into heap and updated
	return true;
}

bool Mgr_TextureResources::LoadFrameData(std::unique_ptr<SpriteTexture>& data, std::string& framesFP)
{
	//Parse file
	rapidjson::Document doc;
	ParseNewJSONDocument(doc, framesFP);

	//Load individual frame data
	data->m_Frames.reserve(doc["frames"].GetArray().Size());
	for (auto& a : doc["frames"].GetArray())
	{
		//Store new frame
		RECT newFrame = {
			a["frame"]["x"].GetInt(),
			a["frame"]["y"].GetInt(),
			a["frame"]["x"].GetInt() + a["frame"]["w"].GetInt(),
			a["frame"]["y"].GetInt() + a["frame"]["h"].GetInt()
		};
		data->m_Frames.push_back(newFrame);

		DirectX::XMFLOAT2 origin;
		//If pivoting enabled, store origin value adjusted by pivot
		if (a.HasMember("pivot"))
		{
			//Store accociated origin
			origin =
			{
				a["spriteSourceSize"]["w"].GetFloat() * a["pivot"]["x"].GetFloat(),
				a["spriteSourceSize"]["h"].GetFloat() * a["pivot"]["y"].GetFloat()
			};
		}
		//Not pivot assigned, so do a safe adjustment to centre for origin data
		else
		{
			origin =
			{
				a["spriteSourceSize"]["w"].GetFloat() * 0.5f,
				a["spriteSourceSize"]["h"].GetFloat() * 0.5f
			};
		}

		data->m_Origins.push_back(origin);
	}

	//Frames loaded
	return true;
}

bool Mgr_TextureResources::LoadAnimationData(std::unique_ptr<SpriteTexture>& data, std::string& animFP)
{
	//Parse file
	rapidjson::Document doc;
	ParseNewJSONDocument(doc, animFP);

	//Reserve space of animations
	data->m_Animations.reserve(doc["Animations"].GetArray().Size());

	//Linear animations are defined
	unsigned frameCount = 0;
	unsigned indexCount = 0;
	//For each animation
	for (unsigned int i(0); i < doc["Animations"].Size(); ++i)
	{
		AnimationData newAnim;
		newAnim.m_AnimationID = i;

		//Animator manages and uses the data different 
		AnimationData::AnimID animID = (AnimationData::AnimID)doc["Animations"][i]["Type ID"].GetUint();
		switch (animID)
		{
		case(AnimationData::AnimID::LINEAR_FRAMES):

			//
			//Loaded Data
			//

			//Store name and type
			newAnim.m_Name = doc["Animations"][i]["Name"].GetString();
			newAnim.m_TypeID = animID;
			//Set animation length
			newAnim.m_AnimationLength = doc["Animations"][i]["Frame Count"].GetUint();
			//Set animation speed
			newAnim.m_Speed = doc["Animations"][i]["FPS"].GetFloat();

			//
			//Additional Setup
			//

			//Set start frame to previous end frame (0 in first loop)
			newAnim.m_StartFrame = frameCount;
			//Update count to align with start of next possible animation with animation length
			frameCount += newAnim.m_AnimationLength;
			//Set end frame as count, realigning slightly to be accurate
			newAnim.m_EndFrame = frameCount - 1;
			break;

		case(AnimationData::AnimID::NON_LINEAR_FRAMES):

			//
			//Loaded Data
			//

			//Set name, type and speed
			newAnim.m_Name = doc["Animations"][i]["Name"].GetString();
			newAnim.m_TypeID = animID;
			newAnim.m_Speed = doc["Animations"][i]["FPS"].GetFloat();
			//Store animation frame indexes
			for (auto& f : doc["Animations"][i]["Animation Frames"].GetArray())
				newAnim.m_FrameIndexes.push_back(f.GetInt());
			break;
		}

		//Set internal index id and then store
		newAnim.m_ContainerIndex = indexCount++;
		data->m_Animations.push_back(newAnim);
	}	

	//Animations done loading
	return true;
}