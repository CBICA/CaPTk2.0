#include <mitkCommandLineParser.h>

#include <CaPTkROIConstruction.h>

#include "mitkException.h"
#include "mitkLogMacros.h"
#include <mitkIOUtil.h>
#include <mitkLabelSetImageConverter.h>

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
    std::string("This command-line app supports operations for lattice ") + 
    std::string("ROI construction for feature extraction."));

  // How should arguments be prefixed
  parser.setArgumentPrefix("--", "-");


  /*---- Add arguments. Unless specified otherwise, each argument is optional.
            See mitkCommandLineParser::addArgument() for more information. ----*/

  parser.addArgument(
    "input",
    "i",
    mitkCommandLineParser::String,
    "Input (Mask)",
    "Input Mask Image",
    us::Any(),
    false); // false -> required parameter

  parser.addArgument(
    "radius",
    "r",
    mitkCommandLineParser::Float,
    "Radius",
    "Lattice window radius",
    us::Any(),
    false); // false -> required parameter

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

  if (parsedArgs["input"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  if (parsedArgs["radius"].Empty())
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

  /*---- Actual parsing ----*/

  auto maskPath  = QString(us::any_cast<std::string>(parsedArgs["input"]).c_str());
  auto outputDir = QString(us::any_cast<std::string>(parsedArgs["output-dir"]).c_str());
  float radius   = us::any_cast<float>(parsedArgs["radius"]);
  float step     = us::any_cast<float>(parsedArgs["step"]);
  
  /*---- Run ----*/

  auto maskFileName = QFileInfo(maskPath).fileName();

  try
  {
    auto mask = mitk::IOUtil::Load<mitk::LabelSetImage>(maskPath.toStdString());

    captk::ROIConstruction constructor;
    
    constructor.Update(
        mask,
        radius,
        step);

    int counter = 1; // for unique output file name

    for (constructor.GoToBegin(); !constructor.IsAtEnd(); constructor++)
    {
      // Create patch mask (patch is the area around the lattice)
      mitk::LabelSetImage::Pointer rMask = mitk::LabelSetImage::New();

      // Each time rMask gets populated with one patch (containing one label)
      std::cout << "Populating mask...\n";
      float weight = constructor.PopulateMask(rMask);

      // Get the info of this mask and create the output path
      auto name = rMask->GetActiveLabelSet()->GetActiveLabel()->GetName();
      auto value = rMask->GetActiveLabelSet()->GetActiveLabel()->GetValue();
      auto outputPath = outputDir + QDir::separator() +
                        QString::number(counter++) + QString("_") +
                        QString::number(value) + QString("_") +
                        QString(name.c_str()) + QString("_") +
                        QString::number(weight) + QString("_") +
                        maskFileName +
                        QString(".nrrd");

      // Save image
      std::cout << ">Saving ROI to: "
                << outputPath.toStdString() << "\n";
      mitk::IOUtil::Save(rMask, outputPath.toStdString());
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