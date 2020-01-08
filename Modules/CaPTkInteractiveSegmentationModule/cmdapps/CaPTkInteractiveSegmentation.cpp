#include <mitkCommandLineParser.h>
#include <mitkIOUtil.h>
#include <mitkDataStorage.h>
#include <mitkStandaloneDataStorage.h>

#include <CaPTkInteractiveSegmentation.h>
#include <GeodesicTrainingSegmentation.h>

#include <json/json.h>

#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>

/** \brief command-line app for batch processing of images
 *
 * This command-line app takes a task and and a cohort and runs the algorithm on everything.
 */
int main(int argc, char* argv[])
{
  mitkCommandLineParser parser;

  // Set general information about the command-line app
  parser.setCategory("CaPTk Cmd App Category");
  parser.setTitle("CaPTk Interactive Segmentation Cmd App");
  parser.setContributor("CBICA");
  parser.setDescription(
    "This command-line app takes a task and cohort and runs the interactive segmentation algorithm on everything.");

  // How should arguments be prefixed
  parser.setArgumentPrefix("--", "-");

  // Add arguments. Unless specified otherwise, each argument is optional.
  // See mitkCommandLineParser::addArgument() for more information.
  parser.addArgument(
    "task",
    "t",
    mitkCommandLineParser::String,
    "Task",
    "JSON file that contains information on how to run this execution.",
    us::Any(),
    false);
  parser.addArgument(
    "cohort",
    "c",
    mitkCommandLineParser::String,
    "Cohort",
    "JSON file that contains information on how to run this execution.",
    us::Any(),
    false);

  // Parse arguments. This method returns a mapping of long argument names to
  // their values.
  auto parsedArgs = parser.parseArguments(argc, argv);

  if (parsedArgs.empty())
    return EXIT_FAILURE; // Just exit, usage information was already printed.

  if (parsedArgs["task"].Empty() || parsedArgs["cohort"].Empty())
  {
    MITK_INFO << parser.helpText();
    return EXIT_FAILURE;
  }

  // Parse, cast and set required arguments
  auto task   = us::any_cast<std::string>(parsedArgs["task"]);
  auto cohort = us::any_cast<std::string>(parsedArgs["cohort"]);

  // // Default values for optional arguments
  // auto verbose = false;
  // // Parse, cast and set optional arguments
  // if (parsedArgs.end() != parsedArgs.find("verbose"))
  //   verbose = us::any_cast<bool>(parsedArgs["verbose"]);

  try
  {
    auto algorithm = new CaPTkInteractiveSegmentation(nullptr);
    algorithm->Run(task, cohort);

    return EXIT_SUCCESS;
  }
  catch (const std::exception &e)
  {
    MITK_ERROR << e.what();
    return EXIT_FAILURE;
  }
  catch (...)
  {
    MITK_ERROR << "Unexpected error!";
    return EXIT_FAILURE;
  }
}
