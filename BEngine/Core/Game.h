//*********************************************************************************\\
// 
// Primary game class, serving as highest level hub for game behaviourand features.
// Uses D3D12_App as parent for window creation and basic DX12 startup.
// 
// Uses singleton pattern.
// 
//*********************************************************************************\\

#pragma once

#include "D3D12_App.h"

//Library Includes
#include <functional>						//Func ptrs

//Engine Includes
#include "Types/BE_SharedTypes.h"			//Frame Resources type
#include "Types/SpriteBatch_Wrapper.h"

//Project Includes
#include "GameConfigs.h"					//Config data and enums


//
//Foward Declarations
//

//Main actor class
class Actor2D_Interface;

//Core Managers, Resources and Functionality
class Game;
class Mgr_TextureResources;
class Mgr_InputGamepad;
class Mgr_InputKBM;
class Mgr_Box2DPhysics;
class Mgr_Audio;
class Mgr_Modes;
class Mgr_Graphics;
class Mgr_UI;
struct GameBlackboard;

//Shipping container for passing all important managers/resources in one go
struct System
{
	//
	//Core Managers, Resources and Functionality
	//

	/*
		Fixed/Limited configurability. Not strictly open to change during projects
		outside of using innate behaviours.
	*/
	Mgr_TextureResources*   m_TexMgr = nullptr;
	Mgr_InputGamepad*		m_GPMgr = nullptr;
	Mgr_InputKBM*			m_KBMMgr = nullptr;
	Mgr_Audio*				m_AudioMgr = nullptr;
	Mgr_Modes*				m_ModeMgr = nullptr;
	Mgr_Box2DPhysics*		m_Box2DMgr = nullptr;
	Mgr_Graphics*			m_GraphicsMgr = nullptr;
	Mgr_UI*					m_UIMgr = nullptr;
	GameTimer*				m_GameTime = nullptr;

	/*
		More open to configuration/custom behaviours relative to the project.
	*/

	Game*				    m_Game = nullptr;
	GameBlackboard*		    m_Blackboard = nullptr;


	//
	//Custom M/R/F (Project Specific)
	//

};	

class Game : public D3D12_App
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	//Windows Creation (Calls D3D12Application)
	Game(HINSTANCE hInstance);
	Game(const Game& rSide) = delete;
	Game& operator=(const Game& rSide) = delete;
	~Game();

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Setup
	//

	//Main Init Process (Called ONCE in main.cpp)
	bool InitialiseGame();

	//
	//Utils
	//

	//Resets unique game object counter (Only use if complete clearing game objects and re-assigning IDs)
	void ClearUniqueGameObjectIDCounter() { m_GameObjectRegisterCount = 0; }

	//Resets the rendering pre/post functions to system default rendering.
	//Like any changes done to these functions, must be done before rendering, not during it.
	void ResetRenderingFunctions();

	//Updates the current window dims and invokes currently bound resize function
	bool ChangeWindowSize_16_9(AspectRatios_16by9 res);


	//
	//General
	//

	//Invokes currently set Pre-Render function (call only ONCE per render, and only BEFORE PostRender)
	void InvokePreRenderFunc() { std::invoke(m_PreRenderFunc); }
	//Invokes currently set Post-Render function (call only ONCE per render, and only AFTER PreRender)
	void InvokePostRenderFunc() { std::invoke(m_PostRenderFunc); }
	//Sets new invoking functions via id (define via enum). New functions must defined internally to the game class
	void SetNewRenderFunctions(BE_ManagerEnums::RenderConfigDescriptions renderID);

	/////////////////
	/// Accessors ///
	/////////////////

	static Game* GetGame() { return m_Game; }
	System& GetSystems() { return m_SystemPointers; }
	FrameResource* GetCurrentFrameResource() { return m_CurrentFrameResource; }


	//Returns unique game ID (increments on the way out). This ID should be used as an arbitrary ID, 
	//and not relied upon for fixed, known numbers.
	/*
		Returns a unique game ID (incrementing on the wayout), which serves as quick and cheap 
		indentifier for determining if objects are the same or not. Should not be relied on for
		fixed, known IDs.
	*/
	unsigned RegisterNewGameObject() { return m_GameObjectRegisterCount++; }

