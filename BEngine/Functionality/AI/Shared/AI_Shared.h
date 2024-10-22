//*********************************************************************************\\
// 
// Overlapping descriptions and behaviours used between different AI models. Done
// this way to help models communicate between each other with common meanings and
// shared types as needed (as well as any other classes needing them).
//
//*********************************************************************************\\

#pragma once

//================================================================================\\
// Definitions
//================================================================================\\

namespace BE_AI 
{
	//Describes AI return codes uses across different models
	enum ConditionCodes
	{
		//
		//General
		//

		SUCCESS = 1,
		FAILURE = 0,
		RUNNING = 2,

		//
		//Behaviour Trees
		//

		BT_ROOT_RESET = 100,
		BT_PARENT_RUNNING,
		BT_PARENT_DONE,
		BT_CONDITION_OK,
		BT_CONDITION_ERROR,
		BT_PARENT_COMPLETE

		//
		//Fuzzy Logic
		//

	};
}