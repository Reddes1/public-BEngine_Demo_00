//*********************************************************************************\\
// 
// Collection of core containers and types used frequently across several key
// managers and functionality classes, as well for options for certain design models.
//
//*********************************************************************************\\

#pragma once

//Library Includes
#include <wrl.h>			//ComPtr
#include <windef.h>			//RECT
#include <DirectXMath.h>	//DirectX types
#include <d3d12.h>			//ID3D12Resource
#include <vector>
#include <string>
#include "DescriptorHeap.h"
#include "ResourceUploadBatch.h"

//Engine Includes
#include "Types/FrameResources.h"

//
//Foward Declarations
//

//Main system container used to pass manager accessors around
struct System;

//================================================================================\\
// General Data Types 
//================================================================================\\

/*
	Composite structure for providing several different data types used in drawing strings
	with Spritefonts. Also provides some additional basic behaviour to get it working out
	of the box, as part of another
*/
struct SFString
{

	////////////////////
	/// Constructors ///
	////////////////////

	SFString() {}
	SFString(const std::string& str) { }
	
	//////////////////
	/// Operations ///
	//////////////////

	//
	//Drawing
	//

	//Standard draw that uses internal batch and font
	void Draw();
	//Draws string to the given batch, using internal font and overriding internal batch (if set)
	void Draw(DirectX::SpriteBatch* batch);
	//Draws string with the given font, using internal batch and overriding internal font (if set)
	void Draw(DirectX::SpriteFont* font);
	//Draws string using both given font and batch
	void Draw(DirectX::SpriteFont* font, DirectX::SpriteBatch* batch);

	//
	//Updating
	//

	//Updates the origin of the string using internal justification position and set font
	void UpdateOrigin();
	/*
		Updates the origin of the string using the given justification ID and internal font.
		Useful if wanting to update origin + data in one call.
	*/
	void UpdateOrigin(StringJustificationID id);

	//Resets drawable string to its default string
	void ResetDrawable() { m_DrawableStr = m_DefaultStr; }

	/////////////////
	/// Accessors ///
	/////////////////

	//Updates the drawable string data, with an optional check for updating the origin after change (true by default)
	void SetNewString(const std::string& str, bool updateOrigin = true);
	//Resets both strings to given string
	void SetBothStrings(const std::string& str);

	//Gets the combined position (main position + offset)
	Vec2 GetAdjustedPosition() { return m_Position + m_PositionOffset; }

	////////////
	/// Data ///
	////////////

	//Keep a pointer to a spritefont so you access it during draws without having to find it
	DirectX::SpriteFont* m_Font = nullptr;
	//As the font needs to be drawn to a batch, also keep a pointer a batch if required
	DirectX::SpriteBatch* m_Batch = nullptr;

	//Often found that a drawable may need to be reset to a norm, keep a copy of that + the drawn version
	std::string m_DefaultStr = "NULL";
	std::string m_DrawableStr = "NULL";

	//Position + offsetting factor
	Vec2 m_Position = { 0.f, 0.f };
	Vec2 m_PositionOffset = { 0.f, 0.f };

	Vec4 m_Colour = DirectX::Colors::White;
	float m_Rotation = 0.f;
	float m_Scale = 1.f;
	float m_LayerDepth = 1.f;
	DirectX::SpriteEffects m_Effect = DirectX::SpriteEffects::SpriteEffects_None;

	/*
		SF fonts use an origin system like regular sprites so we store that, as well as an ID to use with
		CalcStringJustification() util to update the position of the origin around key anchor points.
		The origin would need updated (especially if using a centre or right sided origin) when the string changes length.
	*/
	Vec2 m_Origin = { 0.f, 0.f };
	unsigned short m_JustificationID = 0;
};

//================================================================================\\
// Textures, Sprites & Animations
//================================================================================\\

/*
	Animation structure that stores information about frames relating to an associated texture.
	Doesn't error check with nor know about the associated texture so ensure accuracy in loading process.
*/
struct AnimationData
{
	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	enum class AnimID : unsigned short
	{
		LINEAR_FRAMES,
		NON_LINEAR_FRAMES
	};

	////////////////////
	/// Constructors ///
	////////////////////

	AnimationData()
		:m_Name("N/A"), m_Speed(0), m_StartFrame(0), m_EndFrame(0), m_TypeID(AnimID::LINEAR_FRAMES)
	{
		m_FrameIndexes.reserve(8);
	}
	//Setup for Linear
	AnimationData(std::string& name, short startFrame, short endFrame, float animSpeed)
		:m_Name(name), m_StartFrame(startFrame), m_EndFrame(endFrame), 
		m_Speed(animSpeed), m_TypeID(AnimID::LINEAR_FRAMES)
	{

	}
	~AnimationData()
	{
		m_FrameIndexes.clear();
	}

