#include "Game.h"

//Utilities
#include "Utils/Utils_D3D.h"
#include "Utils/Utils_D3D_Debug.h"
#include "Utils/Utils_Debug.h"
#include "Utils/Utils_General.h"
#include "Utils/Utils_RapidJSON.h"
#include "Helpers/ReadData.h"			//XTK Loading Util for Shaders

//Engine Includes
#include "Custom_Functions/Custom_RenderFunctions.h"

//Project Includes
#include "All_Managers.h"

//Modes
#include "Mode_Demo.h"

Game* Game::m_Game = nullptr;
Game::Game(HINSTANCE hInstance)
	:D3D12_App(hInstance)
{
	assert(m_Game == nullptr);
	m_Game = this;
}

Game::~Game()
{
	if (m_D3DDevice != nullptr)
		FlushCommandQueue();
}

void Game::Update()
{
	//Cycle to next frame resource in circular frame
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % g_NUM_FRAME_RESOURCES;
	m_CurrentFrameResource = m_FrameResources[m_CurrentFrameResourceIndex].get();

	//Has the GPU finished proccessing the commands of the current frame resource?
	//If not, wait here till GPU has completed commands upto this fence point
	if (m_CurrentFrameResource->m_Fence != 0 && m_Fence->GetCompletedValue() < m_CurrentFrameResource->m_Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFrameResource->m_Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	//Update Controller States
	m_GPMgr->Update();
	m_KBMMgr->PreFrameProcess(Game::GetGame()->GetGameTime().DeltaTime());
	//Update Current Mode
	m_ModesMgr->Update(m_SystemPointers);
	//Post frame KBM
	m_KBMMgr->PostFrameProcess();
	//Update frame resources
	Update_FrameResources();
}

void Game::Render()
{
	/*
		If the current modes render prio flag is active, then defer pre and post rendering to the mode 
		(allowing for more granular control on rendering)
	*/
	if (m_ModesMgr->GetActiveModeFlags().m_DefferedRendering)
	{
		m_ModesMgr->Render(m_SystemPointers);
	}
	else
	{
		//Runs current pre-render target (must pair with post-render function)
		std::invoke(m_PreRenderFunc);
		//Call render for current mode
		m_ModesMgr->Render(m_SystemPointers);
		//Runs current post-render target (must pair with pre-render function)
		std::invoke(m_PostRenderFunc);
	}
}

LRESULT Game::MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*
		Before handling any other messages, we wanna intercept here and check for WM_CHAR (Slow Inputs) and
		WM_INPUT (Raw Fast Inputs) and pass them to the appropriate function calls.
	*/
	switch (msg)
	{
	case WM_CHAR:
		m_ModesMgr->ProcessKey(static_cast<char>(wParam));
		break;

	case WM_INPUT:
		m_KBMMgr->MessageEvent((HRAWINPUT)lParam);
		break;
	}

	/*
		Small fix that prevents the alt key release causing program freezing due to it being a menu
		key. This basically stalls the program and have delta time ramifcations.
		Review if more comphensive solution needed.
	*/
	if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
		return 0;

	//Now handle window messages
	switch (msg)
	{
		//Closing Events (window closing + destroy)
	case WM_CLOSE:
		m_WinFlags.m_AppPaused = true;
		HandleWindowClosing();
		DestroyWindow(m_WinData.m_HWND);
		return 0;

	case WM_DESTROY:
		HandleWindowDestroy();
		PostQuitMessage(0);
		return 0;

		//Events where the right mouse is pressed outside of the client area (resize bar)
	case WM_NCRBUTTONDOWN:
		return 0;

		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_WinFlags.m_AppPaused = true;
			m_Timer.Stop();
		}
		else
		{
			m_WinFlags.m_AppPaused = false;
			m_Timer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:

		// Save the new client area dimensions.
		m_WinData.m_CurrWidth = LOWORD(lParam);
		m_WinData.m_CurrHeight = HIWORD(lParam);

		if (m_D3DDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_WinFlags.m_AppPaused = true;
				m_WinFlags.m_AppMinimized = true;
				m_WinFlags.m_AppMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_WinFlags.m_AppPaused = false;
				m_WinFlags.m_AppMinimized = false;
				m_WinFlags.m_AppMaximized = true;
				std::invoke(m_OnResizeFunc);
			}
			else if (wParam == SIZE_RESTORED)
			{
				// Restoring from minimized state?
				if (m_WinFlags.m_AppMinimized)
				{
					m_WinFlags.m_AppPaused = false;
					m_WinFlags.m_AppMinimized = false;
					std::invoke(m_OnResizeFunc);
				}
				// Restoring from maximized state?
				else if (m_WinFlags.m_AppMaximized)
				{
					m_WinFlags.m_AppPaused = false;
					m_WinFlags.m_AppMaximized = false;
					std::invoke(m_OnResizeFunc);
				}
				else if (m_WinFlags.m_AppResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					//std::invoke(m_OnResizeFunc);
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_WinFlags.m_AppPaused = true;
		m_WinFlags.m_AppResizing = true;
		m_Timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_WinFlags.m_AppPaused = false;
		m_WinFlags.m_AppResizing = false;
		m_Timer.Start();
		std::invoke(m_OnResizeFunc);
		return 0;



		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_DEVICECHANGE:
		if (wParam == DBT_DEVICEARRIVAL)
		{
			auto pDev = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
			if (pDev)
			{
				if (pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
				{
					auto pInter = reinterpret_cast<
						const PDEV_BROADCAST_DEVICEINTERFACE>(pDev);
					if (pInter->dbcc_classguid == KSCATEGORY_AUDIO)
					{
						if (&m_AudioMgr)
							m_AudioMgr->OnNewAudioDevice();
					}
				}
			}
		}
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Game::HandleWindowClosing()
{

}

void Game::HandleWindowDestroy()
{

}

bool Game::InitialiseGame()
{
	/////////////////
	/// Pre-amble ///
	/////////////////

	//Call parent Init for window/d3d setup
	if (!D3D12_App::InitialiseD3D())
		return false;

	//Reset CL for init
	ThrowIfFailed(m_CommandList->Reset(m_DirectCommandListAllocator.Get(), nullptr));
	//Get the increment size of a descriptor for this heap type.
	//Hardware specific, so need to query
	m_CbvSrvUavDescriptorSize = m_D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Initialise random seed engine as random seed (can change/recall later as needed)
	MathHelper::SeedRandom(-1);

	//////////////////
	/// Main Setup ///
	//////////////////

	//Allocates managers and resources
	ResourceAllocations();
	//Build frame resources
	BuildFrameResources();
	//Perform preliminary setups
	PreliminaryResourceSetup();

	//Resource loading support for XTK components
	DirectX::ResourceUploadBatch resourceUpload(m_D3DDevice.Get());
	//Start upload session
	resourceUpload.Begin();

	//Build resource heaps
	BuildInitialHeaps();
	//Build root signatures
	BuildRootSignatures();
	//Build spritebatches
	BuildInitialSpritebatches(resourceUpload);
	//Build spritefonts (loads fonts from file)
	BuildInitialSpritefonts(0, resourceUpload);	
	//Load Textures
	LoadInitialTextures(resourceUpload);

	////////////////
	/// Clean Up ///
	////////////////

	//End of setup and loading
	std::future<void> uploadResourcesFinished = resourceUpload.End(m_CommandQueue.Get());
	uploadResourcesFinished.wait();

	//Execute init commands
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsList), cmdsList);

	//Wait for current queue to finish processing
	FlushCommandQueue();

	//////////////////
	/// Post-Setup ///
	//////////////////

	//Setup any pre-project resources
	SetupInitialBlackboardData();
	//Build system container pointers
	BuildSystemContainer();
	//Core setup complete, so now add game modes (can be reliant on previous managers being setup)
	BuildModes(static_cast<size_t>(BE_ManagerEnums::ModeIndexes::TEST_MODE));
	//Finally, build/initialise game specific managers
	BuildGameManagers();

	//Init Complete
	return true;
}

void Game::ResetRenderingFunctions()
{
	m_PreRenderFunc = std::bind(&Game::PreRender_Default, this);
	m_PostRenderFunc = std::bind(&Game::PostRender_Default, this);
	m_OnResizeFunc = std::bind(&Game::OnResize, this);
}

bool Game::ChangeWindowSize_16_9(AspectRatios_16by9 res)
{
	int x, y;
	GetNewWindowSize_16_9(res, x, y);
	m_WinData.m_CurrWidth = x;
	m_WinData.m_CurrHeight = y;

	//Create new rect with target resolution and adjust for size
	RECT rect = { 0, 0, x, y };
	AdjustWindowRect(&rect, m_WinData.m_WinStyle, false);
	//Get new width and height
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	//Reposition window to the middle of the screen
	int xPos = static_cast<int>((GetSystemMetrics(SM_CXSCREEN) - width) * 0.5f);
	int yPos = static_cast<int>((GetSystemMetrics(SM_CYSCREEN) - height) * 0.5f);

	//Set new window position
	SetWindowPos(m_WinData.m_HWND, HWND_TOP, xPos, yPos, width, height, SWP_SHOWWINDOW);

	//Update graphics resources
	std::invoke(m_OnResizeFunc);
	return true;
}

void Game::SetNewRenderFunctions(BE_ManagerEnums::RenderConfigDescriptions renderID)
{
	switch (renderID)
	{
	case BE_ManagerEnums::RenderConfigDescriptions::DEFAULT:
		//Use existing reset render function
		ResetRenderingFunctions();
		break;
	}
}

bool Game::ResourceAllocations()
{
	//
	//Core Managers, Resources & Functionality
	//

	m_TexResourceMgr = std::make_unique<Mgr_TextureResources>();
	m_KBMMgr = std::make_unique<Mgr_InputKBM>();
	m_GPMgr = std::make_unique<Mgr_InputGamepad>();
	m_Box2DPhysics = std::make_unique<Mgr_Box2DPhysics>();
	m_AudioMgr = std::make_unique<Mgr_Audio>();
	m_ModesMgr = std::make_unique<Mgr_Modes>();
	m_GraphicsMgr = std::make_unique<Mgr_Graphics>(m_D3DDevice.Get());
	m_UIMgr = std::make_unique<Mgr_UI>();
	m_Blackboard = std::make_unique<GameBlackboard>();

	//
	//Additional M/R/F Here
	//

	//Setup complete
	return true;
}

bool Game::BuildFrameResources()
{
	unsigned perFrameCount = 1;

	//Create container(s) for circular resource frame + any initial setup required
	for (int i(0); i < g_NUM_FRAME_RESOURCES; ++i)
	{
		m_FrameResources.push_back(std::make_unique<FrameResource>(m_D3DDevice.Get(), perFrameCount));
	}

	return true;
}

bool Game::PreliminaryResourceSetup()
{
	//Initialise audio and KBM managers
	m_AudioMgr->InitAudioEngine(m_WinData.m_HWND);
	m_KBMMgr->RunOnceInit(m_WinData.m_HWND, true, false);
	m_KBMMgr->SetKeyTimersFlag(true);
	m_KBMMgr->SetMouseTimersFlag(true);
	m_GPMgr->SetHeldTimerFlag(true);

	//Bind initial rendering functions
	m_PreRenderFunc = std::bind(&Game::PreRender_Default, this);
	m_PostRenderFunc = std::bind(&Game::PostRender_Default, this);
	//Bind resize function
	m_OnResizeFunc = std::bind(&Game::OnResize, this);

	//Setup complete
	return true;
}

bool Game::BuildInitialHeaps()
{
	//////////////////////
	/// Baseline Setup ///
	//////////////////////
	
	m_TexResourceMgr->CreateNewHeap(
		m_D3DDevice.Get(), 
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		Mgr_TextureResources::DEFAULT_SRV_SIZE
	);

	return true;
}

bool Game::BuildRootSignatures()
{
	//
	//Default Root Sig (Index 0)
	//

	{
		//Create holding root sig
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig;
		//Use a pre-defined build for this signature
		m_GraphicsMgr->BuildRootSig_Default(m_D3DDevice.Get(), &rootSig);
		//Store new signature in manager
		m_GraphicsMgr->InsertNewRootSignature(rootSig);
	}

	return true;
}

bool Game::BuildInitialSpritebatches(DirectX::ResourceUploadBatch& resourceUpload)
{
	////////////////
	/// Presetup ///
	////////////////

	//Setup RTS for spritebatches (using defined formats)
	DirectX::RenderTargetState rtState(m_BackBufferFormat, m_DepthStencilFormat);

	//////////////////////
	/// Baseline Setup ///
	//////////////////////

	//
	//Vanilla SB (No special functions)
	//

	//Create new wrapper and spritebatch to spec
	auto newSB = std::make_unique<W_SpriteBatch>();
	newSB->m_Batch = std::make_unique<DirectX::SpriteBatch>(
		m_D3DDevice.Get(),
		resourceUpload,
		DirectX::SpriteBatchPipelineStateDescription(rtState, &DirectX::CommonStates::NonPremultiplied),
		&m_ScreenViewport
		);
	newSB->m_DrawMode = W_SpriteBatch::DrawMode::BASIC;
	newSB->m_SortMode = DirectX::SpriteSortMode_FrontToBack;
	m_GraphicsMgr->InsertNewSpritebatch(newSB);

	//
	//Vanilla SB 2 (Mirrors above but for use with scene transformation)
	//

	newSB = std::make_unique<W_SpriteBatch>();
	newSB->m_Batch = std::make_unique<DirectX::SpriteBatch>(
		m_D3DDevice.Get(),
		resourceUpload,
		DirectX::SpriteBatchPipelineStateDescription(rtState, &DirectX::CommonStates::NonPremultiplied),
		&m_ScreenViewport
		);
	newSB->m_DrawMode = W_SpriteBatch::DrawMode::TRANSFORMED;
	newSB->m_SortMode = DirectX::SpriteSortMode_FrontToBack;
	m_GraphicsMgr->InsertNewSpritebatch(newSB);

	//
	//Colour Replacement SB (See Macro for Colour)
	//

	{
		//Set macros for shaders (returns red)
		D3D_SHADER_MACRO macros[] =
		{
			"RETURN_RED", "1",
			NULL, NULL
		};

		//Load shaders from file
		auto vs = CompileShader(L"data/shaders/VS_Basic.hlsl", nullptr, "VS", "vs_5_1");
		auto ps = CompileShader(L"data/shaders/PS_Colours.hlsl", macros, "PS", "ps_5_1");

		//Builds PSO description for SB
		DirectX::SpriteBatchPipelineStateDescription desc(rtState, &DirectX::CommonStates::NonPremultiplied);
		//This uses a custom root signature setup so give it over
		desc.customRootSignature = m_GraphicsMgr->GetRootSignature((unsigned)BE_ManagerEnums::RootSigIndexes::DEFAULT);
		desc.customVertexShader = { reinterpret_cast<BYTE*>(vs->GetBufferPointer()), vs->GetBufferSize() };
		desc.customPixelShader = { reinterpret_cast<BYTE*>(ps->GetBufferPointer()), ps->GetBufferSize() };
		newSB = std::make_unique<W_SpriteBatch>();
		newSB->m_Batch = std::make_unique<DirectX::SpriteBatch>(
			m_D3DDevice.Get(),
			resourceUpload,
			desc,
			&m_ScreenViewport
			);
		newSB->m_DrawMode = W_SpriteBatch::DrawMode::TRANSFORMED;
		newSB->m_SortMode = DirectX::SpriteSortMode_FrontToBack;
		m_GraphicsMgr->InsertNewSpritebatch(newSB);
	}


	//
	//Outline Glow
	//

	{
		//Load shaders from file
		auto vs = CompileShader(L"data/shaders/VS_Basic.hlsl", nullptr, "VS", "vs_5_1");
		auto ps = CompileShader(L"data/shaders/PS_Outline.hlsl", nullptr, "PS", "ps_5_1");

		//Builds PSO description for SB
		DirectX::SpriteBatchPipelineStateDescription desc(rtState, &DirectX::CommonStates::NonPremultiplied);
		//This uses a custom root signature setup so give it over
		desc.customRootSignature = m_GraphicsMgr->GetRootSignature((unsigned)BE_ManagerEnums::RootSigIndexes::DEFAULT);
		desc.customVertexShader = { reinterpret_cast<BYTE*>(vs->GetBufferPointer()), vs->GetBufferSize() };
		desc.customPixelShader = { reinterpret_cast<BYTE*>(ps->GetBufferPointer()), ps->GetBufferSize() };
		newSB = std::make_unique<W_SpriteBatch>();
		newSB->m_Batch = std::make_unique<DirectX::SpriteBatch>(
			m_D3DDevice.Get(),
			resourceUpload,
			desc,
			&m_ScreenViewport
			);
		newSB->m_DrawMode = W_SpriteBatch::DrawMode::BASIC;
		newSB->m_SortMode = DirectX::SpriteSortMode_Immediate;
		m_GraphicsMgr->InsertNewSpritebatch(newSB);
	}

	//Spritebatches create, now need to sync up render group count
	m_GraphicsMgr->SyncRenderGroupCount();

	///////////////////////
	/// Post-Init Setup ///
	///////////////////////

	//
	//Outline Glow
	//

	//Need to bind render function to the target spritebatch as it needs custom behaviour
	{
		W_SpriteBatch* sb = m_GraphicsMgr->GetSpritebatch((unsigned)BE_ManagerEnums::SpritebatchIndexes::CUSTOM_OUTLINE_GLOW);
		std::vector<Actor2D_Interface*>* renderGroup = &m_GraphicsMgr->GetRenderGroup((unsigned)BE_ManagerEnums::SpritebatchIndexes::CUSTOM_OUTLINE_GLOW);
		sb->m_HasCustomRender = true;
		sb->m_CustomRenderFunc = std::bind(Custom_WSpritebatchDraw, sb, &m_SystemPointers, renderGroup);
	}

	return true;
}

bool Game::BuildInitialSpritefonts(unsigned entryIndex, DirectX::ResourceUploadBatch& resourceUpload)
{
	//Parse font manifest
	rapidjson::Document fontDoc;
	ParseNewJSONDocument(fontDoc, std::string(BE_FONT_MANIFEST_FP));

	//Get heap target and confirm validity
	unsigned heapIndex = fontDoc["Manifests"][entryIndex]["Heap Index"].GetUint();
	//Grab hold of manifest array
	const rapidjson::Value& arr = fontDoc["Manifests"][entryIndex]["Fonts"].GetArray();

	//Get fonts containers from graphics manager to insert new resources
	std::vector<std::unique_ptr<DirectX::SpriteFont>>& fonts = m_GraphicsMgr->GetSpritefonts();

	//For each font in target manifest index, load into target heap
	for (unsigned i(0); i < arr.Size(); ++i)
	{
		//Grab filepath
		std::wstring fp = StringtoWString(std::string(arr[i]["Font Filepath"].GetString()));
		
		//Load new texture into target heap, and store new font in graphics manager
		fonts.push_back(m_TexResourceMgr->CreateNewFont(
			fp,
			heapIndex,
			m_D3DDevice.Get(),
			resourceUpload
		));
	}

	//Job done
	return true;
}

bool Game::LoadInitialTextures(DirectX::ResourceUploadBatch& resourceUpload)
{
	//Load from manifest index 0 into heap 0
	m_TexResourceMgr->LoadTexturesFromManifest(std::string(BE_TEXTURE_MANIFEST_FP), 0, 0, m_D3DDevice.Get(), resourceUpload);

	return true;
}

bool Game::BuildModes(size_t startingMode)
{
	//////////////////////
	/// Add Modes Here ///
	//////////////////////

	//Testing Mode(s)
	m_ModesMgr->AddMode(new Mode_Demo((unsigned)BE_ManagerEnums::ModeIndexes::TEST_MODE));

	//Set the starting mode
	m_ModesMgr->SwitchMode((unsigned)startingMode);

	return true;
}

bool Game::BuildGameManagers()
{
	//
	//Build any game related managers as required here
	//

	//Whilst not strictly a gameplay manager, it is fully configurable for the purposes for gameplay so setup some initial world(s) here
	m_Box2DPhysics->CreateNewWorldData(
		std::string("Test_World_00"),
		b2Vec2(0.0f, -g_GRAVITY),
		6,
		2
	);

	return true;
}

bool Game::BuildSystemContainer()
{
	//Managers
	m_SystemPointers.m_Game = this;
	m_SystemPointers.m_TexMgr = m_TexResourceMgr.get();
	m_SystemPointers.m_KBMMgr = m_KBMMgr.get();
	m_SystemPointers.m_Box2DMgr = m_Box2DPhysics.get();
	m_SystemPointers.m_GPMgr = m_GPMgr.get();
	m_SystemPointers.m_AudioMgr = m_AudioMgr.get();
	m_SystemPointers.m_ModeMgr = m_ModesMgr.get();
	m_SystemPointers.m_GraphicsMgr = m_GraphicsMgr.get();
	m_SystemPointers.m_UIMgr = m_UIMgr.get();

	//Resources
	m_SystemPointers.m_GameTime = &GetGameTime();
	m_SystemPointers.m_Blackboard = m_Blackboard.get();

	//Functionality

	return true;
}

bool Game::SetupInitialBlackboardData()
{
	//Set initial window size
	m_Blackboard->m_NativeWinX = m_WinData.m_NativeWidth;
	m_Blackboard->m_NativeWinY = m_WinData.m_NativeHeight;

	return true;
}

void Game::PreRender_Default()
{
	/////////////////
	/// Pre-Frame ///
	/////////////////

	//Get the allocator for the current frame
	auto cmdListAlloc = m_CurrentFrameResource->m_CommandListAlloc;

	//Reuse the memory associated with command recording
	//We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());
	//A CL can be reset after it has been added to the command queue via ExecutCommandList
	//Reusing the CL reuses memory
	ThrowIfFailed(m_CommandList->Reset(cmdListAlloc.Get(), nullptr));

	//Indicate a state transition on the resource usage
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_SwapChainBuffer[m_CurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET)
	);

	//Clear views
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = (CD3DX12_CPU_DESCRIPTOR_HANDLE)GetCurrentBackBufferView();
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = (CD3DX12_CPU_DESCRIPTOR_HANDLE)GetDSVHeapStart();

	//Clear the back and depth buffers
	//Specify the buffers being rendered to
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::Black, 0, nullptr);
	m_CommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//Set viewport and scissor rects
	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	//Get resource heaps and build heaps container to bind to pipeline
	/*
		Bind heaps to the pipeline.
		REMINDER: Only (3) different configurations work at anytime:
		1. (1) SRV/CBV/UAV Heap
		2. (1) Sampler Heap
		3. (1) SRV/CBV/UAV Heap OR (1) Sampler Heap <<-- Typical setup.
	*/
	ID3D12DescriptorHeap* heaps[] =
	{
		m_TexResourceMgr->GetHeapAtIndex((unsigned)BE_ManagerEnums::DescHeapIndexes::DEFAULT_SRV).m_ResourceDescriptors->Heap(),	// SRV
		m_GraphicsMgr->GetCommonStates()->Heap()																					// Sampler
	};
	m_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);
	ID3D12RootSignature* rootSig = m_GraphicsMgr->GetRootSignature((unsigned)BE_ManagerEnums::RootSigIndexes::DEFAULT);
	m_CommandList->SetGraphicsRootSignature(rootSig);
}

