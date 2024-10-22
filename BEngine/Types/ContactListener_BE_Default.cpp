#include "ContactListener_BE_Default.h"

//Library Includes
#include <algorithm>	//std::minmax

//Engine Includes
#include "Actors/Actor2D_Interface.h"
#include "Types/Box2D_UserData_Interface.h"

void ContactListener_BE_Default::BeginContact(b2Contact* contact)
{
	/*
		Through the fixture, access body and grab its actor pointer from the user data.
		This is typically set during rigidbody module setup.
	*/
	//Access userdata via fixture connection to body
	Box2D_UserData_Interface* userDataA = reinterpret_cast<Box2D_UserData_Interface*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	Box2D_UserData_Interface* userDataB = reinterpret_cast<Box2D_UserData_Interface*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	if (!userDataA || !userDataB)
		return;

	Actor2D_Interface* actorA = userDataA->m_AssociatedActor;
	Actor2D_Interface* actorB = userDataB->m_AssociatedActor;

	if (!actorA || !actorB)
		return;

	//Use actor callback for collision start for each of them
	actorA->OnCollisionEnter(actorB, contact->GetFixtureB(), contact);
	actorB->OnCollisionEnter(actorA, contact->GetFixtureA(), contact);

	//As this listener supports continuous collision, add this pair to the holding container
	auto sPair = std::minmax(actorA, actorB);
	m_ContinuedCollisions.insert(sPair);
}

void ContactListener_BE_Default::EndContact(b2Contact* contact)
{
	/*
		Through the fixture, access body and grab its actor pointer from the user data.
		This is typically set during rigidbody module setup.
	*/

	//Access userdata via fixture connection to body
	Box2D_UserData_Interface* userDataA = reinterpret_cast<Box2D_UserData_Interface*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
	Box2D_UserData_Interface* userDataB = reinterpret_cast<Box2D_UserData_Interface*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

	if (!userDataA || !userDataB)
		return;

	Actor2D_Interface* actorA = userDataA->m_AssociatedActor;
	Actor2D_Interface* actorB = userDataB->m_AssociatedActor;

	if (!actorA || !actorB)
		return;

	//Use actor callback for collision ending for each of them
	actorA->OnCollisonExit(actorB, contact->GetFixtureB(), contact);
	actorB->OnCollisonExit(actorA, contact->GetFixtureA(), contact);

	//As this listern support continuous collision, remove this pair from the holding container
	auto sPair = std::minmax(actorA, actorB);
	m_ContinuedCollisions.erase(sPair);
}

void ContactListener_BE_Default::RunContinuedCollisions()
{
	//Run each pair and call the target collision function from each
	for (auto& a : m_ContinuedCollisions)
	{
		//Run first against second, then vice versa
		a.first->OnContinuedCollision(a.second);
		a.second->OnContinuedCollision(a.first);
	}		
}