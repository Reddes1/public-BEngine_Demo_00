//*********************************************************************************\\
// 
// Collection of return codes for the purpose of communicating generally shared
// defintions when returning from functions. Used in several different areas of 
// the engine and can be used generally other functions as required (Heavy WIP
// and not fully implemented yet).
//
//*********************************************************************************\\

#pragma once

/*
	Return message structure for functions to use (WIP)
*/
enum class RetCode : int
{
	//
	//Basic Return Codes
	//

	OK = 0,						//Basic, no information good outcome
	FAIL,						//Basic, no information bad outcome
	ERROR_UNKNOWN,				//Unknown outcomes (probable undefined behaviours)

	//
	//General Return Codes
	//

	INVALID_ARG = 1000,			//OOR argument, not appropriate etc
	NULL_PTR,					//Nullptr, Invalid reference etc
	OOM,						//Out of Memory
	NYI,						//Functionality not complete
	TIMEOUT,					//Took too long to operate
	RESOURCE_NOT_FOUND,			//Any resource not found
	CONNECTION_FAILED,			//Failed to connect generally
	OP_ABORTED,					//Operation/Function was aborted

	//
	//Engine Return Codes
	//

	RENDERING_ERROR = 2000,		//Generic render error
	SHADER_ERROR,				//Generic shader error
	TEXTURE_ERROR,				//Generic texture error
	AUDIO_ERROR,				//Generic audio error
	PHYSICS_ERROR,				//Generic physics error
	INIT_ERROR,					//Generic init error 
	CONFIG_ERROR,				//Generic config error
	ENTITY_NOT_FOUND,			//Unable able to find entity generally

	//
	//Module Return Codes
	//

	//UI RETURN CODES
	UI_MOUSE_COLLISION_POSITIVE = 3000,		//Mouse position overlap with UI element confirmed
};