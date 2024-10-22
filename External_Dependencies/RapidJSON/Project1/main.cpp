#include<iostream>
#include<algorithm>
#include<functional>
#include<numeric>
#include<list>

#include<chrono>
#include<vector>
#include<random>
#include<string>
#include<fstream>
#include<sstream>

#include "../../rapidjson-master/include/rapidjson/document.h"


using namespace rapidjson;

std::mt19937 random_number_engine(time(0));

int RandomNumberGenerator(int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(random_number_engine);
}


int GetRand()
{
	return RandomNumberGenerator(INT_MIN, INT_MAX);
}

struct V2
{
	int x, y;
};

struct V3
{
	int x, y, z;
};

struct V4
{
	int x, y, w, h;
};

struct Animation
{
	std::string animName;
	V3 animation;
};

struct Texture
{

};

struct Texture2D
{
	Texture tex;
	std::vector<Animation> animations;
	std::vector<V4> frameIndexes;
};

namespace RapidJSONLoaders
{
	void SpriteArrayValidityCheck(const Value& arr)
	{
		//Check for valid members
		assert(arr[0].HasMember("filename"));
		assert(arr[0].HasMember("frame"));
		assert(arr[0].HasMember("rotated"));
		assert(arr[0].HasMember("trimmed"));
		assert(arr[0].HasMember("spriteSourceSize"));
		assert(arr[0].HasMember("sourceSize"));

		//Check types of members
		assert(arr[0]["filename"].IsString());
		assert(arr[0]["frame"].IsObject());
		assert(arr[0]["rotated"].IsBool());
		assert(arr[0]["trimmed"].IsBool());
		assert(arr[0]["spriteSourceSize"].IsObject());
		assert(arr[0]["sourceSize"].IsObject());

		//Could check subtypes here, but gonna assume its good (Luke roast me if im being lazy)
		// :/ :/ :/ :/
	}
	void AnimArrayValidityCheck(const Value& arr)
	{
		//Check for valid members
		assert(arr[0].HasMember("animname"));
		assert(arr[0].HasMember("animstart"));
		assert(arr[0].HasMember("animend"));
		assert(arr[0].HasMember("animspeed"));

		//Check types of members
		assert(arr[0]["animname"].IsString());
		assert(arr[0]["animstart"].IsInt());
		assert(arr[0]["animend"].IsInt());
		assert(arr[0]["animspeed"].IsInt());
	}

	void LoadSpriteFramesArray(std::vector<V4>& frameDataContainer, std::string framesFP)
	{
		std::stringstream jsonDocBuffer;
		std::string inputLine;
	
		//Read in frame from file
		std::ifstream inputStream(framesFP);
		//Move file data into stringstream
		while (std::getline(inputStream, inputLine))
		{
			jsonDocBuffer << inputLine << "\n";
		}
		//Create rapidjson vars
		Document doc;
		//Parse the string buffer
		doc.Parse(jsonDocBuffer.str().c_str());
		assert(doc.IsObject());
		assert(doc.HasMember("frames"));
		//Get a reference to the main array
		const Value& f = doc["frames"];
		
		//Run a member/type check on the array members
		SpriteArrayValidityCheck(f);

		//Parse the data into arrays (Only doing Sprite frames from this)
		for (auto& a : f.GetArray())
		{
			V4 frm;
			frm.x = a["frame"]["x"].GetInt();
			frm.y = a["frame"]["y"].GetInt();
			frm.w = a["frame"]["w"].GetInt();
			frm.h = a["frame"]["h"].GetInt();

			frameDataContainer.push_back(frm);
		}

		//Close stream
		inputStream.close();
	}

	void LoadAnimationArray(std::vector<Animation>& animDataContainer, std::string animFP)
	{
		std::stringstream jsonDocBuffer;
		std::string inputLine;

		//Read in frame from file
		std::ifstream inputStream(animFP);
		//Move file data into stringstream
		while (std::getline(inputStream, inputLine))
		{
			jsonDocBuffer << inputLine << "\n";
		}
		//Create rapidjson vars
		Document doc;
		//Parse the string buffer
		doc.Parse(jsonDocBuffer.str().c_str());
		assert(doc.IsObject());
		assert(doc.HasMember("animations"));
		//Get a reference to the main array
		const Value& a = doc["animations"];

		AnimArrayValidityCheck(a);

		//Parse the data into arrays (Only doing Sprite frames from this)
		for (auto& a : a.GetArray())
		{
			Animation anim;
			anim.animName = a["animname"].GetString();
			anim.animation.x = a["animstart"].GetInt();
			anim.animation.y = a["animend"].GetInt();
			anim.animation.z = a["animspeed"].GetInt();

			animDataContainer.push_back(anim);
		}

		//Close stream
		inputStream.close();
	}



}

int main()
{
	//Sample testing
	Texture2D tex;
	//filepaths
	std::string framesFP = "../bin/spritesheet.json";
	std::string animFP = "../bin/animsheet.json";			//PH

	RapidJSONLoaders::LoadSpriteFramesArray(tex.frameIndexes, framesFP);
	RapidJSONLoaders::LoadAnimationArray(tex.animations, animFP);


	system("pause");
	return 0;
}