	std::vector<short> m_FrameIndexes;
	std::string m_Name;
	unsigned m_AnimationID = 0;
	float m_Speed;
	short m_StartFrame;
	short m_EndFrame;
	short m_AnimationLength = 0;
	AnimID m_TypeID;
	unsigned m_ContainerIndex = 0;

	//Uses (End Frame - Start Frame + 1) to determine number of frames in the given animation
	void CalculateAnimLength() { m_AnimationLength = (m_EndFrame - m_StartFrame) + 1; }
};

/*
	Specialised sprite texture container. Used with Mgr_TextureResources to store texture and animation information.
	Represents the totality of a sprites data, such as individual frame rects, related animations, origin adjustments, SRV index and resource pointer.
*/
struct SpriteTexture
{
	////////////////////
	/// Constructors ///
	////////////////////

	~SpriteTexture()
	{
		m_Frames.clear();
		m_Animations.clear();
	}

	////////////
	/// Data ///
	////////////

	//Each frame of the texture (the individual animation frames)
	std::vector<RECT> m_Frames;
	//Origin point for each of the frame in the texture
	std::vector<DirectX::XMFLOAT2> m_Origins;
	//Each animation related to this texture
	std::vector<AnimationData> m_Animations;

	//Texture name (alias used in texture map)
	std::string m_Name = "NULL";
	//Pointer to the resource information directly
	Microsoft::WRL::ComPtr<ID3D12Resource> m_TextureResource = nullptr;
	//Pointer to heap that the texture is located in
	DirectX::DescriptorHeap* m_Heap = nullptr;
	//What index does the texture sit in
	unsigned m_HeapIndex = 0;
};

/*
	Packed together data + simple behaviours to enable sprite drawing via SpriteBatch.
*/
class SpriteData
{
public:


	////////////////////
	/// Constructors ///
	////////////////////

	SpriteData()
		:m_Batch(nullptr), m_TexSize(1920, 1080), m_Position(0, 0), m_PositionOffset(0, 0), m_FrameRect({ 0, 0, 1920, 1080 }),
		m_Colour({ 1.f, 1.f, 1.f, 1.f }), m_Rotation(0.f), m_Origin(0.f, 0.f), m_Scale(1.f, 1.f),
		m_SprEffect(DirectX::SpriteEffects::SpriteEffects_None), m_LayerDepth(1.f), m_RotationOffset(0.f)
	{ }

	//////////////////
	/// Operations ///
	//////////////////

	//Main draw call using all present data
	void Draw();
	//Alternate call that draws with given batch instead of held batch
	void Draw(DirectX::SpriteBatch* batch);

	/////////////////
	/// Accessors ///
	/////////////////

	//Get the relative size of the current frame
	float GetFrameSizeX() { return static_cast<float>(m_FrameRect.right - m_FrameRect.left); }
	float GetFrameSizeY() { return static_cast<float>(m_FrameRect.bottom - m_FrameRect.top); }
	//Gets the relative size of the current frame, adjusted for current scale
	float GetFrameSizeX_Scaled() { return GetFrameSizeX() * m_Scale.x; }
	float GetFrameSizeY_Scaled() { return GetFrameSizeY() * m_Scale.y; }

	//Gets the position with offset factored in
	Vec2 GetAdjustedPosition() { return m_Position + m_PositionOffset; }

	//Sets texture via accessor name (accesses Mgr_TextureResources to do so)
	bool SetTexture(std::string& texName, System& sys);
	//Sets texture using given resource
	bool SetTexture(std::shared_ptr<SpriteTexture> tex);
	bool SetTexture(SpriteTexture* tex);
	
	//Sets frame via texture information (Animator uses this to set frame during animation)
	void SetFrame(int index);

	////////////
	/// Data ///
	////////////
	 
	//Texture data + heap data draw call sources from
	SpriteTexture* m_Texture;
	//Spritebatch being used in draw calls (ensure draw call begun when called)
	DirectX::SpriteBatch* m_Batch;

	//Total size of the texture
	XMUI2 m_TexSize;
	//Main position (should be informed by the actor), and an offsetting factor
	Vec2 m_Position;
	Vec2 m_PositionOffset;
	//Frames dims relative to the texture
	RECT m_FrameRect;
	//Colour/Alpha adjustments to the sprite
	XMVec4 m_Colour;
	//Base rotation in radians (should be informed by the actor), and an offsetting factor
	float m_Rotation;
	float m_RotationOffset;
	//Origin of the sprite relative to the frame
	XMF2 m_Origin;
	//Size of the sprite
	XMF2 m_Scale;
	//Applied effect like flipping on x/y axis'
	DirectX::SpriteEffects m_SprEffect;
	//Depth/layering priority within the draw call (1 = highest, 0 = lowest)
	float m_LayerDepth;

private:

};

