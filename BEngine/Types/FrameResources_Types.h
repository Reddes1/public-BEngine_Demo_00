//*********************************************************************************\\
// 
// Pre-defined, modifiable header for exposing types to "FrameResources.h" for use.
// Define or include types here for use in other file.
//
//*********************************************************************************\\

#pragma once

/*
	Example buffer of a general, per-frame buffer (Frame-level, not object-level, data)
*/
struct CBuffer_PerFrame
{
	//Overall/Total game time
	float m_GameTime = 0.f;
	//Current frame time
	float m_DeltaTime = 0.f;
};