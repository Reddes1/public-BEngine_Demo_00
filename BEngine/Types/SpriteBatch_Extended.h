//*********************************************************************************\\
// 
// Heavily WIP class that aims to extend the innate functionality of DirectXTK
// by allowing more customisation beyond what is currently available. This is 
// mostly in allowing more control in the draw setup through more customisable
// ConstBuffer setting, Register slots in general, and input layouts. 
// 
// Additionally, as spritebatching seems to only allow for one PerFrame-esque 
// ConstBuffer and not per-sprite/per-object ConstBuffers, this may extend to 
// allowing for that (though this may just utilise the ImmediateMode version
// of batching for simplicity).
//
//*********************************************************************************\\

/*
	Observational notes:
	- Impl = Private Implementation that you can see in the .cpp file properly
		- Hides private behaviour that isn't meant to be visible to the SpriteBatch that may be called by users incorrectly?
		- Pretty cool stuff :)
	- Need to be able to tap into the Impl behaviour as that is where all the real data is.
	- When Draw() is called it sets up required data for operation (the transforms for instance) and then
		calls the Impl->Draw() call.

	- Can't see a way around this outside of modifying the Spritebatch.h and .cpp Impl file to use
	specific call alongside the innate behaviour.
	- Need to change Spritebatch.h from private to protected.

	- Can make some of the Impl visible by copying the calls needed and trusting the implementation 
	to get it right via the pointer. This reduces the amount of changes needed to the library (sorta)
	whist making clear here what is being added.

	- The custom behaviour does need to added to the Impl struct however, with clear distinction on
	new stuff being added.

	- Can consider "stealing" the files required to make it work, modify it and seperate it from 
	the library and bake it in completely.
*/

#pragma once

#include "SpriteBatch.h"

class PH_Class : public DirectX::SpriteBatch
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	//////////////////
	/// Operations ///
	//////////////////

	/////////////////
	/// Accessors ///
	/////////////////

private:

	//////////////////
	/// Operations ///
	//////////////////
	
	////////////
	/// Data ///
	////////////
};
