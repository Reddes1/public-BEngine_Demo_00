#include "Mode_Demo.h"

//Library Includes
#include <algorithm>

//Utils
#include "Utils/MathHelper.h"
#include "Utils/Utils_General.h"

//Engine Includes
#include "Includes/BE_All_Managers.h"

RetCode Mode_Demo::Update_Main(System& sys)
{
	//Update UI Manager
	sys.m_UIMgr->Update_MouseColliders(sys);

	//Entities
	m_Player.Update_Main(sys);

	//Demo's
	m_SceneDemo.Update_Main(sys);
	m_PhysicsDemo.Update_Main(sys);

	for (auto& a : m_UIs)
		a.Update_Main(sys);


	return RetCode::OK;
}

RetCode Mode_Demo::Update_PrePhysics(System& sys)
{
	//Entities
	m_Player.Update_PrePhysics(sys);

	//Demo's
	m_PhysicsDemo.Update_PrePhysics(sys);

	return RetCode::OK;
}

RetCode Mode_Demo::Update_Physics(System& sys)
{
	//Run physics simulation for target world
	auto* world = sys.m_Box2DMgr->GetWorldData(std::string("Test_World_00"));
	if (world)
		world->StartWorldStep(sys.m_GameTime->DeltaTime());

	return RetCode::OK;
}

RetCode Mode_Demo::Update_PostPhysics(System& sys)
{
	//Entities
	m_Player.Update_PostPhysics(sys);

	//Demo's
	m_PhysicsDemo.Update_PostPhysics(sys);

	return RetCode::OK;
}

RetCode Mode_Demo::Update_PreRender(System& sys)
{
	//Entities
	m_Player.Update_PreRender(sys);

	//Demo's
	m_SceneDemo.Update_PreRender(sys);
	m_PhysicsDemo.Update_PreRender(sys);

	for (auto& a : m_UIs)
		a.Update_PreRender(sys);

	return RetCode::OK;
}

RetCode Mode_Demo::Render(System& sys)
{
	Game* game = Game::GetGame();

	//Render using groups (actors should already be submitted)
	sys.m_GraphicsMgr->DrawBatch(sys, (unsigned)BE_ManagerEnums::SpritebatchIndexes::MAIN_SCENE, game->GetCommandList().Get());
	sys.m_GraphicsMgr->DrawBatch(sys, (unsigned)BE_ManagerEnums::SpritebatchIndexes::MAIN_SCENE_TRANSFORMED, game->GetCommandList().Get());
	sys.m_GraphicsMgr->DrawBatch(sys, (unsigned)BE_ManagerEnums::SpritebatchIndexes::COLOUR_REPLACEMENT, game->GetCommandList().Get());

	//Use custom behaviour for this group
	sys.m_GraphicsMgr->GetSpritebatch((unsigned)BE_ManagerEnums::SpritebatchIndexes::CUSTOM_OUTLINE_GLOW)->m_CustomRenderFunc();

	//Rendering done, so clear groups
	sys.m_GraphicsMgr->ClearRenderGroups();

	return RetCode::OK;
}

RetCode Mode_Demo::ProcessKey(char key)
{


	return RetCode::OK;
}

RetCode Mode_Demo::EnterMode()
{
	if (m_Flags.m_DoRunOnceEnter)
	{
		//Get systems container
		System& sys = Game::GetGame()->GetSystems();

		//
		//Init Entities
		//

		m_Player.RunOnceInit(sys);

		//
		//Init UIs
		//

		//Positional data (current defunct till more menu's added)
		Vec2 anchor = { 185.f, 40.f };
		Vec2 offset = { 0.f, 65.f };
		Vec2 scale = { 0.66f, 0.66f };

		//Setup Scene Switcher Menu
		{
			//Get hold of the ui
			UI_DemoMenu& ui = m_UIs[(unsigned)MenuIndexes::SCENE_SWITCHER];

			//Run internal init
			ui.RunOnceInit(sys);

			/*
				Provide functionality to UI (uses bindable functions to execute behaviour).
				0 = Scene Switch via Scene_Demo,
				1 = Get Scene Name via Scene Demo
			*/
			ui.BindFuncToSlot(
				UI_DemoMenu::FuncSet_B_Indexes::CHANGE_BG_SCENE,
				BindToVoidFuncB<Scene_Demo, System, int>(&Scene_Demo::Bindable_SwitchScene, &m_SceneDemo)
			);

			ui.BindFuncToSlot(
				UI_DemoMenu::FuncSet_A_Indexes::GET_SCENE_NAME,
				BindToVoidFuncA<Scene_Demo, std::string>(&Scene_Demo::Bindable_GetSceneName, &m_SceneDemo)
			);

			//Only UI atm so just place at top of screen
			ui.MoveTo({ sys.m_Blackboard->m_NativeWinX * 0.5f, sys.m_Blackboard->m_NativeWinY * 0.06f });
			//High priority UI so give it a high depth level
			ui.SetDepth(DEPTH_STRATA_MAX);
		}

		//
		//Demo Classes
		//

		m_SceneDemo.RunOnceInit(sys, Scene_Demo::SceneID::DAYTIME);
		m_PhysicsDemo.SetWorld(&sys.m_Box2DMgr->GetWorldData(std::string("Test_World_00"))->m_World);
		m_PhysicsDemo.SetupDemo(sys, Box2DPhysics_Demo::ModeID::BALLPIT);

		//
		//Post
		//

		//Force update UI string(s)
		m_UIs[(unsigned)MenuIndexes::SCENE_SWITCHER].ForceSceneNameUpdate();

		//Run once done
		m_Flags.m_DoRunOnceEnter = false;
	}

	return RetCode::OK;
}


bool Mode_Demo::ExitMode()
{
	return false;
}

RetCode Mode_Demo::Release()
{
	return RetCode::OK;
}
