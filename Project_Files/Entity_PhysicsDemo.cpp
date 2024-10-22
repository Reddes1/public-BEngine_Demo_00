#include "Entity_PhysicsDemo.h"

//Utils
#include "Utils/Utils_General.h"
#include "Utils/MathHelper.h"

//Engine Includes
#include "Includes/All_Managers.h"

//Modules
#include "Modules/Module_Sprite.h"
#include "Modules/Module_Box2D_Body.h"

void Entity_PhysicsDemo::Update_Main(System& sys)
{

}

void Entity_PhysicsDemo::Update_PrePhysics(System& sys)
{

}

void Entity_PhysicsDemo::Update_PostPhysics(System& sys)
{
	//Sync sprite and rigidbody position and rotation (so that rigidbody informs sprite position and rotation)
	Module_Box2D_RigidBody2D* body = RECAST_STATIC(Module_Box2D_RigidBody2D*, m_Modules[(int)ModuleIndexes::RIGIDBODY].get());
	Module_Sprite* spr = RECAST_STATIC(Module_Sprite*, m_Modules[(int)ModuleIndexes::SPRITE].get());
	spr->SyncWithRigidBody(*body);
}

void Entity_PhysicsDemo::Update_PreRender(System& sys)
{
	//Submit to render group
	sys.m_GraphicsMgr->SubmitToRenderGroup((unsigned)BE_ManagerEnums::SpritebatchIndexes::MAIN_SCENE, this);
}

void Entity_PhysicsDemo::Render(System& sys)
{
	m_Modules[(unsigned)ModuleIndexes::SPRITE]->Render(sys);
}

void Entity_PhysicsDemo::Render(System& sys, DirectX::SpriteBatch* batch)
{
	m_Modules[(unsigned)ModuleIndexes::SPRITE]->Render(sys, batch);
}

bool Entity_PhysicsDemo::RunOnceInit(System& sys)
{
	AddNewModule<Module_Sprite>(std::string("Physics_Sprite"));
	AddNewModule<Module_Box2D_RigidBody2D>(std::string("Physics_RigidBody"));

	return true;
}
