/** \file CaPTkTrainingCmdApp.cpp
 * \brief CLI program for the training module
 */

#include <mitkCommandLineParser.h>
#include <mitkException.h>
#include <mitkLogMacros.h>
#include <mitkImage.h>
#include <mitkIOUtil.h>
#include <mitkImageAccessByItk.h>

#include <QString>

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
    mitkCommandLineParser::String,
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
    mitkCommandLineParser::Int,
    "Folds",
    "Folds (for crossvalidation configuration)",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "samples",
    "s",
    mitkCommandLineParser::Int,
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

  QString configuration = us::any_cast<std::string>(parsedArgs["configuration"]).c_str();

  if (QString::compare(configuration, "train", Qt::CaseInsensitive) == 0)
  {
    configuration = "Train";
  }
  else if (QString::compare(configuration, "test", Qt::CaseInsensitive) == 0)
  {
    configuration = "Test";
  }
  else if (QString::compare(configuration, "splittraintest", Qt::CaseInsensitive) == 0)
  {
    configuration = "Split Train/Test";
  }
  else if (QString::compare(configuration, "crossvalidation", Qt::CaseInsensitive) == 0)
  {
    configuration = "Cross-validation";
  }
  else
  {
    MITK_ERROR << "Unknown configuration";
    return EXIT_FAILURE; 
  }

  QString inputfeatures = us::any_cast<std::string>(parsedArgs["inputfeatures"]).c_str();

  /*---- Optional parameters ----*/

  QString inputresponses = "";
  QString classificationKernelStr = "svmlinear";
  int folds = 0;
  int samples = 0;
  QString modeldir = "";
  QString outputdir = "";

  if (parsedArgs.end() != parsedArgs.find("inputresponses"))
  {
    inputresponses = us::any_cast<std::string>(parsedArgs["inputresponses"]).c_str();
  }

  if (parsedArgs.end() != parsedArgs.find("kernel"))
  {
    classificationKernelStr = us::any_cast<std::string>(parsedArgs["kernel"]).c_str();
    if (QString::compare(classificationKernelStr, "svmlinear", Qt::CaseInsensitive) == 0)
    {
      classificationKernelStr = "SVM: Linear";
    }
    else if (QString::compare(classificationKernelStr, "svmrbf", Qt::CaseInsensitive) == 0)
    {
      classificationKernelStr = "SVM: RBF";
    }
    else
    {
      MITK_ERROR << "Unknown kernel";
      return EXIT_FAILURE;
    }
    
  }

  if (parsedArgs.end() != parsedArgs.find("folds"))
  {
    folds = us::any_cast<int>(parsedArgs["folds"]);
  }

  if (parsedArgs.end() != parsedArgs.find("samples"))
  {
    samples = us::any_cast<int>(parsedArgs["samples"]);
  }

  if (parsedArgs.end() != parsedArgs.find("modeldir"))
  {
    modeldir = us::any_cast<std::string>(parsedArgs["modeldir"]).c_str();
  }

  if (parsedArgs.end() != parsedArgs.find("outputdir"))
  {
    outputdir = us::any_cast<std::string>(parsedArgs["outputdir"]).c_str();
  }

  /*---- Run ----*/

  try
  {
    captk::Training training;
    auto result = training.RunThread(
      inputfeatures,
      inputresponses,
      classificationKernelStr,
      configuration,
      folds,
      samples,
      modeldir,
      outputdir
    );

    if (result.ok)
    {
      MITK_INFO << "Finished successfully.";
    }
    else
    {
      MITK_ERROR << result.errorMessage;
      return EXIT_FAILURE;
    }
  }
  catch (const mitk::Exception& e)
  {
    MITK_ERROR << "MITK Exception: " << e.what();
    return EXIT_FAILURE;
  }
  catch (const std::exception& e)
  {
    MITK_ERROR << "Exception: " << e.what();
    return EXIT_FAILURE;
  }
  catch (...)
  {
    std::cerr << "Unexpected error!\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}