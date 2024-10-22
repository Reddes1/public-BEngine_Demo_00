//*********************************************************************************\\
// 
// Collection of simple, functional Contact Listeners used with Box2D Physics engine
// to provide additional resolution and reaction options to detected simulation 
// events.
//
//*********************************************************************************\\

#pragma once
//Library Includes
#include <unordered_set>
//Engine Includes
#include "Types/Box2D_Listener_Interfaces.h"


//
//Foward Declarations
//
class Actor2D_Interface;

/*
	Provides following:
	- BeginContact(): Seeks Actor2D_Interface pointer from b2Body UserData for each side of the event and
	  stores them as a pair in event container (for continuous collision checking). Additionally calls
	  OnCollisionEnter on each actor. See RunContinuedCollisions() for persistent collision events.
	- EndContact(): Seeks same data from b2Body UserData and then removes the pair from event container.
	  Additionally calls OnCollisionExit on each actor.
	- PreSolve(): No Implementation.
	- PostSolve(): No Implementation.
*/
class ContactListener_BE_Default : public Box2D_ContactListener_Interface
{
public:

	////////////////////
	/// Constructors ///
	////////////////////


	//////////////////
	/// Overrrides ///
	//////////////////

	//Called on fixtures making contact
	void BeginContact(b2Contact* contact) override;
	//Called on fixtures ending contact
	void EndContact(b2Contact* contact) override;

	//Called just before contact solver is called (resolving the collision), allowing for adjustments before solving
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override { }
	//Called after the solver is finished
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override { }

	//////////////////
	/// Operations ///
	//////////////////
	
	//Cycles current continuous events and calls OnContinuedCollision for each actor
	void RunContinuedCollisions();

private:

	////////////
	/// Data ///
	////////////

	//Pairing struct for use with set container (Note: May need moving)
	struct HashFunc
	{
		size_t operator()(const std::pair<Actor2D_Interface*, Actor2D_Interface*>& p) const
		{
			//Generate hash values based on pointer addresses
			size_t h0 = std::hash<Actor2D_Interface*>{}(p.first);
			size_t h1 = std::hash<Actor2D_Interface*>{}(p.first);
			//Create and return final hash value (using supposed best hash methods via const magic value + shifts)
			return h0 ^ (h1 + 0x9e3779b9 + (h0 << 6) + (h0 >> 2));
		}
	};

	//Container for holding currently on-going, continuous collision events between actors
	std::unordered_set<std::pair<Actor2D_Interface*, Actor2D_Interface*>, HashFunc> m_ContinuedCollisions;
};
