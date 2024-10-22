#include "Scene_Demo.h"

//Utils
#include "Utils/MathHelper.h"
#include <thread>
#include <synchapi.h>

//Engine Includes
#include "Includes/BE_All_Managers.h"
#include "Texture_Enums/Pixel_Sidescroller_City_Enums.h"


using namespace BE_TextureEnums;

#define CLOUD_SPD_MIN 35.f
#define CLOUD_SPD_MAX 65.f
#define CLOUD_X_MIN 0.00f
#define CLOUD_X_MAX 0.70f
#define CLOUD_Y_MIN 0.05f
#define CLOUD_Y_MAX 0.45f

#define TRAIN_SPD_MIN 375.f
#define TRAIN_SPD_MAX 750.f

#define BG_DEPTH 0.10f
#define BG_SIL_DEPTH 0.12f
#define BG_STARS_DEPTH 0.11f
#define TRAIN_TRACK_DEPTH 0.90f
#define TREE_DEPTH 0.93f
#define BUILDING_DEPTH_MIN 0.8f
#define BUILDING_DEPTH_MAX 0.92f
#define TRAIN_DEPTH 0.89f
#define CLOUD_DEPTH 0.6f

//
//Demo Entities
//

void DemoEnt_Scroller::Update_Main(System& sys)
{
	//Update position (for now, just update the sprite)
	SpriteData& data = static_cast<Module_Sprite*>(m_Modules[0].get())->GetSpriteData();
	data.m_Position.x += m_ScrollSpeed.x * sys.m_GameTime->DeltaTime();

	bool update = false;

	//Use speed as a direction and determine if has hit boundary point
	if ((m_ScrollSpeed.x < 0.f && data.m_Position.x < m_Limits.x))
		update = true;
	else if (m_ScrollSpeed.x > 0.f && data.m_Position.x > m_Limits.x)
		update = true;

	if (update)
	{
		switch (m_ObjectID)
		{
		case ObjectID::CLOUD:
			//Update position
			data.m_Position = { m_Limits.y, sys.m_Game->GetWindowData().m_NativeHeight * MathHelper::RandF(CLOUD_Y_MIN, CLOUD_Y_MAX) };
			//Update new scrolling speed
			m_ScrollSpeed.x = MathHelper::RandF(CLOUD_SPD_MIN, CLOUD_SPD_MAX);

			//Set frame based on current scene
			switch (m_SceneID)
			{
			case SceneID::DAYTIME:
				data.SetFrame(MathHelper::Rand((int)PSC_Frames::DAYTIME_CLOUD_01, (int)PSC_Frames::DAYTIME_CLOUD_08));
				break;

			case SceneID::NIGHTTIME:
				data.SetFrame(MathHelper::Rand((int)PSC_Frames::NIGHT_CLOUD_01, (int)PSC_Frames::NIGHT_CLOUD_08));
				break;
			}

			break;

		case ObjectID::TRAIN:
			//Update position
			data.m_Position.x = m_Limits.y;
			
			//Update new scrolling speed
			if(m_ScrollSpeed.x > 0.f)
				m_ScrollSpeed.x = MathHelper::RandF(TRAIN_SPD_MIN, TRAIN_SPD_MAX);
			else
				m_ScrollSpeed.x = -MathHelper::RandF(TRAIN_SPD_MIN, TRAIN_SPD_MAX);

			break;
		}
	}
}

void DemoEnt_Scroller::Update_PreRender(System& sys)
{
	sys.m_GraphicsMgr->SubmitToRenderGroup((unsigned)BE_ManagerEnums::SpritebatchIndexes::MAIN_SCENE, this);
}

void DemoEnt_Scroller::Render(System& sys, DirectX::SpriteBatch* batch)
{
	m_Modules[0]->Render(sys, batch);
}

void DemoEnt_Scroller::SoftReset(System& sys)
{
	//Reset sprite position
	static_cast<Module_Sprite*>(m_Modules[0].get())->GetSpriteData().m_Position = { -99999.f, -99999.f };
	m_ScrollSpeed = { 0.f, 0.f };
}

void DemoEnt_Static::Update_PreRender(System& sys)
{
	sys.m_GraphicsMgr->SubmitToRenderGroup((unsigned)BE_ManagerEnums::SpritebatchIndexes::MAIN_SCENE, this);
}

void DemoEnt_Static::Render(System& sys, DirectX::SpriteBatch* batch)
{
	m_Modules[0]->Render(sys, batch);
}

