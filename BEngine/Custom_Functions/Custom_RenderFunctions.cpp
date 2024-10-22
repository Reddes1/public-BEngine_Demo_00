#include "Custom_RenderFunctions.h"


void Custom_WSpritebatchDraw(W_SpriteBatch* batch, System* sys, std::vector<Actor2D_Interface*>* renderGroup)
{
	//Get CL and current FR
	ID3D12GraphicsCommandList* cmdList = sys->m_Game->GetCommandList().Get();
	FrameResource* currFR = sys->m_Game->GetCurrentFrameResource();

	//Honour intial setup calls (SB setup + internal wrapper setup)
	batch->BeginBatch(cmdList);

	/*
		SB should have its PSO set with the appropriate RootSig as well, so append another CBuffer to slot 2
	*/
	sys->m_Game->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		2,																		//Cbuffer Location (b1)
		currFR->m_FrameBuffer->Resource()->GetGPUVirtualAddress()				//Resource (aligning with cbuffer sig at b1)
	);

	/*
		The batch doesn't need to be in immediate mode for this (as we aren't doing per-object CBuffers here), but if it is, we want
		to maintain draw depths as best as using the actors depth level as a guide. We sort high to low (as per engine standar)
	*/
	if (batch->m_SortMode == DirectX::SpriteSortMode_Immediate)
	{
		//Lambda sort the container based on actor depth value
		std::sort(renderGroup->begin(), renderGroup->end(), [](Actor2D_Interface* a, Actor2D_Interface* b)
		{
			return a->GetActorDepth() > b->GetActorDepth();
		});
	}

	//Render the target group
	for (auto& a : *renderGroup)
		a->Render(*sys, batch->m_Batch.get());

	//End batch
	batch->EndBatch();
}