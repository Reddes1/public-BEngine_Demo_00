//*********************************************************************************\\
// 
// Box2D listener interfaces that slot inbetween Box2D listener classes and derived
// custom listener implementations. Gives room for extra, generic data to help with
// using custom implementations in terms for information and identification
//
//*********************************************************************************\\

#pragma once

//Library Includes
#include <string>

#include "box2d/b2_world_callbacks.h"
#include "box2d/b2_contact.h"

/*
	Or smaller class definitions here!
*/
class Box2D_ContactListener_Interface : public b2ContactListener
{
public:

	//
	//Note: See b2ContactListener for virtuals to fulfil in derived classes
	//

	////////////
	/// Data ///
	////////////

	//Verbose ID
	std::string m_Name = "Nameless Contact Listener";
	//Type/class ID (must be defined elsewhere). -1 = no derived type
	//Helps with determining types if unknown without type checks
	int m_TypeID = -1;
};