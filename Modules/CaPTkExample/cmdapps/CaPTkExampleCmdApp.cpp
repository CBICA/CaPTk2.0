/** \file CaPTkExampleCmdApp.cpp
 * \brief CLI program for showcasing the example
 * 
 * The example currently is an invert image filter
 */

#include <mitkCommandLineParser.h>
#include <mitkException.h>
#include <mitkLogMacros.h>
#include <mitkImage.h>
#include <mitkIOUtil.h>
#include <mitkImageAccessByItk.h>

#include "CaPTkExampleAlgorithm.h"


/** \brief command-line app for example module
 *
 * This command-line app takes an image and inverts it.
 */
int main(int argc, char* argv[])
{
  mitkCommandLineParser parser;

  /*---- Set general information about the command-line app ----*/

  parser.setCategory("CaPTk Cmd App Category");
  parser.setTitle("CaPTk Example Cmd App");
  parser.setContributor("CBICA");
  parser.setDescription(
    "This command-line app showcases the example module.");

  // How should arguments be prefixed
  parser.setArgumentPrefix("--", "-");

  /*---- Add arguments. Unless specified otherwise, each argument is optional.
            See mitkCommandLineParser::addArgument() for more information. ----*/

  parser.addArgument(
    "input",
    "i",
    mitkCommandLineParser::String,
    "Input image path",
    "Path to the input image to be inverted",
    us::Any(),
    false); // false -> required parameter

  parser.addArgument(
    "output",
    "o",
    mitkCommandLineParser::String,
    "Output",
    "Output image target path",
    us::Any(),
    false); // false -> required parameter

  parser.addArgument(
    "maximum",
    "m",
    mitkCommandLineParser::Int,
    "Maximum",
    "Maximum value for the inverse intensity filter. Default is 100.",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "randomflag",
    "rf",
    mitkCommandLineParser::Bool,
    "Random flag",
    "Useless flag that does nothing.",
    us::Any(),
    true); // false -> required parameter

  /*---- Parse arguments. This method returns a mapping of long argument names to
            their values. ----*/

  auto parsedArgs = parser.parseArguments(argc, argv);

  if (parsedArgs.empty())
  {
    return EXIT_FAILURE; // Just exit, usage information was already printed.
  }

  /*---- Required parameters ----*/

  if (parsedArgs["input"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  if (parsedArgs["output"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  auto inputPath  = us::any_cast<std::string>(parsedArgs["input"]);
  auto outputPath = us::any_cast<std::string>(parsedArgs["output"]);

  /*---- Optional parameters ----*/

  int  maximum    = 100;
  bool randomFlag = false;

  if (parsedArgs.end() != parsedArgs.find("maximum"))
  {
    maximum = us::any_cast<int>(parsedArgs["maximum"]);
  }

  if (parsedArgs.end() != parsedArgs.find("randomflag"))
  {
    randomFlag = true;
  }

  if (randomFlag) { MITK_INFO << "Random flag was true"; }

  /*---- Run ----*/

  try
  {
    // Read input
    mitk::Image::Pointer input = mitk::IOUtil::Load<mitk::Image>(inputPath);

    // Call inverter (notice the parenthesis in the extra parameters)
    mitk::Image::Pointer output = mitk::Image::New();
    AccessByItk_n(input, captk::ExampleAlgorithm::Run, (maximum, output));

    // Save result
    mitk::IOUtil::Save(output, outputPath);
  }
  catch (const mitk::Exception& e)
  {
    MITK_ERROR << "MITK Exception: " << e.what();
  }
  catch (...)
  {
    std::cerr << "Unexpected error!\n";
    return EXIT_FAILURE;
  }
}
