#include <mitkCommandLineParser.h>

#include <CaPTkCohortJsonOperations.h>

#include <QString>
#include <QStringList>
#include <QFile>
#include <QJsonDocument>

#include <json/json.h>

#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>

// MODE_DIR_TO_JSON:  input directory structure -> json
// MODE_MERGE_JSONS: input json files -> one merged json file

final const QString MODE_DIR_TO_JSON = "dirtojson";
final const QString MODE_MERGE_JSONS = "mergejsons";

/** list of all the modes (operations) */
final const QStringList MODES = QStringList() 
  << MODE_DIR_TO_JSON
  << MODE_MERGE_JSONS;

/** \brief command-line app for batch processing of images
 *
 * This command-line app takes a list of directories and merges them to create a cohort json file.
 */
int main(int argc, char* argv[])
{
  mitkCommandLineParser parser;

  /*---- Set general information about the command-line app ----*/

  parser.setCategory("CaPTk Cmd App Category");
  parser.setTitle("CaPTk Cohort Json Operations Cmd App");
  parser.setContributor("CBICA");
  parser.setDescription(
    "This command-line app supports operations for cohort json files.");

  // How should arguments be prefixed
  parser.setArgumentPrefix("--", "-");

  /*---- Add arguments. Unless specified otherwise, each argument is optional.
            See mitkCommandLineParser::addArgument() for more information. ----*/

  parser.addArgument(
    "mode",
    "m",
    mitkCommandLineParser::String,
    "Mode",
    std::string("Operation to perform. Modes: ") + MODES.join(" or ").toStdString(),
    us::Any(),
    false); // false -> required parameter

  parser.addArgument(
    "directories",
    "d",
    mitkCommandLineParser::String,
    "Directories",
    "Paths to the input directories, separated by comma",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "jsons",
    "j",
    mitkCommandLineParser::String,
    "JSONs",
    "Paths to the input jsons, separated by comma",
    us::Any(),
    true); // false -> required parameter

  parser.addArgument(
    "outputfile",
    "o",
    mitkCommandLineParser::String,
    "Output File",
    "Path to the desired output",
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

  if (parsedArgs["outputfile"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  if (parsedArgs["mode"].Empty())
  {
    std::cerr << parser.helpText();
    return EXIT_FAILURE;
  }

  auto outputfile = QString(us::any_cast<std::string>(parsedArgs["outputfile"]).c_str());
  auto mode = QString(us::any_cast<std::string>(parsedArgs["mode"]).c_str());

  if (!MODE.contains(mode))
  {
    std::cerr << "Unknown mode\n";
    return EXIT_FAILURE;
  }

  /*---- Partially required parameters ----*/

  QStringList directories;
  QString jsons;

  if (mode == MODE_DIR_TO_JSON)
  {
    if (parsedArgs["directories"].Empty())
    {
      std::cerr << parser.helpText();
      return EXIT_FAILURE;
    }
    // Separate by comma
    directories = QString(us::any_cast<std::string>(parsedArgs["directories"]).c_str()).split(",");
  }
  else if (mode == MODE_MERGE_JSONS)
  {
    if (parsedArgs["jsons"].Empty())
    {
      std::cerr << parser.helpText();
      return EXIT_FAILURE;
    }
    // Separate by comma
    jsons = QString(us::any_cast<std::string>(parsedArgs["jsons"]).c_str()).split(",");
  }

  /*---- Run ----*/

  try
  {
    if (mode == MODE_DIR_TO_JSON)
    {
      // Create a QJsonDocument for each directory and add it to the list
      QList<QSharedPointer<QJsonDocument>> jsonDocs;
      foreach (QString dir, directories)
      {
        jsonDocs.append(captk::CohortJsonFromDirectoryStructure(dir));
      }

      // Create merged json from all the jsonDocs
      auto mergedJson = captk::CohortJsonMergeObjects(jsonDocs);
      
      // Save merged json to file
      QFile jsonFile(outputfile);
      jsonFile.open(QFile::WriteOnly);
      jsonFile.write(mergedJson.toJson(QJsonDocument::Indented));
    }
    else if (mode == MODE_MERGE_JSONS)
    {
      // Create a QJsonDocument for each json file and add it to the list
      QList<QSharedPointer<QJsonDocument>> jsonDocs;
      foreach (QString json, jsons)
      {
        QFile jsonFile(json);
        jsonFile.open(QFile::ReadOnly);
        jsonDocs.append(QJsonDocument().fromJson(jsonFile.readAll()));
      }

      // Create merged json from all the jsonDocs
      auto mergedJson = captk::CohortJsonMergeObjects(jsonDocs);

      // Save merged json to file
      QFile jsonFile(outputfile);
      jsonFile.open(QFile::WriteOnly);
      jsonFile.write(mergedJson.toJson(QJsonDocument::Indented));
    }
  }
  catch()
  {
    MITK_ERROR << "MITK Exception: " << e.what();
  }
  catch (...)
  {
    std::cerr << "Unexpected error!";
    return EXIT_FAILURE;
  }
}