void DemoEnt_Static::SoftReset(System& sys)
{
	//Reset sprite position
	static_cast<Module_Sprite*>(m_Modules[0].get())->GetSpriteData().m_Position = { -99999.f, -99999.f };
}



//
//Demo Scene
//



bool Scene_Demo::RunOnceInit(System& sys, SceneID id)
{
	//Find core texture for this demo
	SpriteTexture* tex = sys.m_TexMgr->FindTextureData(std::string("Pixel_Sidescroller_City"));

	//If found, go to setup
	if (tex)
	{
		//Initialise both container's actors

		for (auto& a : m_Statics)
		{
			//Get main sprite module
			Module_Sprite* spr = static_cast<Module_Sprite*>(a.GetModule(std::string("Main_Sprite")));
			//Set texture and then some default values to get it up and running
			spr->GetSpriteData().SetTexture(tex);
			spr->GetSpriteData().m_Position = { -99999.f, -99999.f };
			spr->GetSpriteData().SetFrame((unsigned)PSC_Frames::DAYTIME_BACKGROUND);
		}

		for (auto& a : m_Scrollers)
		{
			//Get main sprite module
			Module_Sprite* spr = static_cast<Module_Sprite*>(a.GetModule(std::string("Main_Sprite")));
			//Set texture and then some default values to get it up and running
			spr->GetSpriteData().SetTexture(tex);
			spr->GetSpriteData().m_Position = { -99999.f, -99999.f };
			spr->GetSpriteData().SetFrame((unsigned)PSC_Frames::DAYTIME_BACKGROUND);
		}

		//Setup the scene based on ID given
		switch (id)
		{
		case SceneID::DAYTIME:
			m_SceneID = id;
			Setup_Daytime(sys);
			break;

		case SceneID::NIGHT:
			m_SceneID = id;
			Setup_Night(sys);
			break;
		}

		return true;
	}
	else
	{
		msg_assert(false, "RunOnceInit(): Failed to find core texture, init incomplete!");
		return false;
	}

	assert(false);
	return false;
}

bool Scene_Demo::Update_Main(System& sys)
{
	//Call main update on each entity
	for (auto& a : m_Statics)
		a.Update_Main(sys);
	for (auto& a : m_Scrollers)
		a.Update_Main(sys);

	return true;
}

void Scene_Demo::Update_PreRender(System& sys)
{
	//Each entity uses render groups so allow them to submit themselves
	for (auto& a : m_Statics)
		a.Update_PreRender(sys);
	for (auto& a : m_Scrollers)
		a.Update_PreRender(sys);
}

void Scene_Demo::SwitchScene(System& sys, SceneID id)
{
	if (id == SceneID::COUNT)
	{
		assert(false);
		return;
	}

	//Reset the scene
	ResetScene();

	//Some pre-amble setup
	m_SceneID = id;

	//Using ID, set new scene name and call setup function
	switch (id)
	{
	case SceneID::DAYTIME:

		m_SceneName = "Daytime";
		Setup_Daytime(sys);
		break;

	case SceneID::NIGHT:
		m_SceneID = id;
		m_SceneName = "Nighttime";
		Setup_Night(sys);
		break;
	}
}

void Scene_Demo::SwitchScene(System& sys, int code)
{
	//Capture the current scene ID twice (current and old)
	int sceneID = (int)m_SceneID;
	int oldID = sceneID;

	//Based on inbound code, manipulate the scene ID to determine what scene to shift to later
	switch ((SceneSwitchCode)code)
	{
	case SceneSwitchCode::SCENE_LEFT:
		//Clamp scene ID to acceptable range if OOR
		if (--sceneID < 0)
			sceneID = (int)SceneID::COUNT - 1;
		break;
		
	case SceneSwitchCode::SCENE_RIGHT:
		//Clamp scene ID to acceptable range if OOR
		if (++sceneID >= (int)SceneID::COUNT)
			sceneID = 0;
		break;
	
	default:
		assert(false);
	}

	//If the scene changed (which is typically should in this setup, but better to be safe), then go to main call with new sceneID
	if (sceneID != oldID)
		SwitchScene(sys, (SceneID)sceneID);
}

void Scene_Demo::Bindable_SwitchScene(System* sys, int* sceneCode)
{
	//As we are just indirectly, pass these through to the actual function (shifting pointer to reference/copy)
	SwitchScene(*sys, *sceneCode);
}

