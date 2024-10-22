#include "Entity_DemoProp.h"

//Utils
#include "Utils/Utils_General.h"
#include "Utils/MathHelper.h"

//Engine Includes
#include "All_Managers.h"

//Modules
#include "Modules/Module_Sprite.h"
#include "Modules/Module_Box2D_Body.h"

//Project Includes
#include "Texture_Enums/BE_2DTestingTextureEnums.h"

void Entity_DemoProp::Update_Main(System& sys)
{

}

void Entity_DemoProp::Update_PrePhysics(System& sys)
{

}

void Entity_DemoProp::Update_PostPhysics(System& sys)
{
	//Physics sim done so update the sprite position with the current rigidbody position
	m_Modules[(int)ModuleIndexes::SPRITE].get()->SyncModulePosition(m_Modules[(int)ModuleIndexes::RIGIDBODY].get());
}

void Entity_DemoProp::Update_PreRender(System& sys)
{
	//Submit to render group
	sys.m_GraphicsMgr->SubmitToRenderGroup(m_UtilityIDs.m_RenderGroupIndex, this);
}

void Entity_DemoProp::Render(System& sys)
{
	if (m_Flags.m_CanRender)
		m_Modules[(unsigned)ModuleIndexes::SPRITE]->Render(sys);
}

void Entity_DemoProp::Render(System& sys, DirectX::SpriteBatch* batch)
{
	if (m_Flags.m_CanRender)
		m_Modules[(unsigned)ModuleIndexes::SPRITE]->Render(sys, batch);
}

bool Entity_DemoProp::RunOnceInit(System& sys)
{
	if (m_Flags.m_CanRunOnceInit)
	{
		//Add modules
		AddNewModule<Module_Sprite>(std::string("Sprite"));
		AddNewModule<Module_Box2D_RigidBody2D>(std::string("RigidBody"));
		
		//Set render group ID
		m_UtilityIDs.m_RenderGroupIndex = (unsigned)BE_ManagerEnums::SpritebatchIndexes::MAIN_SCENE;
		
		//Close init flag
		m_Flags.m_CanRunOnceInit = false;
		return true;
	}

	//No init done
	return false;
}