private:

	/////////////////
	/// Overrides ///
	/////////////////

	//Main Update Window
	void Update() override;
	//Main Render Window
	void Render() override;
	//Handles window messages and intercepts part of the message for input manager
	LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	//Additional closing control functions (called when WM_CLOSE and WM_DESTROY)
	void HandleWindowClosing() override;
	void HandleWindowDestroy() override;


	//////////////////
	/// Operations ///
	//////////////////

	//
	//Main Setup
	//These are customisable as required by the project (baseline behaviour gets the game running in a stable state)

	//Manager allocations are done here
	bool ResourceAllocations();
	//Frame Resource Creation
	bool BuildFrameResources();
	//Preliminary manager and resource setup (for core managers specifically)
	bool PreliminaryResourceSetup();
	//Create initial SRV heaps
	bool BuildInitialHeaps();
	//Create all root signatures for game
	bool BuildRootSignatures();
	//Build all initial spritebatches
	bool BuildInitialSpritebatches(DirectX::ResourceUploadBatch& resourceUpload);
	//Build all initial spritefonts
	bool BuildInitialSpritefonts(unsigned entryIndex, DirectX::ResourceUploadBatch& resourceUpload);
	//Load initial textures required (could be everything, or just enough for starting state etc)
	bool LoadInitialTextures(DirectX::ResourceUploadBatch& resourceUpload);

	//
	//Post Setup
	//

	//Allocate and build modes here (and what mode to set as starting mode)
	bool BuildModes(size_t startingMode);
	//Build and initialise any game specific manager classes here
	bool BuildGameManagers();
	//Puts together system container
	bool BuildSystemContainer();
	//Setup any initial blackboard container values
	bool SetupInitialBlackboardData();

	//
	//Other
	//

	//Default pre-frame render behaviour
	void PreRender_Default();
	//Default post-frame render behaviour
	void PostRender_Default();

	////////////
	/// Data ///
	////////////

	//
	//Pointers
	//

	//Static access pointer (for global access if required)
	static Game* m_Game;

	//Pre and post frame setup may require different configurations (based on shader/post processing effect requirements).
	//Use these to set appropriately paired functions for before and after frame draws.
	std::function<void()> m_PreRenderFunc;
	std::function<void()> m_PostRenderFunc;
	//These functions may also require additional behaviour regarding a resize event, so provide an option to set this as well
	std::function<void()> m_OnResizeFunc;

	//
	//System Data
	//

	//Data shipping container
	System m_SystemPointers;

	//Divides frames into X slices, and manages the individual CPU resources for each frame (set to 2 by default)
	std::vector<std::unique_ptr<FrameResource>> m_FrameResources;
	FrameResource* m_CurrentFrameResource = nullptr;
	int m_CurrentFrameResourceIndex = 0;

	//Pre and post frame behaviour can be defered to the active mode (custom behaviour may be required)
	bool m_DeferFrameBehaviour = false; //NYI

	//Game objects may not need more than a number-to-number check, or a globally unique identifier to distinguish
	//themselves from each other. This number both tracks the number of numbered game objects that have called the 
	//support function: GetUniqueGameObjectID(), which also gives each game object a unique sequential number.
	//This can be reset, but MAY incur errors if game objects are granted the same ID
	unsigned m_GameObjectRegisterCount = 0;

	//
	//Managers
	//

	//Core Engines (Texture, Audio, Input, Modes)
	//Gameplay (UI, Entities, Specific GP managers)

	//Manages loaded texture resources, associated data, and resource heaps
	std::unique_ptr<Mgr_TextureResources> m_TexResourceMgr;
	//Manages keyboard and mouse inputs by intercepting message pump messages and processing them
	std::unique_ptr<Mgr_InputKBM> m_KBMMgr;
	//Manages gamepad based inputs
	std::unique_ptr<Mgr_InputGamepad> m_GPMgr;
	//Manages Box2D Physics simulations and operations
	std::unique_ptr<Mgr_Box2DPhysics> m_Box2DPhysics;
	//Manage game audio submissions and volumes
	std::unique_ptr<Mgr_Audio> m_AudioMgr;
	//Manages game mode storage and active mode update/render cycles
	std::unique_ptr<Mgr_Modes> m_ModesMgr;
	//Manages spritebatches, fonts, and other graphical elements of the game
	std::unique_ptr<Mgr_Graphics> m_GraphicsMgr;
	//Manage UI detection methods
	std::unique_ptr<Mgr_UI> m_UIMgr;

	//
	//Resources
	//

	//Hosts commonly used data in one place (updated/accessed by clients as required)
	std::unique_ptr<GameBlackboard> m_Blackboard;

	//
	//Functionality
	//


//================================================================================\\
// Custom Behaviour + Data
//================================================================================\\

public:

private:

	//////////////////
	/// Operations ///
	//////////////////
	
	//
	//Updates
	//

	//Updates FR buffers (called at end of update window)
	void Update_FrameResources();

	//
	//Rendering
	//

	////////////
	/// Data ///
	////////////
	
	//Per-Frame Const Buffer (Used in Glow PSO)
	CBuffer_PerFrame m_FrameBuffer;
};