void Scene_Demo::Bindable_GetSceneName(std::string* inboundStr)
{
	//Copy current scene name into inbound string
	*inboundStr = m_SceneName;
}

void Scene_Demo::ResetScene()
{
	System& sys = Game::GetGame()->GetSystems();

	//Just call innate resets (sets position to way offscreen and disables speed)
	for (auto& a : m_Statics)
		a.SoftReset(sys);
	for (auto& a : m_Scrollers)
	{
		//Some edge cases to check for if this isn't the first time around)
		if (m_InitSetupDone)
		{
			switch (a.m_ObjectID)
			{
			case DemoEnt_Scroller::ObjectID::TRAIN:
				//Do nothing (yet), but this preserves its position and speed, the scene setup will change its frame appropriately
				break;

			default:
				a.SoftReset(sys);
			}
		}
		else
		{
			a.SoftReset(sys);
		}		
	}
	
}

bool Scene_Demo::Setup_Abandoned(System& sys)
{
	return false;
}

bool Scene_Demo::Setup_Daytime(System& sys)
{
	//
	//Initial Setup
	//

	//Window dims for alignment
	float winX = (float)sys.m_Blackboard->m_NativeWinX;
	float winY = (float)sys.m_Blackboard->m_NativeWinY;

	//Scene adjustment values like scaling and offsets
	float scaleX = 1.f;
	float scaleY = 1.f;
	//Sub-scaler for some internal elements (scaling is pegged against background being scaled up to current window size)
	float subScaling = 0.8f;	
	float trainOffset = 0.f;

	//
	//Static Entities
	//

	{
		//Track current accessed index
		unsigned index = 0;

		//Sprite holding pointer (updated as index increments)
		Module_Sprite* spr = static_cast<Module_Sprite*>(m_Statics[index].GetModule(0));

		//
		//Background Layers
		//

		//Main background
		spr->GetSpriteData().SetFrame((int)PSC_Frames::DAYTIME_BACKGROUND);
		spr->GetSpriteData().m_Position = { winX * 0.5f, winY * 0.5f };
		spr->GetSpriteData().m_LayerDepth = BG_DEPTH;
		//Calc scale adjustment so its syncs with window size (most other sprites are scaled around this ratio)
		scaleX = winX / spr->GetSpriteData().GetFrameSizeX();
		scaleY = winY / spr->GetSpriteData().GetFrameSizeY();
		spr->GetSpriteData().m_Scale = { scaleX, scaleY };

		//Increment index + get new actors module
		spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));
		
		//Background Silhouettes
		for (int i((int)PSC_Frames::DAYTIME_BUILDING_SILHOUETTE_BACK); i < (int)PSC_Frames::DAYTIME_BUILDING_SILHOUETTE_FRONT + 1; ++i)
		{
			spr->GetSpriteData().SetFrame(i);
			spr->GetSpriteData().m_Position = { winX * 0.5f, winY * 1.0f };
			spr->GetSpriteData().m_LayerDepth = BG_SIL_DEPTH;
			spr->GetSpriteData().m_Scale = { scaleX, scaleY };

			
			spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));
		}

		//
		//Front Train Tracks + Trees
		//

		spr->GetSpriteData().SetFrame((int)PSC_Frames::DAYTIME_TRAIN_TRACKS);
		spr->GetSpriteData().m_Position = { winX * 0.5f, winY * 1.0f };
		spr->GetSpriteData().m_LayerDepth = TRAIN_TRACK_DEPTH;
		spr->GetSpriteData().m_Scale = { scaleX, scaleY };
		//Set train offset here (so that the train(s) align with the tracks properly)
		trainOffset = spr->GetSpriteData().GetFrameSizeY_Scaled();
		spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));


		spr->GetSpriteData().SetFrame((int)PSC_Frames::DAYTIME_TREES);
		spr->GetSpriteData().m_Position = { winX * 0.5f, winY * 1.0f };
		spr->GetSpriteData().m_LayerDepth = TREE_DEPTH;
		spr->GetSpriteData().m_Scale = { scaleX, scaleY };
		spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));

		//
		//Buildings
		//

		//Used for positional adjustments
		unsigned posMod = 0;

		//Window dims for alignment
		float winX = (float)sys.m_Blackboard->m_NativeWinX;
		float winY = (float)sys.m_Blackboard->m_NativeWinY;

		//Cycle through and setup buildings
		for (unsigned i((unsigned)PSC_Frames::DAYTIME_BUILDING_02); i < (unsigned)PSC_Frames::DAYTIME_BUILDING_08 + 1; ++i)
		{
			//Set frame, scale and depth values
			spr->GetSpriteData().SetFrame(i);
			spr->GetSpriteData().m_Scale = { scaleX * subScaling, scaleY * subScaling };
			//Randomise the depth somewhat so that some of the buildings show head of the tracks and trains
			spr->GetSpriteData().m_LayerDepth = (i % MathHelper::Rand(3, 4) ? BUILDING_DEPTH_MIN : BUILDING_DEPTH_MAX);

			//Generate a sub-range that the buildings can slot into randomly (moving along as the loop moves on)
			float rMin = (winX * 0.05f) + ((winX * 0.14f) * posMod++);
			float rMax = rMin + (winX * 0.14f);

			/*
				RandF (and its underlying use of other classes) doesn't provide a variation outcome when using predictable inputs
			*/
			float randX = MathHelper::RandF(rMin, rMax);;
			spr->GetSpriteData().m_Position = { randX, winY };

			spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));
		}
	}


	//
	//Scrolling Entities
	//

	{
		//Track current accessed index
		unsigned index = 0;

		//Set scroller limits
		float leftLimit = static_cast<float>(-winX);
		float rightLimit = static_cast<float>(winX * 2.f);

		//Sprite holding pointer (updated as index increments)
		Module_Sprite* spr = static_cast<Module_Sprite*>(m_Scrollers[index].GetModule(0));
	
		//
		//Trains
		//
		
		for (unsigned i(0); i < 2; ++i)
		{
			//Setup differently if initial setup already done
			if (m_InitSetupDone)
			{
				//Switch frame
				spr->GetSpriteData().SetFrame((int)PSC_Frames::DAYTIME_TRAIN);
			}
			else
			{

				//Odd/even split setting up some train settings
				if (i % 2 == 0)
				{
					m_Scrollers[index].m_ScrollSpeed = { -MathHelper::RandF(TRAIN_SPD_MIN, TRAIN_SPD_MAX), 0.f };
					m_Scrollers[index].m_Limits = { leftLimit, rightLimit };
					spr->GetSpriteData().m_Position = { winX * 1.5f, winY - trainOffset + 5.f };
				}
				else
				{
					m_Scrollers[index].m_ScrollSpeed = { MathHelper::RandF(TRAIN_SPD_MIN, TRAIN_SPD_MAX), 0.f };
					m_Scrollers[index].m_Limits = { rightLimit, leftLimit };
					spr->GetSpriteData().m_Position = { -winX, winY - trainOffset + 5.f };
				}

				//Shared setup
				m_Scrollers[index].m_ObjectID = DemoEnt_Scroller::ObjectID::TRAIN;
				m_Scrollers[index].m_SceneID = DemoEnt_Scroller::SceneID::DAYTIME;
				spr->GetSpriteData().SetFrame((int)PSC_Frames::DAYTIME_TRAIN);
				spr->GetSpriteData().m_LayerDepth = TRAIN_DEPTH;
				spr->GetSpriteData().m_Scale = { scaleX * subScaling, scaleY * subScaling };				
			}

			//Update sprite pointer
			spr = static_cast<Module_Sprite*>(m_Scrollers[++index].GetModule(0));
		}

		//
		//Clouds
		//

		for (unsigned i((unsigned)PSC_Frames::DAYTIME_CLOUD_01); i < (unsigned)PSC_Frames::DAYTIME_CLOUD_08 + 1; ++i)
		{
			//Set scroller custom data
			m_Scrollers[index].m_ScrollSpeed = { MathHelper::RandF(CLOUD_SPD_MIN, CLOUD_SPD_MAX), 0.f };
			m_Scrollers[index].m_Limits = { rightLimit * 0.6f, leftLimit * 0.15f };
			m_Scrollers[index].m_ObjectID = DemoEnt_Scroller::ObjectID::CLOUD;
			m_Scrollers[index].m_SceneID = DemoEnt_Scroller::SceneID::DAYTIME;

			spr->GetSpriteData().SetFrame(i);
			spr->GetSpriteData().m_Position = { winX * MathHelper::RandF(CLOUD_X_MIN, CLOUD_X_MAX), winY * MathHelper::RandF(CLOUD_Y_MIN, CLOUD_Y_MAX) };
			spr->GetSpriteData().m_LayerDepth = CLOUD_DEPTH;
			spr->GetSpriteData().m_Scale = { scaleX * subScaling, scaleY * subScaling };

			//Update sprite pointer
			spr = static_cast<Module_Sprite*>(m_Scrollers[++index].GetModule(0));
		}
	}


	m_InitSetupDone = true;
	return true;
}

