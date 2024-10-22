#include "FrameResources.h"

FrameResource::FrameResource(ID3D12Device* d3dDevice)
{
	ThrowIfFailed(d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_CommandListAlloc.GetAddressOf())));
}

FrameResource::FrameResource(ID3D12Device* d3dDevice, unsigned frameBufferCount)
{
	ThrowIfFailed(d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_CommandListAlloc.GetAddressOf())));

	m_FrameBuffer = std::make_unique<UploadBuffer<CBuffer_PerFrame>>(d3dDevice, 1, true);
}