class SpriteAnimator
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//
	struct AnimatorData
	{
		AnimatorData()
			:m_Elapsed(0.f), m_Speed(1.f), m_AnimSpeedMod(1.f), m_CurrentFrame(0), m_Play(false), m_Loop(false),
			m_Reverse(false), m_AnimLockedIn(false)
		{}

		float m_Elapsed;
		float m_Speed;
		float m_AnimSpeedMod;
		short m_CurrentFrame;
		bool m_Play : 1;
		bool m_Loop : 1;
		bool m_Reverse : 1;
		bool m_AnimLockedIn : 1;
	};

	////////////////////
	/// Constructors ///
	////////////////////

	SpriteAnimator(SpriteData* sprPtr)
		:m_SprData(sprPtr)
	{}
	~SpriteAnimator() { }

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Main Behaviours
	//

	//Update window that should be called once per frame
	void Update(float dTime);
	//Restarts the current animation and timer
	void RestartAnimation(bool play);

	/*
		Attempts to branch the current animation into another similar or compatible animation.
		The only major restriction is that the new animation must be the same length or longer for
		a technically faultless branch (applicablity of the branch is down the frame + logic involved).
		Some additional optional calls to help maintain consistency between animations if required.
	*/
	bool BranchAnimation(int animID, bool play, bool loop, bool reverse, bool preserveSpeed);

	/*
		Locks the current animation cycle so that new calls won't be able to pre-maturely end the animation. This lock
		is disabled when the animation ends.
		This is incompatible with loops so if loop is enabled it will be disabled.
	*/
	void EnableAnimationLock();

	//Disables current animation lock (can optionally re-enable loop here)
	void DisableAnimationLock(bool loop = false);

	//
	//Utiltiies
	//

	//Rebinds a sprite to this animator (should be the same sprite data, and called when sprite is lost for whatever reason)
	void RebindSprite(SpriteData* spr) { m_SprData = spr; }

	/*
		
	*/

	/////////////////
	/// Accessors ///
	/////////////////

	const AnimationData* GetCurrentAnimation() const { return m_CurrentAnim; }

	float GetSpeed()								 { return m_Data.m_Speed; }
	bool GetPlayState()								 { return m_Data.m_Play; }
	float GetAnimationSpeedMod()					 { return m_Data.m_AnimSpeedMod; }
	bool GetLoopState()								 { return m_Data.m_Loop; }
	bool GetReverse()								 { return m_Data.m_Reverse; }
	short GetCurrentFrame()							 { return m_Data.m_CurrentFrame; }
	short GetFrame()								 { return m_CurrentAnim->m_FrameIndexes[m_Data.m_CurrentFrame]; }
	short GetAnimationLength();
	bool GetAnimationLockState()					 { return m_Data.m_AnimLockedIn; }
	unsigned GetAnimationID()						 { return m_CurrentAnim->m_AnimationID; }
	//Gets the frame relative to the current animation
	short GetRelativeCurrentFrame();

	void SetSpeed(float newSpeed)					 { m_Data.m_Speed = newSpeed; }
	void SetPlay(bool play)							 { m_Data.m_Play = play; }
	void SetLoop(bool loop)							 { m_Data.m_Loop = loop; }
	void SetReverse(bool reverse)					 { m_Data.m_Reverse = reverse; }
	//Sets frame relative to the number in the index container
	void SetFrame(int newFrame, bool resetElapsed = true);

	//Set/Resets an animation with some optional control flags.
	bool SetAnimation(int index, bool play, bool restartIfPlaying = false, bool loop = true, bool reverse = false);

	//Animation speed works via multiplication (e.g. 2.f = 2x Slower AnimSpeed, 0.5f = 2x Faster AnimSpeed)
	void SetAnimationSpeedMod(float modSpeed)		 { m_Data.m_AnimSpeedMod = modSpeed; }

private:

	//////////////////
	/// Operations ///
	//////////////////

	void Update_LinearAnimation(float dTime);
	void Update_NonLinearAnimation(float dTime);

	////////////
	/// Data ///
	////////////

	//Internal animation data (play state, current frame, elapsed etc)
	AnimatorData m_Data;
	//Current animation data being accessed
	const AnimationData* m_CurrentAnim = nullptr;
	//Current sprite class that is being animated by this
	SpriteData* m_SprData = nullptr;
};

//Packaging together of the above types for quick usage (Sort of like the AnimatedSprite Module)
struct AnimatedSprite
{
	//Bind sprite to animator
	AnimatedSprite()
		:m_Animator(&m_SprData)
	{}

	SpriteData m_SprData;
	SpriteAnimator m_Animator;
};

//================================================================================\\
// Misc Containers
//================================================================================\\

/*
	Simple location and ID container when using pool based game design.
*/
struct PoolIDs
{
	unsigned m_PoolLocationID = 0;
	unsigned m_InternalPoolID = 0;
};