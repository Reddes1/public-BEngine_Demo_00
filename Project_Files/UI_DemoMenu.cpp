#include "UI_DemoMenu.h"

//Utils
#include "Utils/Utils_General.h"
#include "Utils/Utils_Debug.h"

//Project Includes
#include "Texture_Enums/BE_2DTestingTextureEnums.h"
#include "All_Managers.h"

//Modules
#include "Modules/Module_Sprite.h"
#include "Modules/Module_AnimatedSprite.h"
#include "Modules/Module_UI_MouseCollider.h"
#include "Modules/Module_UI_SFString.h"

void UI_DemoMenu::Update_Main(System& sys)
{
	//Only update if active
	if (m_Flags.m_IsActive)
	{
		//Get and cycle through colliders
		std::vector<Module_UI_MouseCollider*> colliders = GetTModuleList<Module_UI_MouseCollider>(Module_Interface::ModuleTypeID::UI_MOUSE_COLLIDER);
		for (unsigned i(0); i < colliders.size(); ++i)
		{	
			//Each collider is bound to a sprite, so sync collider to sprite position (as it could have been moved)
			colliders[i]->SyncPositionWithSpriteData(colliders[i]->GetAssociatedObject<Module_Sprite>()->GetSpriteData());

			//Is this collider being hovered?
			if (colliders[i]->IsMouseHovering())
			{
				//Pass collider to apply on-hover behaviour
				OnHover(sys, colliders[i]);

				//Check for LC, and run input logic if detected
				if (sys.m_KBMMgr->IsMouseButtonPressedNoRepeat(Mgr_InputKBM::ButtonTypes::LBUTTON))
					OnInput(sys, colliders[i]);
			}
			//Check if this was being hovered (reverts on hover behaviour)
			else if (colliders[i]->WasMouseHovering())
			{
				//Pass collider to apply post-hover behaviour
				OnHoverEnd(sys, colliders[i]);
			}
		}
	}
}

void UI_DemoMenu::Update_PreRender(System& sys)
{
	//Submit to render group
	sys.m_GraphicsMgr->SubmitToRenderGroup((unsigned)BE_ManagerEnums::SpritebatchIndexes::MAIN_SCENE, this);
}

void UI_DemoMenu::Render(System& sys)
{
	//If can render in general, go to checking individual render status
	if (m_Flags.m_CanRender)
		//Call render on modules (only rendering rendables)
		for (auto& a : m_Modules)
			if (a->GetFlags().m_CanRender)
				a->Render(sys);
}

void UI_DemoMenu::Render(System& sys, DirectX::SpriteBatch* batch)
{
	//If can render in general, go to checking individual render status
	if(m_Flags.m_CanRender)
		//Call render on modules (only rendering rendables)
		for (auto& a : m_Modules)
			if (a->GetFlags().m_CanRender)
				a->Render(sys, batch);
}

