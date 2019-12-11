#include "UtilGTS.h"

#include <algorithm>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include<sstream>

// Some of the headers below are not needed
#if (_WIN32)
#define NOMINMAX
#include <direct.h>
#include <windows.h>
#include <conio.h>
#include <lmcons.h>
#include <Shlobj.h>
#include <filesystem>
#else
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <errno.h>
#include <ftw.h>
#endif

std::string GeodesicTrainingSegmentation::UtilGTS::currentDateTime()
{
	// Get current time
	std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();

	// Convert to time_t for <ctime>
	std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);

	// Format to datetime
	std::tm now_tm = *std::localtime(&time_now_t);
	char buf[512];
	std::strftime(buf, 512, "%Y-%m-%d %H.%M.%S", &now_tm);
	std::string dateTime(buf);

	return dateTime;
}

bool GeodesicTrainingSegmentation::UtilGTS::directoryExists(const std::string &dName)
{
	struct stat info;
	std::string dName_Wrap = dName;

	if (dName_Wrap[dName_Wrap.length() - 1] == '/')
	{
		dName_Wrap.erase(dName_Wrap.end() - 1);
	}

	if (stat(dName_Wrap.c_str(), &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)  // S_ISDIR() doesn't exist on windows
		return true;
	else
		return false;
}

bool GeodesicTrainingSegmentation::UtilGTS::createDir(const std::string &dName)
{
	//! Pure c++ based directory creation
#if defined(_WIN32)
	DWORD ftyp = GetFileAttributesA(dName.c_str()); // check if directory exists or not
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		_mkdir(dName.c_str());
    return true;
#else
    DIR *pDir;
    pDir = opendir(dName.c_str()); // check if directory exists or not
    if (pDir == NULL)
		mkdir(dName.c_str(), 0777);
    return true;
#endif
	return false;
}

/**Splits a string into a list using a delimiter*/
std::vector<std::string> 
GeodesicTrainingSegmentation::UtilGTS::split_string(const std::string &s, char delim) 
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(std::move(item));
	}
	return elems;
}

std::string GeodesicTrainingSegmentation::UtilGTS::getFileExtension(std::string filePath)
{
	std::replace( filePath.begin(), filePath.end(), '\\', '/'); // replace all '\\' to '/'
	auto fSplitVec = split_string(filePath, '/'); // Split based on '/'
	auto fLastComp = fSplitVec[fSplitVec.size()-1]; // The last component (aka filename)

	auto fSplitDotsVec = split_string(fLastComp, '.'); // Split filename based on '.'

	// We basically keep the last component as file extension, except for nii.gz
	if (fSplitDotsVec.size() > 2 && fSplitDotsVec[fSplitDotsVec.size()-2] == "nii" &&
		fSplitDotsVec[fSplitDotsVec.size()-1] == "gz")
	{
		return ".nii.gz"; // Special case
	}
	else if (fSplitDotsVec.size() == 1)
	{
		return "";
	}
	else {
		return fSplitDotsVec[fSplitDotsVec.size()-1]; // Last component
	}
}