void Game::PostRender_Default()
{
	//////////////////
	/// Post Frame ///
	//////////////////

	//Indicate a state transition on the resource usage
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_SwapChainBuffer[m_CurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));

	//Done recording commands
	ThrowIfFailed(m_CommandList->Close());
	//Add CL to the queue for execution
	ID3D12CommandList* cmdsList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsList), cmdsList);

	//Swap the back and front buffers
	ThrowIfFailed(m_SwapChain->Present(0, 0));

	//Swap current buffer target
	m_CurrentBackBuffer = (m_CurrentBackBuffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

	//Advance the current fence value to mark the commands upto this point
	m_CurrentFrameResource->m_Fence = ++m_CurrentFence;

	/*
		Add an instruction to the CQ to set a new fence point.
		Because we are using the GPU's timeline, the new fence point won't be set until
		the GPU finishes processing all the commands prior to this Signal().
	*/
	m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence);

	//Commit Spritebatch inputs
	m_GraphicsMgr->GetGraphicsMemory()->Commit(m_CommandQueue.Get());
}


//================================================================================\\
// Custom Behaviour + Data
//================================================================================\\


void Game::Update_FrameResources()
{
	//Update buffer
	m_FrameBuffer.m_DeltaTime = m_Timer.DeltaTime();
	m_FrameBuffer.m_GameTime = m_Timer.TotalTime();

	//Upload to buffer
	m_CurrentFrameResource->m_FrameBuffer->CopyData(0, m_FrameBuffer);
}


