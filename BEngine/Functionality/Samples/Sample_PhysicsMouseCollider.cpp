#include "Sample_PhysicsMouseCollider.h"

#include "Utils/Utils_General.h"
#include "All_Managers.h"

#include "Modules/Module_Box2D_Body.h"

Sample_PhysicsMouseCollider::Sample_PhysicsMouseCollider()
{
	AddNewModule<Module_Box2D_RigidBody2D>(std::string("Main_Collider"));
}

void Sample_PhysicsMouseCollider::Update_Main(System& sys)
{
	//Get recasted module
	Module_Box2D_RigidBody2D* bodyMod = recast_static(Module_Box2D_RigidBody2D*, m_Modules[0].get());
	//Get current position
	b2Vec2 bPos = bodyMod->GetBody()->GetPosition();
	//Get mouse position and convert into metres and b2Vec2 type
	Vec2 mPos = sys.m_KBMMgr->GetMouseRelativePosition();
	b2Vec2 b2Pos = { Mgr_Box2DPhysics::PixelsToMetres(mPos.x), -Mgr_Box2DPhysics::PixelsToMetres(mPos.y) };

	b2Vec2 vel = (b2Pos - bPos);
	vel.x /= sys.m_GameTime->DeltaTime();
	vel.y /= sys.m_GameTime->DeltaTime();
	bodyMod->GetBody()->SetLinearVelocity(vel);
}

bool Sample_PhysicsMouseCollider::Init(b2World* world, float colliderRadius)
{
	//Resource checks
	if (!world)
	{
		msg_assert(false, "Init(): No valid World pointer passed!");
		return false;
	}

	//Get recasted module
	Module_Box2D_RigidBody2D* bodyMod = recast_static(Module_Box2D_RigidBody2D*, m_Modules[0].get());


	b2BodyDef bodyDef;
	bodyDef.type = b2_kinematicBody;
	Vec2 mPos = Game::GetGame()->GetSystems().m_KBMMgr->GetMouseRelativePosition();
	bodyDef.position.Set(Mgr_Box2DPhysics::PixelsToMetres(mPos.x), -Mgr_Box2DPhysics::PixelsToMetres(mPos.y));	

	//Setup body through module
	b2Body* body = bodyMod->AttachNewBody(
		bodyDef,
		*world,
		Box2D_UserData_Interface()		//Use interface (no data requirements) 
	);

	body->SetLinearDamping(100.0f);

	b2CircleShape shape;
	shape.m_radius = colliderRadius;

	//Create fixture
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 0.0f;
	fixtureDef.friction = 1.0f;
	fixtureDef.restitution = 0.0f;

	body->CreateFixture(&fixtureDef);

	return false;
}
