//*********************************************************************************\\
// 
// RapidJSON loading utilities.
//
//*********************************************************************************\\

#pragma once

#include <string>
#include "document.h"

#include <fstream>
#include <sstream>

//Loads and parses JSON file into document object
static inline void ParseNewJSONDocument(rapidjson::Document& doc, std::string& filePath)
{
	//Open a stringstream and load file into it
	std::stringstream jsonDocBuffer;
	std::string inputLine;
	std::ifstream inputStream(filePath);

	while (std::getline(inputStream, inputLine))
		jsonDocBuffer << inputLine << "\n";

	//Close, parse and confirm object status
	inputStream.close();
	doc.Parse(jsonDocBuffer.str().c_str());
	assert(doc.IsObject());
}