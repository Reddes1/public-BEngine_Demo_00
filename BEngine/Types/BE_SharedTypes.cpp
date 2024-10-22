#include "BE_SharedTypes.h"

//Utilities
#include "Utils/Utils_Debug.h"
#include "Utils/Utils_D3D_Debug.h"

//Library Includes
#include "DirectXHelpers.h"			//Get Texture Size

//Engine Includes
#include "Includes/BE_All_Managers.h"

void SFString::Draw()
{
	msg_assert(m_Font && m_Batch, "Draw(): No font and/or batch set!");
	m_Font->DrawString(
		m_Batch,
		m_DrawableStr.c_str(),
		m_Position + m_PositionOffset,
		m_Colour,
		m_Rotation,
		m_Origin,
		m_Scale,
		m_Effect,
		m_LayerDepth
	);
}

void SFString::Draw(DirectX::SpriteBatch* batch)
{
	msg_assert(m_Font && batch, "Draw(): No font and/or batch set!");
	m_Font->DrawString(
		batch,
		m_DrawableStr.c_str(),
		m_Position + m_PositionOffset,
		m_Colour,
		m_Rotation,
		m_Origin,
		m_Scale,
		m_Effect,
		m_LayerDepth
	);
}

void SFString::Draw(DirectX::SpriteFont* font)
{
	msg_assert(font && m_Batch, "Draw(): No font and/or batch set!");
	font->DrawString(
		m_Batch,
		m_DrawableStr.c_str(),
		m_Position + m_PositionOffset,
		m_Colour,
		m_Rotation,
		m_Origin,
		m_Scale,
		m_Effect,
		m_LayerDepth
	);
}

void SFString::Draw(DirectX::SpriteFont* font, DirectX::SpriteBatch* batch)
{
	msg_assert(font && batch, "Draw(): No font and/or batch set!");
	font->DrawString(
		batch,
		m_DrawableStr.c_str(),
		m_Position + m_PositionOffset,
		m_Colour,
		m_Rotation,
		m_Origin,
		m_Scale,
		m_Effect,
		m_LayerDepth
	);
}

void SFString::UpdateOrigin()
{
    msg_assert(m_Font, "UpdateOrigin(): No SF set!");

	//Call justification calc
    CalcStringJustification(m_Font, (StringJustificationID)m_JustificationID, m_DrawableStr, m_Origin);
}

void SFString::UpdateOrigin(StringJustificationID id)
{
	msg_assert(m_Font, "UpdateOrigin(): No SF set!");

	//Set new ID and call calc
	m_JustificationID = (unsigned short)id;
	CalcStringJustification(m_Font, (StringJustificationID)m_JustificationID, m_DrawableStr, m_Origin);
}

void SFString::SetNewString(const std::string& str, bool updateOrigin)
{
    //Set new drawable
    m_DrawableStr = str;

    //Update origin if flagged
    if (updateOrigin)
        UpdateOrigin();
}

void SFString::SetBothStrings(const std::string& str)
{
	//Reset both strings with new string
	m_DefaultStr = str;
	m_DrawableStr = m_DefaultStr;
}

void SpriteData::Draw()
{
    m_Batch->Draw(
        m_Texture->m_Heap->GetGpuHandle(m_Texture->m_HeapIndex),
        m_TexSize,
        m_Position + m_PositionOffset,
        &m_FrameRect,
        m_Colour,
        m_Rotation + m_RotationOffset,
        m_Origin,
        m_Scale,
        m_SprEffect,
        m_LayerDepth
    );
}

void SpriteData::Draw(DirectX::SpriteBatch* batch)
{
	batch->Draw(
		m_Texture->m_Heap->GetGpuHandle(m_Texture->m_HeapIndex),
		m_TexSize,
		m_Position + m_PositionOffset,
		&m_FrameRect,
		m_Colour,
		m_Rotation + m_RotationOffset,
		m_Origin,
		m_Scale,
		m_SprEffect,
		m_LayerDepth
	);
}

