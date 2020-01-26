#include <mitkCommandLineParser.h>

#include <CaPTkROIConstruction.h>

#include "mitkException.h"
#include "mitkLogMacros.h"
#include <mitkIOUtil.h>

#include <QString>
#include <QDir>
#include <QFileInfo>

#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>

/** \brief command-line app for creating lattice ROIs from a mask
 *
 * This command-line app takes a mask and creates multiple lattice ROIs
 */
int main(int argc, char* argv[])
{
  mitkCommandLineParser parser;

  /*---- Set general information about the command-line app ----*/

  parser.setCategory("CaPTk Cmd App Category");
  parser.setTitle("CaPTk ROI Construction Cmd App");
  parser.setContributor("CBICA");
  parser.setDescription(
    "This command-line app supports operations for lattice ROI construction for feature extraction.");

  // How should arguments be prefixed
  parser.setArgumentPrefix("--", "-");

  /*---- Add arguments. Unless specified otherwise, each argument is optional.
            See mitkCommandLineParser::addArgument() for more information. ----*/

  parser.addArgument(
    "mask",
    "m",
    mitkCommandLineParser::String,
    "Mask",
    "Input Mask Image",
    us::Any(),
    false); // false -> required parameter

  parser.addArgument(
    "lattice",
    "l",
    mitkCommandLineParser::Bool,
    "Lattice",
    "Enables lattice calculation",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "window",
    "w",
    mitkCommandLineParser::Float,
    "Window",
    "Lattice window",
    us::Any(),
    false); // false -> required parameter

  parser.addArgument(
    "flux-neumann-condition",
    "f",
    mitkCommandLineParser::Bool,
    "flux-neumann-condition",
    "Enables flux-neumann-condition",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "patch-construction-roi",
    "pr",
    mitkCommandLineParser::Bool,
    "patch-construction-roi",
    "Enables patch-construction-roi",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "patch-construction-none",
    "pn",
    mitkCommandLineParser::Bool,
    "patch-construction-none",
    "Enables patch-construction-none",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "step",
    "s",
    mitkCommandLineParser::Float,
    "step",
    "Lattice step",
    us::Any(),
    false); // false -> required parameter

  parser.addArgument(
    "output-dir",
    "o",
    mitkCommandLineParser::String,
    "Output Directory",
    "Path to the desired output directory",
    us::Any(),
    false); // false -> required parameter

  /*---- Parse arguments. This method returns a mapping of long argument names to
            their values. ----*/

  auto parsedArgs = parser.parseArguments(argc, argv);

  if (parsedArgs.empty())
  {
    return EXIT_FAILURE; // Just exit, usage information was already printed.
  }

  /*---- Required parameters ----*/

  if (parsedArgs["mask"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  if (parsedArgs["window"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  if (parsedArgs["step"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  if (parsedArgs["output-dir"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  auto maskPath = QString(us::any_cast<std::string>(parsedArgs["mask"]).c_str());
  auto outputDir = QString(us::any_cast<std::string>(parsedArgs["output-dir"]).c_str());
  float window = us::any_cast<float>(parsedArgs["window"]);
  float step = us::any_cast<float>(parsedArgs["step"]);

  bool  lattice(false);
  bool  fluxNeumannCondition(false);
  bool  patchConstructionROI(false);
  bool  patchConstructionNone(false);

  if (parsedArgs.count("lattice"))
  {
      lattice = us::any_cast<bool>(parsedArgs["lattice"]);
  }
  if (parsedArgs.count("flux-neumann-condition"))
  {
      fluxNeumannCondition = us::any_cast<bool>(parsedArgs["flux-neumann-condition"]);
  }
  if (parsedArgs.count("patch-construction-roi"))
  {
      patchConstructionROI = us::any_cast<bool>(parsedArgs["patch-construction-roi"]);
  }
  if (parsedArgs.count("patch-construction-none"))
  {
      patchConstructionNone = us::any_cast<bool>(parsedArgs["patch-construction-none"]);
  }

  if (!lattice)
  {
      step = window = 0;
  }

  /*---- Run ----*/

  auto maskFileName = QFileInfo(maskPath).fileName();

  try
  {
      captk::ROIConstruction constructor;
      constructor.SetInputMask(
          mitk::IOUtil::Load<mitk::LabelSetImage>(maskPath.toStdString())
      );
      constructor.Update(
        lattice,
        window,
        fluxNeumannCondition,
        patchConstructionROI,
        patchConstructionNone,
        step
      );

      while(constructor.HasNext())
      {
          auto miniMask = constructor.GetNext();

          auto name = constructor.GetCurrentName().c_str();
          auto value = QString::number(constructor.GetCurrentValue());

          auto outputPath = outputDir + QDir::separator() 
            + name + "_" + value + "_" + 
            maskFileName;

          std::cout << std::endl
                    << ">Will save ROI to file "
                    << outputPath.toStdString().c_str();

          mitk::IOUtil::Save(
		    miniMask, 
		    outputPath.toStdString()
	      );
      }
  }
  catch (const mitk::Exception& e)
  {
    MITK_ERROR << "MITK Exception: " << e.what();
  }
  catch (...)
  {
    std::cerr << "Unexpected error!";
   return EXIT_FAILURE;
  }
}
