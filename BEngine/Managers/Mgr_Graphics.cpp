#include "Mgr_Graphics.h"

#include "Actors/Actor2D_Interface.h"

#include "Utils/Utils_Debug.h"
#include "Utils/Utils_D3D_Debug.h"

#include "Game.h"

Mgr_Graphics::Mgr_Graphics(ID3D12Device* d3dDevice)
{
	//Initialise common states
	m_CommonStates = std::make_unique<DirectX::CommonStates>(d3dDevice);
	//Initialise graphics memory
	m_GraphicsMemory = std::make_unique<DirectX::GraphicsMemory>(d3dDevice);
}

ID3D12RootSignature* Mgr_Graphics::GetRootSignature(unsigned index)
{
	msg_assert(index < m_RootSigs.size(), "GetRootSignature(): Index OOR!");
	return m_RootSigs[index].Get();
}

W_SpriteBatch* Mgr_Graphics::GetSpritebatch(unsigned index)
{
	msg_assert(index < m_Spritebatches.size(), "GetSpritebatch(): Index OOR!");
	return m_Spritebatches[index].get();
}

std::vector<Actor2D_Interface*>& Mgr_Graphics::GetRenderGroup(unsigned index)
{
	msg_assert(index < m_RenderGroups.size(), "GetRenderGroup(): Index OOR!");
	return m_RenderGroups[index];
}

DirectX::SpriteFont* Mgr_Graphics::GetSpritefont(unsigned index)
{
	msg_assert(index < m_Spritefonts.size(), "GetSpritefont(): Index OOR!");
	return m_Spritefonts[index].get();
}

unsigned Mgr_Graphics::InsertNewRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSig)
{
	m_RootSigs.push_back(std::move(rootSig));
	return static_cast<unsigned>(m_Spritebatches.size() - 1);
}

unsigned Mgr_Graphics::InsertNewSpritebatch(W_SpriteBatch* data)
{
	m_Spritebatches.push_back(std::unique_ptr<W_SpriteBatch>(data));
	return static_cast<unsigned>(m_Spritebatches.size() - 1);
}

unsigned Mgr_Graphics::InsertNewSpritebatch(std::unique_ptr<W_SpriteBatch>& data)
{
	m_Spritebatches.push_back(std::move(data));
	return static_cast<unsigned>(m_Spritebatches.size() - 1);
}

void Mgr_Graphics::SyncRenderGroupCount()
{
	m_RenderGroups.clear();
	m_RenderGroups.reserve(m_Spritebatches.size());

	for (unsigned i(0); i < m_Spritebatches.size(); ++i)
	{
		m_RenderGroups.push_back(std::vector<Actor2D_Interface*>());
		m_RenderGroups[i].reserve(GROUP_RESERVE_COUNT);
	}
}

void Mgr_Graphics::SubmitToRenderGroup(unsigned index, Actor2D_Interface* actor)
{
	msg_assert(index <= m_Spritebatches.size(), "SubmitToRenderGroup(): Index OOR");

	m_RenderGroups[index].push_back(actor);
}

void Mgr_Graphics::DrawBatch(System& sys, unsigned index, ID3D12GraphicsCommandList* cmdList)
{
	msg_assert(index <= m_Spritebatches.size(), "DrawBatch(): Index OOR");

	//Open batch to drawing
	m_Spritebatches[index]->BeginBatch(cmdList);
	//Submit sync'd render group for draw
	m_Spritebatches[index]->Render(sys, m_RenderGroups[index]);
	//Close batch
	m_Spritebatches[index]->EndBatch();
}

void Mgr_Graphics::ClearRenderGroups()
{
	for (auto& a : m_RenderGroups)
		a.clear();
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> Mgr_Graphics::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

bool Mgr_Graphics::BuildRootSig_Default(ID3D12Device* d3dDevice, Microsoft::WRL::ComPtr<ID3D12RootSignature>* rootSig)
{
	enum RootParams : unsigned
	{
		TextureSRV,
		FixedConstBuffer,
		FlexibleConstBuffer,
		COUNT
	};

	//Setup flags
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	// Include texture and srv
	CD3DX12_DESCRIPTOR_RANGE textureSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// Create root parameters and initialize first (constants)
	CD3DX12_ROOT_PARAMETER rootParameters[RootParams::COUNT] = {};
	CD3DX12_DESCRIPTOR_RANGE textureSampler(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

	//Setup basic 1 texture, 3 CBV root parameter

	rootParameters[RootParams::TextureSRV].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);		 //t0
	rootParameters[RootParams::FixedConstBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);			 //b0									 //b1
	rootParameters[RootParams::FlexibleConstBuffer].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);		 //b1

	/*
		Get some static samplers to include in the root signature.
		This should align to registers as such:
		- s0 : Point Wrap
		- s1 : Point Clamp
		- s2 : Linear Wrap
		- s3 : Linear Clamp
		- s4 : Anisotropic Wrap
		- s5 : Anisotropic Clamp
	*/
	auto samplers = GetStaticSamplers();

	//Build root signature from defined components
	CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};
	rsigDesc.Init(
		(UINT)RootParams::COUNT,
		rootParameters,
		(UINT)samplers.size(),
		samplers.data(),
		rootSignatureFlags
	);

	//Serialise the root signature
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(
		&rsigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(),
		errorBlob.GetAddressOf()
	);
	if (errorBlob != nullptr)
	{
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	//Finish creation 
	ThrowIfFailed(d3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(rootSig->GetAddressOf())
	));

	return true;
}
