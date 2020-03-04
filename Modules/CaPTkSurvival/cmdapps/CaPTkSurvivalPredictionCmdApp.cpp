/** \file CaPTkSurvivalPredictionCmdApp.cpp
* \brief CLI program for invoking the Glioblastoma Survival Prediction Index algorithms
* 
* These algorithms can be used to train a new survival prediction model
* or to infer a survival prediction for subjects.
*/

#include <mitkCommandLineParser.h>
#include <mitkLogMacros.h>

#include "CaPTkSurvivalPredictionAlgorithm.h"


#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <string>
#include <map>





int main(int argc, char* argv[])
{
    // consts placed here for now
    QCoreApplication cmdapp(argc, argv);
    const QString CBICA_MODEL_DIR = qApp->applicationDirPath() + QString("/models/survival_model");
    const std::string MODE_TRAIN = "train";
    const std::string MODE_PREDICT = "predict";


    mitkCommandLineParser parser;

    /**** Set general information about the command line app ****/

    parser.setCategory("CaPTk Cmd App Category");
    parser.setTitle("CaPTk Survival Prediction Cmd App");
    parser.setContributor("CBICA");
    parser.setDescription(
                "This command line app accepts a directory of subjects and produces either a"
                " newly-trained model or a survival prediction result "
                " depending on the provided parameters.");

    parser.setArgumentPrefix("--", "-");

    /**** Add arguments. Unless specified otherwise, each argument is optional.
            See mitkCommandLineParser::addArgument() for more information. ****/

    parser.addArgument(
                "mode",
                "mo",
                mitkCommandLineParser::String,
                "Mode",
                "Select which operation to perform. Modes: " + MODE_TRAIN + " or " + MODE_PREDICT,
                us::Any(),
                false);

    parser.addArgument(
                "subjects",
                "s",
                mitkCommandLineParser::String,
                "Subjects",
                "Path to the directory containing all subjects as subdirectories",
                us::Any(),
                false);

    parser.addArgument(
                "output",
                "o",
                mitkCommandLineParser::String,
                "Output Directory",
                "Path to the desired output directory where the model or results will be created",
                us::Any(),
                false);

    parser.addArgument(
                "model",
                "md",
                mitkCommandLineParser::String,
                "Custom Model",
                "Path to the directory containing the model to use. By default, the CBICA CaPTk Model is used.",
                us::Any(),
                true);
    bool parseSuccess;
    std::map<std::string, us::Any> parsedArgs = parser.parseArguments(argc, argv, &parseSuccess);

    if (!parseSuccess)
    {
        MITK_INFO << "Failed to parse command-line parameters.";
        MITK_INFO << parser.helpText();
        return EXIT_FAILURE;

    }

    bool trainNewModel = false;
    bool useCustomModel = false;

    if (parsedArgs.empty())
    {
        MITK_INFO << "No arguments provided.";
        return EXIT_FAILURE; // Exit, usage information is already displayed
    }

    if (parsedArgs["subjects"].Empty() || parsedArgs["output"].Empty() || parsedArgs["mode"].Empty())
    {
        MITK_INFO << "Missing a required parameter.";
        MITK_INFO << parser.helpText();
        return EXIT_FAILURE;
    }


    // Handle train/predict mode switching
    if (us::any_cast<std::string>(parsedArgs["mode"]) == MODE_TRAIN)
    {
        trainNewModel = true;
    }
    else if (us::any_cast<std::string>(parsedArgs["mode"]) == MODE_PREDICT)
    {
        trainNewModel = false;
    }
    else
    {
        MITK_INFO << parser.helpText();
        MITK_ERROR << "Provided an invalid mode. See the help text for valid options.";
        return EXIT_FAILURE;
    }

    MITK_INFO << "mode: " << parsedArgs["mode"].ToString();
    MITK_INFO << "subjects: " << parsedArgs["subjects"].ToString();
    MITK_INFO << "output: " << parsedArgs["output"].ToString();

    if (parser.argumentParsed("model"))
    {
        useCustomModel = true;
        MITK_INFO << "model: " << parsedArgs["model"].ToString();
    }

    else if (parsedArgs["model"].Empty() && !trainNewModel)
    {
        useCustomModel = false;
        MITK_INFO << "Defaulting to the CBICA CaPTk survival prediction index model.\n"
                     "This is a model trained on de novo glioblastoma cases.\n"
                     "Please note that this model was created following certain assumptions\n"
                     "(described in the paper below)\n"
                     "It can be used for research purposes only.\n"
                     "L.Macyszyn, et al. Imaging Patterns Predict Patient Survival and Molecular\n"
                     "Subtype in Glioblastoma via Machine Learning Techniques, Neuro-Oncology.\n"
                     "18(3) : 417-425, 2016.\n\n"
                     "https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4767233";
    }

    // get strings from mandatory arguments
    QString outputDir = QString::fromStdString(
                parsedArgs["output"].ToString()
            );

    QString subjectDir = QString::fromStdString(
                parsedArgs["subjects"].ToString()
            );


    QString modelDir;
    if (useCustomModel)
    {
        modelDir = QString::fromStdString(parsedArgs["model"].ToString()
                );
    }


    captk::SurvivalPredictionModuleAlgorithm algorithm = captk::SurvivalPredictionModuleAlgorithm();
    auto resAlgorithm = algorithm.Run(
                modelDir,
                subjectDir,
                outputDir,
                trainNewModel,
                useCustomModel,
                CBICA_MODEL_DIR);

    bool ok = std::get<0>(resAlgorithm);
    std::string errorMessage = std::get<1>(resAlgorithm);

    if (ok)
    {
        MITK_INFO << "The CaPTk survival algorithm ran successfully.";
    }
    else
    {
        MITK_ERROR << "The CaPTk survival algorithm failed while running with the following error:";
        MITK_ERROR << errorMessage;
        return EXIT_FAILURE;
    }
    if (trainNewModel)
    {
        MITK_INFO << "The trained model has been saved in " << outputDir.toStdString();
    }
    else
    {
        MITK_INFO << "Survival prediction results have been saved in " << outputDir.toStdString();
    }

    return 0;

}