bool Entity_DemoProp::InitProp(PropSetupID id, b2World* targetWorld)
{
	if (!targetWorld)
	{
		msg_assert(false, "InitProp(): No Valid World Passed!");
		return false;
	}

	//Get system data
	System& sys = Game::GetGame()->GetSystems();

	//Get window dims and dims in metres (box2d measurements)
	float winX = (float)sys.m_Game->GetWindowData().m_NativeWidth;
	float winY = (float)sys.m_Game->GetWindowData().m_NativeHeight;
	float winXM = Mgr_Box2DPhysics::PixelsToMetres(winX);
	float winYM = Mgr_Box2DPhysics::PixelsToMetres(winY);

	//Get modules for setup
	Module_Box2D_RigidBody2D* body = recast_static(Module_Box2D_RigidBody2D*, m_Modules[(int)ModuleIndexes::RIGIDBODY].get());
	Module_Sprite* spr = recast_static(Module_Sprite*, m_Modules[(int)ModuleIndexes::SPRITE].get());

	if (!(body && spr))
	{
		msg_assert(false, "InitProp(): Body/Spr Module Missing!");
		return false;
	}

	//Get texture used in this setup
	auto tex = sys.m_TexMgr->FindTextureData(std::string(BE_2DTestingTexture_ALIAS));
	if (!tex)
	{
		msg_assert(false, "InitProp(): No Texture Found!");
		return false;
	}
	spr->GetSpriteData().SetTexture(tex);

	//Setup general body data and initialise it with module
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;

	body->AttachNewBody(
		bodyDef,
		*targetWorld,
		Box2D_UserData_Interface()		//No special data needed, use default
	);

	//Position to middle by default
	bodyDef.position.Set(winXM * 0.5f, winYM * 0.5f);

	//Setup fixture (shape/collider element)
	b2FixtureDef fixDef;
	fixDef.density = 10.f;
	fixDef.friction = 0.5f;
	fixDef.restitution = 0.05f;

	switch (id)
	{
	case PropSetupID::BLOCK_SMALL:
	{
		//Set frame
		spr->GetSpriteData().SetFrame((int)BE_TextureEnums::BE_2DTestingTextureFrames::TO_SQUARE_32x32);

		//Setup shape for fixture
		b2PolygonShape shape;
		shape.SetAsBox(Mgr_Box2DPhysics::PixelsToMetres(16.f), Mgr_Box2DPhysics::PixelsToMetres(16.f));		//Converted half sizes
		fixDef.shape = &shape;

		//Attach fixture to module
		body->AttachNewFixture(fixDef);

		//Set category ID (helps other know what this is at a glance)
		m_CoreIDs.m_CategoryID = (int)BE_Actor_Enums::CategoryIDs::PROP_TYPE_A;
	}
		break;

	case PropSetupID::BLOCK_MED:
	{
		//Set frame
		spr->GetSpriteData().SetFrame((int)BE_TextureEnums::BE_2DTestingTextureFrames::TO_SQUARE_64x64);

		//Setup shape for fixture
		b2PolygonShape shape;
		shape.SetAsBox(Mgr_Box2DPhysics::PixelsToMetres(32.f), Mgr_Box2DPhysics::PixelsToMetres(32.f));		//Converted half sizes
		fixDef.shape = &shape;

		//Attach fixture to module
		body->AttachNewFixture(fixDef);

		//Set category ID (helps other know what this is at a glance)
		m_CoreIDs.m_CategoryID = (int)BE_Actor_Enums::CategoryIDs::PROP_TYPE_A;
	}
		break;

	case PropSetupID::BLOCK_LARGE:
	{
		//Set frame
		spr->GetSpriteData().SetFrame((int)BE_TextureEnums::BE_2DTestingTextureFrames::TO_SQUARE_128x128);

		//Setup shape for fixture
		b2PolygonShape shape;
		shape.SetAsBox(Mgr_Box2DPhysics::PixelsToMetres(64.f), Mgr_Box2DPhysics::PixelsToMetres(64.f));		//Converted half sizes
		fixDef.shape = &shape;

		//Attach fixture to module
		body->AttachNewFixture(fixDef);

		//Set category ID (helps other know what this is at a glance)
		m_CoreIDs.m_CategoryID = (int)BE_Actor_Enums::CategoryIDs::PROP_TYPE_A;
	}
		break;

	case PropSetupID::BALL_SMALL:
	{
		//Set frame
		spr->GetSpriteData().SetFrame((int)BE_TextureEnums::BE_2DTestingTextureFrames::TO_CIRCLE_32x32);
		b2CircleShape shape;
		shape.m_p.Set(0.f, 0.f);
		shape.m_radius = Mgr_Box2DPhysics::PixelsToMetres(16.f);	//Converted half size
		fixDef.shape = &shape;
		body->AttachNewFixture(fixDef);

		//Set category ID (helps other know what this is at a glance)
		m_CoreIDs.m_CategoryID = (int)BE_Actor_Enums::CategoryIDs::PROP_TYPE_B;
	}
		break;

	case PropSetupID::BALL_MED:
	{
		//Set frame
		spr->GetSpriteData().SetFrame((int)BE_TextureEnums::BE_2DTestingTextureFrames::TO_CIRCLE_64x64);
		b2CircleShape shape;
		shape.m_p.Set(0.f, 0.f);
		shape.m_radius = Mgr_Box2DPhysics::PixelsToMetres(32.f);	//Converted half size
		fixDef.shape = &shape;
		body->AttachNewFixture(fixDef);

		//Set category ID (helps other know what this is at a glance)
		m_CoreIDs.m_CategoryID = (int)BE_Actor_Enums::CategoryIDs::PROP_TYPE_B;
	}
		break;

	case PropSetupID::BALL_LARGE:
	{
		//Set frame
		spr->GetSpriteData().SetFrame((int)BE_TextureEnums::BE_2DTestingTextureFrames::TO_CIRCLE_128x128);
		b2CircleShape shape;
		shape.m_p.Set(0.f, 0.f);
		shape.m_radius = Mgr_Box2DPhysics::PixelsToMetres(64.f);	//Converted half size
		fixDef.shape = &shape;
		body->AttachNewFixture(fixDef);

		//Set category ID (helps other know what this is at a glance)
		m_CoreIDs.m_CategoryID = (int)BE_Actor_Enums::CategoryIDs::PROP_TYPE_B;

	}
		break;

	case PropSetupID::WALL:
	{
		//Set frame
		spr->GetSpriteData().SetFrame((int)BE_TextureEnums::BE_2DTestingTextureFrames::TO_SQUARE_512x512);
		//Setup scaling factor (need to shrink on x and expand on y)
		float sX = 1.f / (spr->GetSpriteData().GetFrameSizeY() * 0.125f);
		float sY = MathHelper::GetProportionalScaler(winY, spr->GetSpriteData().GetFrameSizeY());
		spr->GetSpriteData().m_Scale = { sX, sY };
		//Setup shape for fixture
		b2PolygonShape shape;
		shape.SetAsBox(
			Mgr_Box2DPhysics::PixelsToMetres(spr->GetSpriteData().GetFrameSizeX_Scaled() * 0.5f),
			Mgr_Box2DPhysics::PixelsToMetres(spr->GetSpriteData().GetFrameSizeY_Scaled() * 0.5f)
		);
		fixDef.shape = &shape;
		fixDef.density = 10.f;
		fixDef.friction = 0.5f;
		fixDef.restitution = 0.00f;
		//Attach fixture to module
		body->AttachNewFixture(fixDef);

		//Adjust body type to static
		body->GetBody()->SetType(b2_staticBody);

		//Set category ID (helps other know what this is at a glance)
		m_CoreIDs.m_CategoryID = (int)BE_Actor_Enums::CategoryIDs::STATIC_TERRAIN_TYPE_A;

	}	
		break;

	case PropSetupID::FLOOR:
	{
		//Set frame
		spr->GetSpriteData().SetFrame((int)BE_TextureEnums::BE_2DTestingTextureFrames::TO_SQUARE_512x512);
		//Setup scaling factor (need to shrink on x and expand on y)
		float sX = MathHelper::GetProportionalScaler(winX, spr->GetSpriteData().GetFrameSizeX());
		float sY =  1.f / (spr->GetSpriteData().GetFrameSizeY() * 0.125f);
		spr->GetSpriteData().m_Scale = { sX, sY };
		//Setup shape for fixture
		b2PolygonShape shape;
		shape.SetAsBox(
			Mgr_Box2DPhysics::PixelsToMetres(spr->GetSpriteData().GetFrameSizeX_Scaled() * 0.5f),
			Mgr_Box2DPhysics::PixelsToMetres(spr->GetSpriteData().GetFrameSizeY_Scaled() * 0.5f)
		);
		fixDef.shape = &shape;
		fixDef.density = 10.f;
		fixDef.friction = 0.5f;
		fixDef.restitution = 0.00f;
		//Attach fixture to module
		body->AttachNewFixture(fixDef);
		//Adjust body type to static
		body->GetBody()->SetType(b2_kinematicBody);

		//Set category ID (helps other know what this is at a glance)
		m_CoreIDs.m_CategoryID = (int)BE_Actor_Enums::CategoryIDs::STATIC_TERRAIN_TYPE_A;

	}
	break;

	}

	//Setup done
	return true;
}
