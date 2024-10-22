//*********************************************************************************\\
// 
//	Primary FL operational class. Hosts sets and associated rules to represent a
//  complete logic model. Uses Sugeno-based FL methods.
//
//*********************************************************************************\\

#pragma once

#include "FL_Set.h"
#include "FL_Rules.h"

class FL_Model
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	FL_Model() {}
	~FL_Model() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Init
	//

	//Inits all model sections from file (See FL_Model_Config_Template.json)
	bool InitModelFromFile(std::string& fp, unsigned configIndex);

	//
	//Operation
	//

	//Run logic algoritm for all exist sets using given array of values (runs indexes in order of sets).
	//Returns the resolved state as an int value (with context given via enum elsewhere).
	//-1/-2 returns indicate undefined rule or no found rule states.
	int RunModelAlgorithm(const std::vector<float>& inputs);

	/////////////////
	/// Accessors ///
	/////////////////


private:

	//////////////////
	/// Operations ///
	//////////////////

	//Clear up
	void Release();

	////////////
	/// Data ///
	////////////

	//Model name (the totality of the subject)
	std::string m_Name = "N/A";
	//Each individual set in the model
	std::vector<FL_Set> m_Sets;
	//Associated ruleset
	FL_Rules m_Rules;
};