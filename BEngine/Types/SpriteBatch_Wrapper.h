//*********************************************************************************\\
// 
// Simple wrapper class that packs possible data used with spritebatch alongside
// a draw type parameter to dictate what data is used in the call. Allows for
// easier state control over spritebatches.
//
//*********************************************************************************\\

#pragma once

//Library Includes
#include "SpriteBatch.h"

//Utilities
#include "Utils/Utils_General.h"

//Engine Includes
#include "Camera/SceneTransform.h"

//Forward Declarations
struct System;
class Actor2D_Interface;

struct W_SpriteBatch
{
	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Draw mode indicator (e.g. what data to include in the draw)
	enum class DrawMode
	{
		BASIC,
		TRANSFORMED
	};
	
	////////////////////
	/// Constructors ///
	////////////////////

	W_SpriteBatch() {}
	~W_SpriteBatch() {}

	//////////////////
	/// Operations ///
	//////////////////

	//Starts draw call based on current draw mode (overriddable by directly accessing batch)
	void BeginBatch(ID3D12GraphicsCommandList* cmdList)
	{
		switch (m_DrawMode)
		{
		case DrawMode::BASIC:
			m_Batch->Begin(cmdList, m_SortMode);
			break;
		case DrawMode::TRANSFORMED:
			m_Transform.Update();
			m_Batch->Begin(cmdList, m_SortMode, m_Transform.GetMatrix());
			break;
		}
	}

	//Render/Submits group of actors with batch
	void Render(System& sys, std::vector<Actor2D_Interface*>& actors);
	//Render/Submits single actor with batch
	void Render(System& sys, Actor2D_Interface* actor);

	//Ends the batch (just an indirect call)
	void EndBatch() { m_Batch->End(); }

	////////////
	/// Data ///
	////////////

	//The batch this wrapper manages
	std::unique_ptr<DirectX::SpriteBatch> m_Batch;
	//Dictates how the begin call is called
	DrawMode m_DrawMode = DrawMode::BASIC;
	//How the batch manages sprites (Note: Immediate_Mode draws sprites immediately)
	DirectX::SpriteSortMode m_SortMode = DirectX::SpriteSortMode::SpriteSortMode_FrontToBack;
	//Transformation/Camera
	//Batches can be transformed as required so keep a specially tailored class for it here
	SceneTransform m_Transform;
	
	/*
		As part of Mgr_Graphics, this can be used for render group based rendering. 
		Whilst the manager provides a basic render function, custom rendering may need
		custom code so enable that via pointer.
	*/
	BindingFunc<void> m_CustomRenderFunc;
	bool m_HasCustomRender = false;
};