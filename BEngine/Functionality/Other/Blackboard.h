//*********************************************************************************\\
// 
// Open and expandable container that is usable for several puposes. Generally,
// it can be used as a hub for common, well used data and information that any
// game object would need to access, with the benefit of being editable and free
// to copy.
// 
//*********************************************************************************\\

#pragma once

#include "Utils/Utils_D3D.h"		//D3D Types

/*
	Hosting container for important, commonly used game data.
	Can be accessed and updated as needed by clients
*/
struct GameBlackboard
{
	//
	//Per-Frame Data
	//


	//
	//System Info
	//

	//The current, native window dims (update if native changes, not the window size changing)
	int m_NativeWinX = 1920;
	int m_NativeWinY = 1080;
};