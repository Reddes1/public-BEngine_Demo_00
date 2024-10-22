//*********************************************************************************\\
// 
// Reserved header for defining custom render functions for use in engine.
//
//*********************************************************************************\\

#pragma once

//Engine Includes
#include "Game.h"
#include "Actors/Actor2D_Interface.h"
#include "Types/SpriteBatch_Wrapper.h"

//
//Custom Binding Functions
//

/*
	Custom bindable function for rendering. Starts batch draw, binds const buffer from FrameResources to slot 2,
	renders given render group (after sorting if required), and then ends the batch.
*/
void Custom_WSpritebatchDraw(W_SpriteBatch* batch, System* sys, std::vector<Actor2D_Interface*>* renderGroup);
