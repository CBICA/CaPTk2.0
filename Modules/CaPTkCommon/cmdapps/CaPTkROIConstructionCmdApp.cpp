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
    std::string("This command-line app supports operations for lattice ") + 
    std::string("ROI construction for feature extraction."));

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
    "mode",
    "m",
    mitkCommandLineParser::Bool,
    "mode",
    "Set mode (roi_based or full)",
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

  if (parsedArgs["mode"].Empty())
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

  auto maskPath  = QString(us::any_cast<std::string>(parsedArgs["mask"]).c_str());
  auto outputDir = QString(us::any_cast<std::string>(parsedArgs["output-dir"]).c_str());
  auto modeStr   = QString(us::any_cast<std::string>(parsedArgs["mode"]).c_str());
  float radius   = us::any_cast<float>(parsedArgs["radius"]);
  float step     = us::any_cast<float>(parsedArgs["step"]);

  /*---- Run ----*/

  auto maskFileName = QFileInfo(maskPath).fileName();

  try
  {
    auto mask = mitk::IOUtil::Load<mitk::LabelSetImage>(maskPath.toStdString());

    auto mode = (modeStr.toLower() == "full") ? 
        captk::ROIConstructionHelperBase::MODE::FULL : 
        captk::ROIConstructionHelperBase::MODE::ROI_BASED;

    // Iterate through the labels
    mitk::LabelSet::Pointer labelSet = mask->GetActiveLabelSet();
    mitk::LabelSet::LabelContainerConstIteratorType it;
    for (it = labelSet->IteratorConstBegin();
         it != labelSet->IteratorConstEnd();
         ++it)
    {
      if (it->second->GetValue() != 0) // Ignore zero
      {
        // ---- For each label in the mask ----

        auto value = it->second->GetValue();
        auto name  = it->second->GetName();

        // TODO: Delete all the other labels

        std::cout << "Found label set name: "
                  << it->second->GetName() << "\n";

        captk::ROIConstruction constructor;
        constructor.Update(
            mask,
            mode,
            radius,
            step);

        int counter = 1;
        while (!constructor.IsAtEnd())
        {
          mitk::LabelSetImage::Pointer rMask;
          float weight = constructor.PopulateMask(rMask);

          auto outputPath = outputDir + QDir::separator() +
                            QString::number(counter++) + QString("_") +
                            QString::number(value)     + QString("_") +
                            QString(name.c_str())      + QString("_") +
                            QString::number(weight)    + QString("_") +
                            maskFileName;

          std::cout << ">Saving ROI to: "
                    << outputPath.toStdString() << "\n";

          mitk::IOUtil::Save(rMask, outputPath.toStdString());

          constructor++;
        }
      }
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
