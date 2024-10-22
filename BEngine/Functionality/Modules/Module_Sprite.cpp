#include "Module_Sprite.h"

//Utilities
#include "Utils/Utils_General.h"

//Engine Includes
#include "Managers/Mgr_Box2DPhysics.h"
#include "Includes/BE_All_Modules.h"

Module_Sprite::Module_Sprite(Actor2D_Interface* actor, std::string& moduleName, SpriteTexture* tex, DirectX::SpriteBatch* batch)
	:Module_Interface(actor, moduleName, ModuleTypeID::SPRITE)
{
	//Set batch and texture data
	m_SprData.m_Batch = batch;
	m_SprData.SetTexture(tex);
}

void Module_Sprite::Render(System& sys)
{
	//Use draw call from sprite data
	m_SprData.Draw();
}

void Module_Sprite::Render(System& sys, DirectX::SpriteBatch* batch)
{
	//Use given batch in draw
	m_SprData.Draw(batch);
}

void Module_Sprite::SyncModulePosition(Module_Interface* otherMod)
{
	if (!otherMod)
	{
		msg_assert(false, "SyncModulePosition(): No valid pointer passed!");
		return;
	}

	//Get type from the mod, recast and seek data as required
	switch (otherMod->GetType())
	{
	case ModuleTypeID::SPRITE:
	{
		//Recast to same type and copy over position
		m_SprData.m_Position = recast_static(Module_Sprite*, otherMod)->GetSpriteData().m_Position;
	}
	break;

	case ModuleTypeID::ANIMATED_SPRITE:
	{
		//Take position from sprite and override current
		m_SprData.m_Position = recast_static(Module_AnimatedSprite*, otherMod)->GetSpriteData().m_Position;
	}
	break;

	case ModuleTypeID::BOX2D_RIGIDBODY:
	{
		//Call existing function
		SyncWithRigidBody(*recast_static(Module_Box2D_RigidBody2D*, otherMod));
	}
	break;

	//TBD: Other appropriate modules added

	}
}

void Module_Sprite::SyncWithRigidBody(Module_Box2D_RigidBody2D& rigidbody)
{
	//Get current body position
	const b2Vec2& pos = rigidbody.GetBody()->GetPosition();
	//Update position, converting as needed
	m_SprData.m_Position =
	{
		Mgr_Box2DPhysics::MetresToPixels(pos.x),
		-Mgr_Box2DPhysics::MetresToPixels(pos.y), //Inverted due to coord system differences)
	};
	//Update sprite new rotation (inverted due to coord system differences)
	m_SprData.m_Rotation = -rigidbody.GetBody()->GetAngle();
}
