#include "FL_Model.h"

#include "Utils/Utils_Debug.h"
#include "Utils/Utils_RapidJSON.h"

bool FL_Model::InitModelFromFile(std::string& fp, unsigned configIndex)
{
	//Clear and release any existing data
	Release();

	//Load doc
	rapidjson::Document doc;
	ParseNewJSONDocument(doc, fp);

	//Grab configs array
	const rapidjson::Value& arr = doc["FL Configs"].GetArray();
	msg_assert(arr.IsArray(), "InitModelFromFile(): Array not found!");
	msg_assert(configIndex <= arr.Size(), "InitModelFromFile: Index OOR!");

	//Get model info
	m_Name = arr[configIndex]["Model_Name"].GetString();
	unsigned setCount = arr[configIndex]["Set_Count"].GetUint();
	unsigned maxKeyDepth = arr[configIndex]["Max_Key_Depth"].GetUint();

	//
	//Sets
	//

	//Setup sets container
	m_Sets.reserve(setCount);
	m_Sets.shrink_to_fit();
	//Add new sets
	m_Sets.insert(m_Sets.begin(), setCount, FL_Set());

	//Setup individual sets
	for (unsigned i(0); i < m_Sets.size(); ++i)
	{
		//Get set name
		m_Sets[i].SetName(std::string(arr[configIndex]["Sets"][i]["Set_Name"].GetString()));

		//Get members array for brevity
		const rapidjson::Value& mArr = arr[configIndex]["Sets"][i]["Members"].GetArray();

		//Cycle through each member and grab its data for setup
		for (unsigned j(0); j < mArr.Size(); ++j)
		{
			//Holding container for each member anchoring point
			std::vector<float> points;
			//Cycle and store member data
			for (unsigned k(0); k < mArr[j]["Points"].Size(); ++k)
				points.push_back(mArr[j]["Points"][k].GetFloat());

			//Setup a new member for the set
			m_Sets[i].AddNewMember(
				std::string(mArr[j]["Name"].GetString()),
				(FL_Member_Interface::MemberTypes)mArr[j]["Shape_ID"].GetUint(), 
				points
			);

			//Set ID
			m_Sets[i].SetSetID(i);
		}
	}

	//
	//Rules
	//

	//Get ruleset name
	m_Rules.SetRulesetName(std::string(arr[configIndex]["Ruleset_Name"].GetString()));

	//Compile and set state names
	std::vector<std::string> stateNames;
	for (unsigned i(0); i < arr[configIndex]["State_Names"].Size(); ++i)
		stateNames.push_back(arr[configIndex]["State_Names"][i].GetString());
	stateNames.shrink_to_fit();
	m_Rules.InitStateNames(stateNames);

	//Setup each key and rule combo
	for (unsigned i(0); i < arr[configIndex]["Keys_&_Rules"].Size(); ++i)
	{
		//Keys are strings, so hold one here for concatenating one together as part of a pair
		std::pair<std::string, int> pair;
		pair.first = "";
		//Get output state
		pair.second = arr[configIndex]["Keys_&_Rules"][i]["Output"].GetInt();

		//Cycle the keys container and build key
		for (unsigned j(0); j < arr[configIndex]["Keys_&_Rules"][i]["Keys"].Size(); ++j)
			pair.first += std::to_string(arr[configIndex]["Keys_&_Rules"][i]["Keys"][j].GetUint());

		//Finally, insert the new rule
		m_Rules.InitIndividualRule(pair);
	}

	//Job done
	return true;
}

int FL_Model::RunModelAlgorithm(const std::vector<float>& inputs)
{
	//Pre-check inputs container
	msg_assert(inputs.size() >= m_Sets.size(), "RunModelAlgorithm(): Not enough inputs!");

	//As the algorithm is run, we form a key here and then pass it into the rule map to (hopefully) find a matching state
	std::string key;
	for (unsigned i(0); i < m_Sets.size(); ++i)
	{
		//Input the input value and calculate
		m_Sets[i].CalcMembership(inputs[i]);
		//Get and append value to key
		key += std::to_string(m_Sets[i].GetHighestMember()->GetMemberID());

		//TO-DO: Sub-search for partial rules that only care a certain amount of the outputs (e.g. 1-- or 12- over 012)
	}

	return m_Rules.UpdateToNewState(key);
}

void FL_Model::Release()
{
	//Clear existing sets and current rules table
	m_Sets.clear();
	m_Rules.Reset();
}
