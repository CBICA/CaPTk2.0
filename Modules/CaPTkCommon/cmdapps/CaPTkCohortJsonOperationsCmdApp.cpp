/** \file CaPTkCohortJsonOperationsCmdApp.cpp
 * \brief CLI program for operations related to cohorts
 * 
 * Such operations are related with interacting with
 * cohort representations on the file system,
 * either via json files or directory structures
 */

#include <mitkCommandLineParser.h>

#include "CaPTkCohortOperations.h"

#include <mitkException.h>
#include <mitkLogMacros.h>

#include <QString>
#include <QStringList>
#include <QFile>
#include <QJsonDocument>
#include <QSharedPointer>

#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>

/** \brief input directory structure -> json */
const QString MODE_DIR_TO_JSON = "dirtojson";
/** \brief input json files -> one merged json file */
const QString MODE_MERGE_JSONS = "mergejsons";

/** \brief list of all the modes (operations) */
const QStringList MODES = QStringList() 
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

  auto outputfile = QString(
    us::any_cast<std::string>(parsedArgs["outputfile"]).c_str()
  );
  auto mode = QString(
    us::any_cast<std::string>(parsedArgs["mode"]).c_str()
  );

  if (!MODES.contains(mode))
  {
    std::cerr << "Unknown mode\n";
    return EXIT_FAILURE;
  }

  /*---- Partially required parameters ----*/

  QStringList directories;
  QStringList jsons;

  if (mode == MODE_DIR_TO_JSON)
  {
    // This mode needs the following arguments

    if (parsedArgs["directories"].Empty())
    {
      std::cerr << parser.helpText();
      std::cerr << "arg \"directories\" is required in mode" << mode.toStdString() << "\n";
      return EXIT_FAILURE;
    }
    // Separate by comma
    directories = QString(
      us::any_cast<std::string>(parsedArgs["directories"]).c_str()
    ).split(",");
  }
  else if (mode == MODE_MERGE_JSONS)
  {
    // This mode needs the following arguments

    if (parsedArgs["jsons"].Empty())
    {
      std::cerr << parser.helpText();
      std::cerr << "arg \"jsons\" is required in mode" << mode.toStdString() << "\n";
      return EXIT_FAILURE;
    }
    // Separate by comma
    jsons = QString(
      us::any_cast<std::string>(parsedArgs["jsons"]).c_str()
    ).split(",");
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
      QList<QSharedPointer<captk::Cohort>> cohorts;
      for (auto jsonDoc : jsonDocs)
      {
        cohorts.push_back(captk::CohortJsonLoad(jsonDoc));
      }
      auto mergedCohort = captk::CohortMerge(cohorts);
      auto mergedJson = captk::CohortToJson(mergedCohort);
      
      // Save merged json to file
      QFile jsonFile(outputfile);
      jsonFile.open(QFile::WriteOnly);
      jsonFile.write(mergedJson->toJson(QJsonDocument::Indented));
    }
    else if (mode == MODE_MERGE_JSONS)
    {
      // Create a QJsonDocument for each json file and add it to the list
      QList<QSharedPointer<QJsonDocument>> jsonDocs;
      foreach (QString json, jsons)
      {
        QFile jsonFile(json);
        jsonFile.open(QFile::ReadOnly);
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
        // Looks bad, but tries to avoid making unnecessary copies
        jsonDocs.append(
          QSharedPointer<QJsonDocument>(
            new QJsonDocument(std::move(doc))
          )
        );
      }

      // Create merged json from all the jsonDocs
      QList<QSharedPointer<captk::Cohort>> cohorts;
      for (auto jsonDoc : jsonDocs)
      {
        cohorts.push_back(captk::CohortJsonLoad(jsonDoc));
      }
      auto mergedCohort = captk::CohortMerge(cohorts);
      auto mergedJson = captk::CohortToJson(mergedCohort);

      // Save merged json to file
      QFile jsonFile(outputfile);
      jsonFile.open(QFile::WriteOnly);
      jsonFile.write(mergedJson->toJson(QJsonDocument::Indented));
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
