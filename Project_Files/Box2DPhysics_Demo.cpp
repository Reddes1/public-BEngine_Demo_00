#include "Box2DPhysics_Demo.h"

//Utilities
#include "Utils/MathHelper.h"
#include "Utils/Utils_General.h"

//Project Includes
#include "All_Managers.h"
#include "Texture_Enums/BE_2DTestingTextureEnums.h"

//Modules
#include "Modules/Module_Sprite.h"
#include "Modules/Module_Box2D_Body.h"

void Box2DPhysics_Demo::Release()
{

}

void Box2DPhysics_Demo::Setup_Ballpit(System& sys)
{
	/////////////////
	/// NEW SETUP ///
	/////////////////

	m_Props.clear();
	m_Props.reserve((unsigned)ConfigData::DEFAULT_RESERVE_COUNT);

	float winX = (float)sys.m_Blackboard->m_NativeWinX;
	float winY = (float)sys.m_Blackboard->m_NativeWinY;

	//
	//Boundaries setup
	//

	{
		//Add and run initial setup
		for (unsigned i(0); i < (unsigned)ConfigData::DEFAULT_BOUNDARIES_COUNT; ++i)
		{
			//Insert new prop
			m_Props.push_back(Entity_DemoProp());
			m_Props.back().RunOnceInit(sys);
		}

		{
			int count = 0;

			for (count; count < (unsigned)ConfigData::DEFAULT_BOUNDARIES_COUNT/2; ++count)
			{
				m_Props[count].InitProp(Entity_DemoProp::PropSetupID::FLOOR, m_World);
				Module_Sprite* spr = recast_static(Module_Sprite*, m_Props[count].GetModule(Module_Interface::ModuleTypeID::SPRITE));
				Module_Box2D_RigidBody2D* body = recast_static(Module_Box2D_RigidBody2D*, m_Props[count].GetModule(Module_Interface::ModuleTypeID::BOX2D_RIGIDBODY));

				body->GetBodyUserData()->m_CategoryID = 0;

				if (MathHelper::IsEven(count))
					spr->GetSpriteData().m_Position = { winX * 0.5f, 0.f };
				else
					spr->GetSpriteData().m_Position = { winX * 0.5f, winY };

				body->SyncWithSpriteData(spr->GetSpriteData());
			}

			for (count; count < (unsigned)ConfigData::DEFAULT_BOUNDARIES_COUNT; ++count)
			{
				m_Props[count].InitProp(Entity_DemoProp::PropSetupID::WALL, m_World);
				Module_Sprite* spr = recast_static(Module_Sprite*, m_Props[count].GetModule(Module_Interface::ModuleTypeID::SPRITE));
				Module_Box2D_RigidBody2D* body = recast_static(Module_Box2D_RigidBody2D*, m_Props[count].GetModule(Module_Interface::ModuleTypeID::BOX2D_RIGIDBODY));

				body->GetBodyUserData()->m_CategoryID = 0;

				if (MathHelper::IsEven(count))
					spr->GetSpriteData().m_Position = { 0.f, winY * 0.5f };
				else
					spr->GetSpriteData().m_Position = { winX, winY * 0.5f };

				body->SyncWithSpriteData(spr->GetSpriteData());
			}
		}
	}

	//
	//Prop setup
	//

	{
		//for (unsigned i(0); i < (unsigned)ConfigData::BALLPIT_DEFAULT_PROP_COUNT; ++i)
		//{
		//	//Add new prop
		//	m_Props.push_back(Entity_DemoProp());
		//	Entity_DemoProp& prop = m_Props.back();
		//	
		//	//Run initial setups
		//	prop.RunOnceInit(sys);
		//	int initType = MathHelper::Rand(3, 4);
		//	prop.InitProp((Entity_DemoProp::PropSetupID)initType, m_World);
		//
		//	Module_Sprite* spr = RECAST_STATIC(Module_Sprite*, prop.GetModule(Module_Interface::ModuleTypeID::SPRITE));
		//	Module_Box2D_RigidBody2D* body = RECAST_STATIC(Module_Box2D_RigidBody2D*, prop.GetModule(Module_Interface::ModuleTypeID::BOX2D_RIGIDBODY));

		//	//Random position the ball
		//	float xPos = MathHelper::RandF(winX * 0.3f, winX * 0.7f);
		//	float yPos = MathHelper::RandF(winY * 0.15f, winY * 0.3f);
		//	spr->GetSpriteData().m_Position = { xPos, yPos };

		//	body->SyncWithSpriteData(spr->GetSpriteData());
		//}
	}

	//This "Shouldn't" be required as the setup is different, but resync actor addresses with modules
	for (auto& a : m_Props)
		a.ReSyncModules();
}

void Box2DPhysics_Demo::Update_Main(System& sys)
{
	for (auto& a : m_Props)
		a.Update_Main(sys);
}

void Box2DPhysics_Demo::Update_PrePhysics(System& sys)
{
	for (auto& a : m_Props)
		a.Update_PrePhysics(sys);
}

void Box2DPhysics_Demo::Update_Physics(System& sys)
{

}

void Box2DPhysics_Demo::Update_PostPhysics(System& sys)
{
	for (auto& a : m_Props)
		a.Update_PostPhysics(sys);
}

void Box2DPhysics_Demo::Update_PreRender(System& sys)
{
	for (auto& a : m_Props)
		a.Update_PreRender(sys);
}

void Box2DPhysics_Demo::Render(System& sys)
{
	for (auto& a : m_Props)
		a.Render(sys);
}

void Box2DPhysics_Demo::SetupDemo(System& sys, ModeID id)
{
	//If world not already set, signal and exit out
	if (!m_World)
	{
		msg_assert(false, "SetupDemo(): No world set, cannot proceed!");
		return;
	}

	switch(id)
	{
	case ModeID::BALLPIT:
		Setup_Ballpit(sys);
		break;
	}
}