bool UI_DemoMenu::RunOnceInit(System& sys)
{
	if (m_Flags.m_CanRunOnceInit)
	{
		//
		//Data Setup
		//

		//Get texture shared by all sprites
		SpriteTexture* tex = sys.m_TexMgr->FindTextureData(std::string(BE_2DTestingTexture_ALIAS));

		//Get frames used in this setup
		int frms[] = {
			(int)BE_TextureEnums::BE_2DTestingTextureFrames::UI_02_PANEL_BLUE_00,
			(int)BE_TextureEnums::BE_2DTestingTextureFrames::UI_02_ARROW_00,
			(int)BE_TextureEnums::BE_2DTestingTextureFrames::UI_02_ARROW_00
		};

		//Default positioning
		float posX = sys.m_Blackboard->m_NativeWinX * 0.5f;
		float posY = sys.m_Blackboard->m_NativeWinY * 0.5f;

		//
		//Setup Sprites
		//

		for (unsigned i(0); i < (int)ModuleCounts::SPRITE_COUNT; ++i)
		{
			//Add new module, keep the pointer
			Module_Sprite* spr = AddNewModule<Module_Sprite>();
			//Set texture + frame
			spr->GetSpriteData().SetTexture(tex);
			spr->GetSpriteData().SetFrame(frms[i]);

			//Set position + offsets
			spr->GetSpriteData().m_Position = { posX + m_Offsets[i].x, posY + m_Offsets[i].y };

			if (i == 1)
				spr->GetSpriteData().m_SprEffect = DirectX::SpriteEffects::SpriteEffects_FlipHorizontally;
		}

		//
		//Setup Colliders
		//

		//Indexes to access for sprites
		unsigned sprIndexes[(int)ModuleCounts::COLLIDER_COUNT] =
		{
			(int)ModuleIndexes::SPRITE_ARROW_LEFT,
			(int)ModuleIndexes::SPRITE_ARROW_RIGHT
		};

		for (unsigned i(0); i < (int)ModuleCounts::COLLIDER_COUNT; ++i)
		{
			Module_UI_MouseCollider* collider = AddNewModule<Module_UI_MouseCollider>();
			Module_Sprite* spr = recast_static(Module_Sprite*, m_Modules[sprIndexes[i]].get());

			//Setup collider parameters
			DirectX::BoundingBox box;
			box.Extents =
			{
				spr->GetSpriteData().GetFrameSizeX_Scaled() * 0.45f,
				spr->GetSpriteData().GetFrameSizeY_Scaled() * 0.45f,
				1.f
			};
			box.Center =
			{
				spr->GetSpriteData().GetAdjustedPosition().x,
				spr->GetSpriteData().GetAdjustedPosition().y,
				1.f
			};

			//Assign to module
			collider->SetAsBoundingBox(box);

			//Submit to UI manager
			sys.m_UIMgr->AddCollider(collider);

			//Give sprite pointer to the collider (allowing easier access to what the collider is effecting later)
			collider->SetPairedObjectPointer(spr);

			//Specifically setup internal ID based on i (used later in determining what the object is in context of the UI)
			switch (i)
			{
				//Left Arrow Collider
			case 0:
				collider->SetPairedObjectID((int)ModuleIndexes::M_COLLIDER_ARROW_LEFT);
				break;

				//Left Arrow Collider
			case 1:
				collider->SetPairedObjectID((int)ModuleIndexes::M_COLLIDER_ARROW_RIGHT);
				break;
			}
		}

		//
		//SFString(s)
		//

		//Get font for this string
		DirectX::SpriteFont* font = sys.m_GraphicsMgr->GetSpritefont((unsigned)BE_ManagerEnums::SpritefontIndexes::CALABRI_22);
		//Add modules for box message
		Module_UI_SFString* str = AddNewModule<Module_UI_SFString>(font);

		//Set string message
		str->GetStringData().SetBothStrings(std::string("PH_NAME"));
		//Setup justification and origin
		str->GetStringData().UpdateOrigin(StringJustificationID::CENTER);
		//Adjust colour
		str->GetStringData().m_Colour = Colours::Black;
		//Sync position to dialog box element
		str->SyncModulePosition(m_Modules[(unsigned)ModuleIndexes::SPRITE_DIALOG_BOX].get());
	}

	return false;
}

void UI_DemoMenu::MoveTo(const Vec2& pos)
{
	std::vector<Module_Sprite*> sprites = GetTModuleList<Module_Sprite>(Module_Interface::ModuleTypeID::SPRITE);
	msg_assert(sprites.size() == (int)ModuleCounts::SPRITE_COUNT, "MoveTo(): Less/More sprites than expected, will crash!");

	for (unsigned i(0); i < sprites.size(); ++i)
		sprites[i]->GetSpriteData().m_Position = pos + m_Offsets[i];

	//Manually sync the msg box string to the box position
	m_Modules[(int)ModuleIndexes::SFS_BOX_MSG].get()->SyncModulePosition(m_Modules[(int)ModuleIndexes::SPRITE_DIALOG_BOX].get());
}

void UI_DemoMenu::ForceSceneNameUpdate()
{
	//Get string data (for repeat accesses)
	SFString& strData = recast_static(Module_UI_SFString*, m_Modules[(unsigned)ModuleIndexes::SFS_BOX_MSG].get())->GetStringData();
	//Update string via funcptr
	m_FuncsA[(unsigned)FuncSet_A_Indexes::GET_SCENE_NAME](cast_void_ptr(&strData.m_DrawableStr));
	//Update string origin (as it changed)
	strData.UpdateOrigin();
}

void UI_DemoMenu::BindFuncToSlot(FuncSet_A_Indexes index, VoidFuncA& func)
{
	//Check that count not being used
	bool check = index != FuncSet_A_Indexes::FUNC_PTR_COUNT;
	if (!check)
	{
		msg_assert(false, "BindFuncToSlot(): Index OOR!");
	}
	//Bind new function
	m_FuncsA[(unsigned)index] = func;
}

