//*********************************************************************************\\
//  
// FrameResources is used to define resources that need to be used each by each
// frame, and structured as such so that several can be used to keep an array
// of resources on the go keeping idling between the CPU and GPU down as much
// as possible.
// 
// This is customisable as required for the projects needs, but must include
// an CL Allocator and the Fence minimally. Game logic must keep the internal 
// resources updated as required. 
// 
// See "FrameResources_Types.h" for readily defined, visible header for defining
// types, buffers or other resources that need to be included in FrameResources.
// 
//*********************************************************************************\\

#pragma once

//Library Inclusions
#include <wrl.h>						//ComPtr
#include <d3d12.h>						//ID3D12Resource

//Engine Includes
#include "Helpers/UploadBuffer.h"		//For creating resource buffers
#include "Types/FrameResources_Types.h"

struct FrameResource
{
public:

	////////////////////
	/// Constructors ///
	////////////////////

	FrameResource(ID3D12Device* d3dDevice);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource() {}

	//
	//Custom Constructors (relative to resource additions
	//

	FrameResource(ID3D12Device* d3dDevice, unsigned frameBufferCount);

	////////////
	/// Data ///
	////////////

	//
	//Core Data
	//

	/*
		As the allocator cannot be reused/reset till the GPU is done processing the commands,
		each frame gets its own allocator to keep things moving.
	*/
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandListAlloc;
	/*
		Simple fence marker for mark commands up to this point, letting us see if
		the GPU is still usein
	*/
	UINT64 m_Fence = 0;

	//
	//Project Resources Here!
	//

	/*
		Per-Frame buffer for use in with select Spritebatches with custom PSOs.
	*/
	std::unique_ptr<UploadBuffer<CBuffer_PerFrame>> m_FrameBuffer;
};