bool Scene_Demo::Setup_Night(System& sys)
{
	//
	//Initial Setup
	//

	//Window dims for alignment
	float winX = (float)sys.m_Blackboard->m_NativeWinX;
	float winY = (float)sys.m_Blackboard->m_NativeWinY;

	//Scene adjustment values like scaling and offsets
	float scaleX = 1.f;
	float scaleY = 1.f;
	//Sub-scaler for some internal elements (scaling is pegged against background being scaled up to current window size)
	float subScaling = 0.8f;
	float trainOffset = 0.f;

	//
	//Static Entities
	//

	{
		//Track current accessed index
		unsigned index = 0;

		//Sprite holding pointer (updated as index increments)
		Module_Sprite* spr = static_cast<Module_Sprite*>(m_Statics[index].GetModule(0));

		//
		//Background Layers
		//

		//Main background
		spr->GetSpriteData().SetFrame((int)PSC_Frames::NIGHT_BACKGROUND);
		spr->GetSpriteData().m_Position = { winX * 0.5f, winY * 0.5f };
		spr->GetSpriteData().m_LayerDepth = BG_DEPTH;
		//Calc scale adjustment so its syncs with window size (most other sprites are scaled around this ratio)
		scaleX = winX / spr->GetSpriteData().GetFrameSizeX();
		scaleY = winY / spr->GetSpriteData().GetFrameSizeY();
		spr->GetSpriteData().m_Scale = { scaleX, scaleY };
		//Increment index + get new actors module
		spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));

		for (unsigned i((unsigned)PSC_Frames::NIGHT_STARS_01); i < (unsigned)PSC_Frames::NIGHT_STARS_02 + 1; ++i)
		{
			spr->GetSpriteData().SetFrame(i);
			spr->GetSpriteData().m_Position = { winX * 0.5f, winY * 0.3f };
			spr->GetSpriteData().m_LayerDepth = BG_STARS_DEPTH;
			spr->GetSpriteData().m_Scale = { scaleX, scaleY };

			spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));
		}

		//Background Silhouettes
		for (unsigned i((unsigned)PSC_Frames::NIGHT_BUILDING_SILHOUETTE_BACK); i < (unsigned)PSC_Frames::NIGHT_BUILDING_SILHOUETTE_FRONT + 1; ++i)
		{
			spr->GetSpriteData().SetFrame(i);
			spr->GetSpriteData().m_Position = { winX * 0.5f, winY * 1.0f };
			spr->GetSpriteData().m_LayerDepth = BG_SIL_DEPTH;
			spr->GetSpriteData().m_Scale = { scaleX, scaleY };

			spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));
		}

		//
		//Front Train Tracks + Trees
		//

		spr->GetSpriteData().SetFrame((int)PSC_Frames::NIGHT_TRAIN_TRACKS);
		spr->GetSpriteData().m_Position = { winX * 0.5f, winY * 1.0f };
		spr->GetSpriteData().m_LayerDepth = TRAIN_TRACK_DEPTH;
		spr->GetSpriteData().m_Scale = { scaleX, scaleY };
		//Set train offset here (so that the train(s) align with the tracks properly)
		trainOffset = spr->GetSpriteData().GetFrameSizeY_Scaled();
		spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));


		spr->GetSpriteData().SetFrame((int)PSC_Frames::NIGHT_TREES);
		spr->GetSpriteData().m_Position = { winX * 0.5f, winY * 1.0f };
		spr->GetSpriteData().m_LayerDepth = TREE_DEPTH;
		spr->GetSpriteData().m_Scale = { scaleX, scaleY };
		spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));

		//
		//Buildings
		//

		//Used for positional adjustments
		unsigned posMod = 0;

		//Cycle through and setup buildings
		for (unsigned i((unsigned)PSC_Frames::NIGHT_BUILDING_01); i < (unsigned)PSC_Frames::NIGHT_BUILDING_08 + 1; ++i)
		{
			//Set frame, scale and depth values
			spr->GetSpriteData().SetFrame(i);
			spr->GetSpriteData().m_Scale = { scaleX * subScaling, scaleY * subScaling };
			//Randomise the depth somewhat so that some of the buildings show head of the tracks and trains
			spr->GetSpriteData().m_LayerDepth = (i % MathHelper::Rand(3, 4) ? BUILDING_DEPTH_MIN : BUILDING_DEPTH_MAX);

			//Generate a sub-range that the buildings can slot into randomly (moving along as the loop moves on)
			float rMin = (winX * 0.05f) + ((winX * 0.14f) * posMod++);
			float rMax = rMin + (winX * 0.14f);
			spr->GetSpriteData().m_Position = { MathHelper::RandF(rMin, rMax), winY };

			spr = static_cast<Module_Sprite*>(m_Statics[++index].GetModule(0));
		}
	}


	//
	//Scrolling Entities
	//

	{
		//Track current accessed index
		unsigned index = 0;

		//Set scroller limits
		float leftLimit = static_cast<float>(-winX);
		float rightLimit = static_cast<float>(winX * 2.f);

		//Sprite holding pointer (updated as index increments)
		Module_Sprite* spr = static_cast<Module_Sprite*>(m_Scrollers[index].GetModule(0));;

		//
		//Trains
		//

		for (unsigned i(0); i < 2; ++i)
		{

			//Setup differently if initial setup already done
			if (m_InitSetupDone)
			{
				//Switch frame
				spr->GetSpriteData().SetFrame((int)PSC_Frames::NIGHT_TRAIN);
			}
			else
			{

				//Odd/even split setting up some train settings
				if (i % 2 == 0)
				{
					m_Scrollers[index].m_ScrollSpeed = { -MathHelper::RandF(TRAIN_SPD_MIN, TRAIN_SPD_MAX), 0.f };
					m_Scrollers[index].m_Limits = { leftLimit, rightLimit };
					spr->GetSpriteData().m_Position = { winX * 1.5f, winY - trainOffset + 5.f };
				}
				else
				{
					m_Scrollers[index].m_ScrollSpeed = { MathHelper::RandF(TRAIN_SPD_MIN, TRAIN_SPD_MAX), 0.f };
					m_Scrollers[index].m_Limits = { rightLimit, leftLimit };
					spr->GetSpriteData().m_Position = { -winX, winY - trainOffset + 5.f };
				}

				//Shared setup
				m_Scrollers[index].m_ObjectID = DemoEnt_Scroller::ObjectID::TRAIN;
				m_Scrollers[index].m_SceneID = DemoEnt_Scroller::SceneID::NIGHTTIME;
				spr->GetSpriteData().SetFrame((int)PSC_Frames::NIGHT_TRAIN);
				spr->GetSpriteData().m_LayerDepth = TRAIN_DEPTH;
				spr->GetSpriteData().m_Scale = { scaleX * subScaling, scaleY * subScaling };
			}

			//Update sprite pointer
			spr = static_cast<Module_Sprite*>(m_Scrollers[++index].GetModule(0));
		}

		//
		//Clouds
		//

		for (unsigned i((unsigned)PSC_Frames::NIGHT_CLOUD_01); i < (unsigned)PSC_Frames::NIGHT_CLOUD_08 + 1; ++i)
		{
			//Set scroller custom data
			m_Scrollers[index].m_ScrollSpeed = { MathHelper::RandF(CLOUD_SPD_MIN, CLOUD_SPD_MAX), 0.f };
			m_Scrollers[index].m_Limits = { rightLimit * 0.6f, leftLimit * 0.15f };
			m_Scrollers[index].m_ObjectID = DemoEnt_Scroller::ObjectID::CLOUD;
			m_Scrollers[index].m_SceneID = DemoEnt_Scroller::SceneID::NIGHTTIME;

			spr->GetSpriteData().SetFrame(i);
			spr->GetSpriteData().m_Position = { winX * MathHelper::RandF(CLOUD_X_MIN, CLOUD_X_MAX), winY * MathHelper::RandF(CLOUD_Y_MIN, CLOUD_Y_MAX) };
			spr->GetSpriteData().m_LayerDepth = CLOUD_DEPTH;
			spr->GetSpriteData().m_Scale = { scaleX * subScaling, scaleY * subScaling };

			spr = static_cast<Module_Sprite*>(m_Scrollers[++index].GetModule(0));
		}
	}

	m_InitSetupDone = true;
	return true;
}