void UI_DemoMenu::BindFuncToSlot(FuncSet_B_Indexes index, VoidFuncB& func)
{
	bool check = index != FuncSet_B_Indexes::FUNC_PTR_COUNT;
	if (!check)
	{
		msg_assert(false, "BindFuncToSlot(): Index OOR!");
	}
	//Bind new function
	m_FuncsB[(unsigned)index] = func;
}

void UI_DemoMenu::SetDepth(float depth)
{
	//Loop whole container, and set depths for certain types
	for (auto& a : m_Modules)
	{
		switch (a->GetType())
		{
		case Module_Interface::ModuleTypeID::SPRITE:
			recast_static(Module_Sprite*, a.get())->GetSpriteData().m_LayerDepth = depth;
			break;

		case Module_Interface::ModuleTypeID::ANIMATED_SPRITE:
			recast_static(Module_AnimatedSprite*, a.get())->GetSpriteData().m_LayerDepth = depth;
			break;

		case Module_Interface::ModuleTypeID::UI_MOUSE_COLLIDER:
			recast_static(Module_UI_MouseCollider*, a.get())->SetDepthLevel(depth);
			break;

		case Module_Interface::ModuleTypeID::UI_SF_STRING:
			recast_static(Module_UI_SFString*, a.get())->GetStringData().m_LayerDepth = depth;
			break;
		}
	}

}

bool UI_DemoMenu::OnHover(System& sys, Module_UI_MouseCollider* collider)
{
	//Use the colliders unique ID to determine what the collider is paired with (arrow or body), and act on that accordingly
	switch ((ModuleIndexes)collider->GetPairedObjectID())
	{
		//If either arrow passed, just set colour for now
	case ModuleIndexes::M_COLLIDER_ARROW_LEFT:
	case ModuleIndexes::M_COLLIDER_ARROW_RIGHT:
		collider->GetAssociatedObject<Module_Sprite>()->GetSpriteData().m_Colour = DirectX::Colors::SkyBlue;
		break;
	}

	return false;
}

bool UI_DemoMenu::OnHoverEnd(System& sys,Module_UI_MouseCollider* collider)
{
	//Use the colliders unique ID to determine what the collider is paired with (arrow or body), and act on that accordingly
	switch ((ModuleIndexes)collider->GetPairedObjectID())
	{
		//If either arrow passed, just set colour for now
	case ModuleIndexes::M_COLLIDER_ARROW_LEFT:
	case ModuleIndexes::M_COLLIDER_ARROW_RIGHT:
		collider->GetAssociatedObject<Module_Sprite>()->GetSpriteData().m_Colour = DirectX::Colors::White;
		break;
	}

	return false;
}

bool UI_DemoMenu::OnInput(System& sys, Module_UI_MouseCollider* collider)
{
	//Use the colliders unique ID to determine what the collider is paired with (arrow or body), and act on that accordingly
	switch ((ModuleIndexes)collider->GetPairedObjectID())
	{
		//Managing scene change (Left arrow)
	case ModuleIndexes::M_COLLIDER_ARROW_LEFT:
		{
			//Define code then call scene switch
			int code = 0;
			m_FuncsB[(unsigned)FuncSet_B_Indexes::CHANGE_BG_SCENE](static_cast<void*>(&sys), static_cast<void*>(&code));

			//Get string data (for repeat accesses)
			SFString& strData = recast_static(Module_UI_SFString*, m_Modules[(unsigned)ModuleIndexes::SFS_BOX_MSG].get())->GetStringData();
			//Update string via funcptr
			m_FuncsA[(unsigned)FuncSet_A_Indexes::GET_SCENE_NAME](cast_void_ptr(&strData.m_DrawableStr));
			//Update string origin (as it changed)
			strData.UpdateOrigin();

			return true;
		}
		break;

		//Managing scene change (Right arrow)
	case ModuleIndexes::M_COLLIDER_ARROW_RIGHT:
		{	
			//Change code then call 
			int code = 1;
			m_FuncsB[(unsigned)FuncSet_B_Indexes::CHANGE_BG_SCENE](static_cast<void*>(&sys), static_cast<void*>(&code));

			//Get string data (for repeat accesses)
			SFString& strData = recast_static(Module_UI_SFString*, m_Modules[(unsigned)ModuleIndexes::SFS_BOX_MSG].get())->GetStringData();
			//Update string via funcptr
			m_FuncsA[(unsigned)FuncSet_A_Indexes::GET_SCENE_NAME](cast_void_ptr(&strData.m_DrawableStr));
			//Update string origin (as it changed)
			strData.UpdateOrigin();

			return true;
		}
		
		break;

	}

	return false;
}