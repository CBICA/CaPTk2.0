#include <mitkCommandLineParser.h>

#include <CaPTkCohortJsonFromFilesystem.h>
#include <CaPTkWriteJsonToFile.h>

//Delete this
#include <CaPTkROIConstruction.h>

#include <json/json.h>

#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>

/**Splits a string into a list using a delimiter*/
std::vector<std::string> 
split(const std::string &s, char delim) {
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(std::move(item));
	}
	return elems;
}

/** \brief command-line app for batch processing of images
 *
 * This command-line app takes a list of directories and merges them to create a cohort json file.
 */
int main(int argc, char* argv[])
{
  mitkCommandLineParser parser;

  /**** Set general information about the command-line app ****/

  parser.setCategory("CaPTk Cmd App Category");
  parser.setTitle("CaPTk Cohort Json From Filesystem Cmd App");
  parser.setContributor("CBICA");
  parser.setDescription(
    "This command-line app takes a list of directories and merges them to create a cohort json file.");

  // How should arguments be prefixed
  parser.setArgumentPrefix("--", "-");

  /**** Add arguments. Unless specified otherwise, each argument is optional.
            See mitkCommandLineParser::addArgument() for more information. ****/

  parser.addArgument(
    "directories",
    "d",
    mitkCommandLineParser::String,
    "Directories",
    "Paths to the input directories, separated by comma",
    us::Any(),
    false);

  parser.addArgument(
    "outputfile",
    "o",
    mitkCommandLineParser::String,
    "Output File",
    "Path to the desired output",
    us::Any(),
    false);

  /**** Parse arguments. This method returns a mapping of long argument names to
            their values. ****/

  auto parsedArgs = parser.parseArguments(argc, argv);

  if (parsedArgs.empty())
    return EXIT_FAILURE; // Just exit, usage information was already printed.

  if (parsedArgs["directories"].Empty() || parsedArgs["outputfile"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  // // Parse, cast and set required arguments
  // auto task   = us::any_cast<std::string>(parsedArgs["task"]);
  // auto cohort = us::any_cast<std::string>(parsedArgs["cohort"]);

  auto directories = us::any_cast<std::string>(parsedArgs["directories"]);
  auto outputfile  = us::any_cast<std::string>(parsedArgs["outputfile"]);

  std::vector<std::string> directoriesVector = split(directories, ',');

  /**** Run ****/

  try
  {
    auto json = captk::CohortJsonFromFilesystem(directoriesVector);
    captk::WriteJsonToFile(json, outputfile);
  }
  catch (...)
  {
    std::cerr << "Unexpected error!";
    return EXIT_FAILURE;
  }

  //Test ROI Construction (delete this)
  auto c = new captk::ROIConstruction();

  delete c;
}