bool SpriteData::SetTexture(std::string& texName, System& sys)
{
    //Attempt to find target texture from resource manager
	SpriteTexture* tex = sys.m_TexMgr->FindTextureData(texName);
    msg_assert(tex, "SetTexture(): No texture found!");

    //If texture found, finish setup
    if (tex)
    {
		//Set texture
		m_Texture = tex;
		//Store texture size
		m_TexSize = DirectX::GetTextureSize(m_Texture->m_TextureResource.Get());
        //Should be at least one frame, set to first frame by default
        SetFrame(0);

        return true;
    }

	//No texture found, return false
    return false;
}

bool SpriteData::SetTexture(std::shared_ptr<SpriteTexture> tex)
{
    msg_assert(tex, "SetTexture(): No texture found!");
    //If valid texture passed, finish setup
    if (tex)
    {
        //Set texture
        m_Texture = tex.get();
        //Store texture size
        m_TexSize = DirectX::GetTextureSize(m_Texture->m_TextureResource.Get());
        //Should be at least one frame, set to first frame by default
        SetFrame(0);

        return true;
    }

	//No texture set, return false
	return false;
}

bool SpriteData::SetTexture(SpriteTexture* tex)
{
	msg_assert(tex, "SetTexture(): No texture found!");
	//If valid texture passed, finish setup
	if (tex)
	{
		//Set texture
		m_Texture = tex;
		//Store texture size
		m_TexSize = DirectX::GetTextureSize(m_Texture->m_TextureResource.Get());
		//Should be at least one frame, set to first frame by default
		SetFrame(0);

		return true;
	}

	//No texture set, return false
	return false;
}

void SpriteData::SetFrame(int index)
{
    assert(index >= 0 && index <= m_Texture->m_Frames.size());
    m_FrameRect = m_Texture->m_Frames[index];
    m_Origin = m_Texture->m_Origins[index];
}

void SpriteAnimator::Update(float dTime)
{
	//Skip out if not flagged to play
	if (!m_Data.m_Play)
		return;

	//Updates differently based on animation type.
	//Do NOT erronously change type ID, will cause undefined beheviour.
	switch (m_CurrentAnim->m_TypeID)
	{
	case(AnimationData::AnimID::LINEAR_FRAMES):
		Update_LinearAnimation(dTime);
		break;
	case(AnimationData::AnimID::NON_LINEAR_FRAMES):
		Update_NonLinearAnimation(dTime);
		break;
	}
}

void SpriteAnimator::RestartAnimation(bool play)
{
	//Restart to base frame (based on animation type and reverse state)
	switch (m_Data.m_Reverse)
	{
	case false:
		switch (m_CurrentAnim->m_TypeID)
		{
		case(AnimationData::AnimID::LINEAR_FRAMES):
			m_Data.m_CurrentFrame = m_CurrentAnim->m_StartFrame;
			break;
		case(AnimationData::AnimID::NON_LINEAR_FRAMES):
			m_Data.m_CurrentFrame = m_CurrentAnim->m_FrameIndexes[0];
			break;
		}
		break;

	case true:
		switch (m_CurrentAnim->m_TypeID)
		{
		case(AnimationData::AnimID::LINEAR_FRAMES):
			m_Data.m_CurrentFrame = m_CurrentAnim->m_EndFrame;
			break;
		case(AnimationData::AnimID::NON_LINEAR_FRAMES):
			m_Data.m_CurrentFrame = m_CurrentAnim->m_FrameIndexes[m_CurrentAnim->m_FrameIndexes.size() - 1];
			break;
		}
		break;
	}

	//Set frame & origin
	m_SprData->SetFrame(m_Data.m_CurrentFrame);
	//Reset timer
	m_Data.m_Elapsed = 0.f;
	//Set play flag
	m_Data.m_Play = play;
}

void SpriteAnimator::EnableAnimationLock()
{
	m_Data.m_AnimLockedIn = true;
	m_Data.m_Loop = false;
}

void SpriteAnimator::DisableAnimationLock(bool loop)
{
	m_Data.m_AnimLockedIn = false;
	m_Data.m_Loop = loop;
}

