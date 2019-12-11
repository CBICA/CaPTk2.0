#ifndef H_CBICA_UTIL_GTS
#define H_CBICA_UTIL_GTS

// #pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#include <ctime>
#include <chrono>
#include <iostream>
#include <vector>

namespace GeodesicTrainingSegmentation 
{
	namespace UtilGTS
	{
		/**
		Get current date and time
		@return current date and time as %Y-%m-%d %H.%M.%S
		*/
		std::string currentDateTime();

		/** Find if a directory exists
		@param dName the name of the directory
		@return bool if the directory exists
		*/
		bool directoryExists(const std::string &dName);

		/**Creates a directory*/
		bool createDir(const std::string &dName);

		/**Splits a string into a list using a delimiter*/
		std::vector<std::string> 
		split_string(const std::string &s, char delim);

		/**Find the file extension of the file*/
		std::string getFileExtension(const std::string filePath);
	}

}

#endif