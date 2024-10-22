#include "Utils_Box2D.h"

//Engine Interface
#include "Types/Box2D_UserData_Interface.h"

//Library Interface
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_joint.h"

void ReleaseBox2DBody(b2Body* body)
{
	//If no body passed, don't do anything
	if (!body)
		return;

	//Cycle through the body fixture list first
	b2Fixture* currFix = body->GetFixtureList();
	while (currFix != nullptr)
	{
		Box2D_UserData_Interface* userData = reinterpret_cast<Box2D_UserData_Interface*>(currFix->GetUserData().pointer);
		if (userData)
		{
			delete userData;
			currFix->GetUserData().pointer = 0;
		}
		currFix = currFix->GetNext();
	}

	//Cycle through body joints
	b2JointEdge* currJoint = body->GetJointList();
	while (currJoint != nullptr)
	{
		b2Joint* joint = currJoint->joint;
		Box2D_UserData_Interface* userData = reinterpret_cast<Box2D_UserData_Interface*>(joint->GetUserData().pointer);
		if (void* jointUserData = reinterpret_cast<void*>(joint->GetUserData().pointer))
		{
			delete userData;
			joint->GetUserData().pointer = 0;
		}
		currJoint = currJoint->next;
	}

	//Finally clear up body user data and then destroy the body
	{
		Box2D_UserData_Interface* userData = reinterpret_cast<Box2D_UserData_Interface*>(body->GetUserData().pointer);
		if (userData)
		{
			delete userData;
			body->GetUserData().pointer = 0;
		}
		body->GetWorld()->DestroyBody(body);
	}
}
