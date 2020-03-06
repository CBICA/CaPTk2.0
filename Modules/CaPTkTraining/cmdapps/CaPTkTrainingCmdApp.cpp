/** \file CaPTkTrainingCmdApp.cpp
 * \brief CLI program for the training module
 */

#include <mitkCommandLineParser.h>
#include <mitkException.h>
#include <mitkLogMacros.h>
#include <mitkImage.h>
#include <mitkIOUtil.h>
#include <mitkImageAccessByItk.h>

#include "CaPTkTraining.h"


/** \brief command-line app for training module
 *
 * This command-line app takes csv fails and performs ML operations.
 */
int main(int argc, char* argv[])
{
  mitkCommandLineParser parser;

  /*---- Set general information about the command-line app ----*/

  parser.setCategory("CaPTk Cmd App Category");
  parser.setTitle("CaPTk Training Cmd App");
  parser.setContributor("CBICA");
  parser.setDescription(
    "This command-line app showcases the example module.");

  // How should arguments be prefixed
  parser.setArgumentPrefix("--", "-");

  /*---- Add arguments. Unless specified otherwise, each argument is optional.
            See mitkCommandLineParser::addArgument() for more information. ----*/

  parser.addArgument(
    "configuration",
    "c",
    mitkCommandLineParser::Bool,
    "Configuration",
    "Configuration (\"crossvalidation\" or \"splittraintest\" or \"train\" or \"test\"",
    us::Any(),
    false); // false -> required parameter

  parser.addArgument(
    "inputfeatures",
    "if",
    mitkCommandLineParser::String,
    "Input features (samples) file",
    "Path to the input CSV file",
    us::Any(),
    false); // false -> required parameter

  parser.addArgument(
    "inputresponses",
    "ir",
    mitkCommandLineParser::String,
    "Input Responses",
    "1-column CSV with the responses for each sample",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "kernel",
    "k",
    mitkCommandLineParser::String,
    "Kernel",
    "Classification kernel (svmlinear or svmrbf)",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "folds",
    "f",
    mitkCommandLineParser::String,
    "Folds",
    "Folds (for crossvalidation configuration)",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "samples",
    "s",
    mitkCommandLineParser::String,
    "Samples",
    "Samples (for splittraintest configuration)",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "modeldir",
    "md",
    mitkCommandLineParser::String,
    "Model directory",
    "Path to model directory",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "outputdir",
    "o",
    mitkCommandLineParser::String,
    "Output directory",
    "Path to output directory",
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

  if (parsedArgs["configuration"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  if (parsedArgs["inputfeatures"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  auto configuration     = us::any_cast<std::string>(parsedArgs["configuration"]);
  auto inputfeatures     = us::any_cast<std::string>(parsedArgs["inputfeatures"]);

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