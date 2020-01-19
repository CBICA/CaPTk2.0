#include "CaPTkWriteJsonToFile.h"

#include <fstream>

void captk::WriteJsonToFile(Json::Value json, std::string outputfile)
{
	std::ofstream file;
	file.open(outputfile, std::ofstream::out | std::ofstream::trunc); // trunc deletes the contents
	file << json.toStyledString() << std::endl;
	file.close();
}