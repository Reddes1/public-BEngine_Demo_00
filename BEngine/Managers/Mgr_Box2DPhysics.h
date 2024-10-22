//*********************************************************************************\\
// 
// Wrapper class for Box2D Physics functionality (Heavy WIP as still learning 
// how to use it and how to best configure the class).
//
//*********************************************************************************\\

#pragma once

//Library Includes
#include <unordered_map>
//Engine Includes
#include "Types/Box2D_WorldData.h"


//Default conversion rate for converting Box2D metrics (metres?) to screen/pixel rate (1 meter = 50 pixels)
static const float g_BOX2D_CONVERSION_RATE = 50.f;

class Mgr_Box2DPhysics
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Map and pair typedefs
	typedef std::pair<std::string, std::shared_ptr<Box2D_WorldData>> WorldPair;
	typedef std::unordered_map<std::string, std::shared_ptr<Box2D_WorldData>> WorldMap;
	//To-do: Add maps and accessors for significant types (b2body, fixtures, joints etc)


	////////////////////
	/// Constructors ///
	////////////////////

	Mgr_Box2DPhysics() { }
	~Mgr_Box2DPhysics() { m_Worlds.clear(); }

	//////////////////
	/// Operations ///
	//////////////////
	
	//
	//Setups
	//

	//Creates new world data, consisting of world and listener type
	void CreateNewWorldData(std::string& worldName, b2Vec2& initGravity, int velIters = 6, int posIters = 2);

	//
	//Conversions
	//

	//Converts Box2D metres to pixels using default conversion rate
	static inline float MetresToPixels(float metresIn) { return metresIn * g_BOX2D_CONVERSION_RATE; }
	static inline b2Vec2 MetresToPixels(const b2Vec2& metresIn) { return b2Vec2((metresIn.x * g_BOX2D_CONVERSION_RATE), (metresIn.y * g_BOX2D_CONVERSION_RATE)); }
	//Converts pixels to Box2D metres using default conversion rate
	static inline float PixelsToMetres(float pixelsIn) { return pixelsIn / g_BOX2D_CONVERSION_RATE; }
	static inline b2Vec2 PixelsToMetres(const b2Vec2& pixelsIn) { return b2Vec2((pixelsIn.x / g_BOX2D_CONVERSION_RATE), (pixelsIn.y / g_BOX2D_CONVERSION_RATE)); }

	/////////////////
	/// Accessors ///
	/////////////////

	Box2D_WorldData* GetWorldData(const std::string key);

private:

	//////////////////
	/// Operations ///
	//////////////////

	////////////
	/// Data ///
	////////////

	//All current worlds (accessed via unique name)
	WorldMap m_Worlds;

};