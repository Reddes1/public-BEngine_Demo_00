#include "Module_UI_SFString.h"

//Utilities
#include "Utils/Utils_General.h"

//Engine Includes
#include "Includes/BE_All_Modules.h"

Module_UI_SFString::Module_UI_SFString(Actor2D_Interface* actor, DirectX::SpriteFont* font)
	:Module_Interface(actor, std::string("Nameless SFString"), ModuleTypeID::UI_SF_STRING)
{
	msg_assert(font, "Module_UI_SFString(): Invalid font pointer passed!");
	m_StringData.m_Font = font;
}

Module_UI_SFString::Module_UI_SFString(Actor2D_Interface* actor, std::string& name, DirectX::SpriteFont* font)
	:Module_Interface(actor, name, ModuleTypeID::UI_SF_STRING)
{
	msg_assert(font, "Module_UI_SFString(): Invalid font pointer passed!");
	m_StringData.m_Font = font;
}

Module_UI_SFString::Module_UI_SFString(Actor2D_Interface* actor, DirectX::SpriteFont* font, DirectX::SpriteBatch* batch)
	:Module_Interface(actor, std::string("Nameless SFString"), ModuleTypeID::UI_SF_STRING)
{
	msg_assert(font && batch, "Module_UI_SFString(): Invalid font and/or batch pointer passed!");
	m_StringData.m_Font = font;
	m_StringData.m_Batch = batch;
}

Module_UI_SFString::Module_UI_SFString(Actor2D_Interface* actor, std::string& name, DirectX::SpriteFont* font, DirectX::SpriteBatch* batch)
	:Module_Interface(actor, name, ModuleTypeID::UI_SF_STRING)
{
	msg_assert(font && batch, "Module_UI_SFString(): Invalid font and/or batch pointer passed!");
	m_StringData.m_Font = font;
	m_StringData.m_Batch = batch;
}

void Module_UI_SFString::Render(System& sys)
{
	//Call string draw
	m_StringData.Draw();
}

void Module_UI_SFString::Render(System& sys, DirectX::SpriteBatch* batch)
{
	//Call string draw with given batch
	m_StringData.Draw(batch);
}

void Module_UI_SFString::SyncModulePosition(Module_Interface* otherMod)
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
		//Take position from sprite and override current
		m_StringData.m_Position = recast_static(Module_Sprite*, otherMod)->GetSpriteData().m_Position;
	}
	break;
	case ModuleTypeID::ANIMATED_SPRITE:
	{
		//Take position from sprite and override current
		m_StringData.m_Position = recast_static(Module_AnimatedSprite*, otherMod)->GetSpriteData().m_Position;
	}
	break;

	//TBD: Other appropriate modules added

	}
}
