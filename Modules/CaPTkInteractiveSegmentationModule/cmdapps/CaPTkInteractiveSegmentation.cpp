#include <mitkCommandLineParser.h>
#include <mitkIOUtil.h>

#include <CaPTkInteractiveSegmentation.h>
#include <GeodesicTrainingSegmentationSegmentation.h>

#include <algorithm>
#include <string>

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

  // // Parse, cast and set required arguments
  // auto inFilename = us::any_cast<std::string>(parsedArgs["input"]);
  // auto outFilename = us::any_cast<std::string>(parsedArgs["output"]);
  // auto offset = us::any_cast<int>(parsedArgs["offset"]);

  // // Default values for optional arguments
  // auto verbose = false;

  // // Parse, cast and set optional arguments
  // if (parsedArgs.end() != parsedArgs.find("verbose"))
  //   verbose = us::any_cast<bool>(parsedArgs["verbose"]);

  // try
  // {
  //   if (verbose)
  //     MITK_INFO << "Read input file";

  //   auto inImage = mitk::IOUtil::Load<mitk::Image>(inFilename);

  //   if (inImage.IsNull())
  //   {
  //     MITK_ERROR << "Could not read \"" << inFilename << "\"!";
  //     return EXIT_FAILURE;
  //   }

  //   if (verbose)
  //     MITK_INFO << "Add offset to image";

    // auto exampleFilter = ExampleImageFilter::New();
    // exampleFilter->SetInput(inImage);
    // exampleFilter->SetOffset(offset);
    // exampleFilter->Update();

    // auto outImage = exampleFilter->GetOutput();

    // if (nullptr == outImage)
    // {
    //   MITK_ERROR << "Image processing failed!";
    //   return EXIT_FAILURE;
    // }

    // if (verbose)
    //   MITK_INFO << "Write output file";

    // mitk::IOUtil::Save(outImage, outFilename);

    return EXIT_SUCCESS;
  // }
  // catch (const std::exception &e)
  // {
  //   MITK_ERROR << e.what();
  //   return EXIT_FAILURE;
  // }
  // catch (...)
  // {
  //   MITK_ERROR << "Unexpected error!";
  //   return EXIT_FAILURE;
  // }
}