bool SpriteAnimator::BranchAnimation(int animID, bool play, bool loop, bool reverse, bool preserveSpeed)
{
	//Get the new animation
	AnimationData* newAnim = &m_SprData->m_Texture->m_Animations[animID];
	//Calculate length and compare (returning if target anim is not long enough)
	int length = (newAnim->m_EndFrame - newAnim->m_StartFrame) + 1;
	if (length < GetAnimationLength())
		return false;

	//Capture how many steps through the animation have occured so we can sync with the new animation
	int relativeFramePosition = 0;
	if (!m_Data.m_Reverse)
		relativeFramePosition = m_Data.m_CurrentFrame - m_CurrentAnim->m_StartFrame;
	else
		relativeFramePosition = m_CurrentAnim->m_EndFrame - m_Data.m_CurrentFrame;

	//Anim length good, so move to override current animation and set appropriate values
	m_CurrentAnim = newAnim;
	m_Data.m_Play = play;
	m_Data.m_Loop = loop;
	m_Data.m_Reverse = reverse;
	if (!preserveSpeed)
		m_Data.m_Speed = m_CurrentAnim->m_Speed;

	//Now figure out what frame to set on
	if (!reverse)
	{
		m_Data.m_CurrentFrame = m_CurrentAnim->m_StartFrame + relativeFramePosition;
	}
	else
	{
		m_Data.m_CurrentFrame = m_CurrentAnim->m_EndFrame - relativeFramePosition;
	}

	//Finally set the new frame
	m_SprData->SetFrame(m_Data.m_CurrentFrame);

	return true;
}

short SpriteAnimator::GetAnimationLength()
{
	switch (m_CurrentAnim->m_TypeID)
	{
	case AnimationData::AnimID::LINEAR_FRAMES:
		return (m_CurrentAnim->m_EndFrame - m_CurrentAnim->m_StartFrame) + 1;
		break;

	case AnimationData::AnimID::NON_LINEAR_FRAMES:
		return static_cast<short>(m_CurrentAnim->m_FrameIndexes.size());
		break;
	}

	//Shouldn't hit this point
	assert(false);
	return 0;
}

short SpriteAnimator::GetRelativeCurrentFrame()
{
	//Determine how to find current frame (relative the length of the animation)
	switch (m_CurrentAnim->m_TypeID)
	{
	case AnimationData::AnimID::LINEAR_FRAMES:
		if (!m_Data.m_Reverse)
			return GetAnimationLength() - ((m_CurrentAnim->m_EndFrame - m_Data.m_CurrentFrame) + 1);
		else
			return m_CurrentAnim->m_EndFrame - m_Data.m_CurrentFrame;
		break;

	case AnimationData::AnimID::NON_LINEAR_FRAMES:
		return m_Data.m_CurrentFrame;
		break;

	}

	//Shouldn't hit this point
	assert(false);
	return 0;
}

void SpriteAnimator::SetFrame(int newFrame, bool resetElapsed)
{
	switch (m_CurrentAnim->m_TypeID)
	{
	case AnimationData::AnimID::LINEAR_FRAMES:
		m_Data.m_CurrentFrame = newFrame;
		break;

	case AnimationData::AnimID::NON_LINEAR_FRAMES:
		m_Data.m_CurrentFrame = newFrame;
		m_SprData->SetFrame(m_CurrentAnim->m_FrameIndexes[m_Data.m_CurrentFrame]);
		break;
	}

	if (resetElapsed)
		m_Data.m_Elapsed = 0.f;
}

bool SpriteAnimator::SetAnimation(int index, bool play, bool restartIfPlaying, bool loop, bool reverse)
{
	//Check index validity
	bool isValid = index >= 0 && index <= m_SprData->m_Texture->m_Animations.size();
	if (!isValid)
	{
		msg_assert(false, "SetAnimation(): Index invalid!");
		return false;
	}

	//Index ok, check for the following
	/*
		Index check good, now check for the following:
		- Is the current animation in locked state? (Must finish playing before changes through this function are allowed)
		- Is this animation already playing, and should it be restarted if so.
	*/
	if (m_Data.m_AnimLockedIn || (m_CurrentAnim && (m_CurrentAnim->m_AnimationID == index && !restartIfPlaying)))
		return false;

	//Checks all good for changing anim, so set new animation pointer
	m_CurrentAnim = &m_SprData->m_Texture->m_Animations[index];
	
	//Set local data
	m_Data.m_Loop = loop;
	m_Data.m_Reverse = reverse;
	m_Data.m_Speed = m_CurrentAnim->m_Speed;

	//This sets the frame, play status and resets current frame timer
	RestartAnimation(play);

	return true;
}

void SpriteAnimator::Update_LinearAnimation(float dTime)
{
	//Uptick elapsed time
	m_Data.m_Elapsed += dTime;
	//Calculate current frame duration (in terms of Frames Per Second, modified by speed multiplier)
	float frameDuration = (1.f / (m_Data.m_Speed * m_Data.m_AnimSpeedMod));
	//If elapsed over animation speed
	if (m_Data.m_Elapsed > frameDuration)
	{
		//Reset the clock
		m_Data.m_Elapsed = 0.f;

		//Determine how to manage the animation based on reverse flag
		switch (m_Data.m_Reverse)
		{
		case false:
			//Increment the frame count
			++m_Data.m_CurrentFrame;
			//Check if out of animation range
			if (m_Data.m_CurrentFrame > m_CurrentAnim->m_EndFrame)
			{
				//If set to loop, change to first frame, else set frame to end and stop
				if (m_Data.m_Loop)
					m_Data.m_CurrentFrame = m_CurrentAnim->m_StartFrame;
				else
				{
					m_Data.m_CurrentFrame = m_CurrentAnim->m_EndFrame;
					m_Data.m_Play = false;

					//Disable lock
					m_Data.m_AnimLockedIn = false;
				}
			}
			break;

		case true:
			--m_Data.m_CurrentFrame;
			if (m_Data.m_CurrentFrame < m_CurrentAnim->m_StartFrame)
			{
				if (m_Data.m_Loop)
					m_Data.m_CurrentFrame = m_CurrentAnim->m_EndFrame;
				else
				{
					m_Data.m_CurrentFrame = m_CurrentAnim->m_StartFrame;
					m_Data.m_Play = false;

					//Disable lock
					m_Data.m_AnimLockedIn = false;
				}
			}
			break;
		}

		//Set frame post anim update
		m_SprData->SetFrame(m_Data.m_CurrentFrame);
	}
}

void SpriteAnimator::Update_NonLinearAnimation(float dTime)
{
	//Uptick elapsed time
	m_Data.m_Elapsed += dTime;
	//Calculate current frame duration (in terms of Frames Per Second, modified by speed multiplier)
	float frameDuration = (1.f / (m_Data.m_Speed * m_Data.m_AnimSpeedMod));
	//If elapsed over animation speed
	if (m_Data.m_Elapsed > frameDuration)
	{
		//Reset the clock
		m_Data.m_Elapsed = 0.f;

		//Determine how to manage the animation based on reverse flag
		switch (m_Data.m_Reverse)
		{
		case false:
			//Increment the frame count
			++m_Data.m_CurrentFrame;
			//Check if out of animation range
			if (m_Data.m_CurrentFrame > m_CurrentAnim->m_FrameIndexes.size())
			{
				//If set to loop, change to first frame, else set frame to end and stop
				if (m_Data.m_Loop)
					m_Data.m_CurrentFrame = m_CurrentAnim->m_FrameIndexes[0];
				else
				{
					m_Data.m_CurrentFrame = static_cast<short int>(m_CurrentAnim->m_FrameIndexes.size());
					m_Data.m_Play = false;
				}
			}
			break;

		case true:
			--m_Data.m_CurrentFrame;
			if (m_Data.m_CurrentFrame < 0)
			{
				if (m_Data.m_Loop)
					m_Data.m_CurrentFrame = static_cast<short>(m_CurrentAnim->m_FrameIndexes.size());
				else
				{
					m_Data.m_CurrentFrame = m_CurrentAnim->m_FrameIndexes[0];
					m_Data.m_Play = false;
				}
			}
			break;
		}

		//Set frame post anim update
		m_SprData->SetFrame(m_Data.m_CurrentFrame);
	}
}