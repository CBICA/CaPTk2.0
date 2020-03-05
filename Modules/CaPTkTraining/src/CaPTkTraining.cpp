#include "CaPTkTraining.h"

#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

#include <iostream>
#include <fstream>

#include "CaPTkTrainingAlgorithm.h"

captk::Training::Training(QObject *parent) : 
	QObject(parent)
{
	connect(&m_Watcher, SIGNAL(finished()), this, SLOT(OnAlgorithmFinished()));
}

captk::Training::~Training()
{
	
}

void captk::Training::Run(
	const QString featuresCsvPath,
	const QString responsesCsvPath,
	const QString classificationKernelStr,
	const QString configurationStr,
	const int     folds,
	const int     samples,
	const QString modelDirPath,
	const QString outputDirPath)
{
	std::cout << "[captk::Training::Run]\n";

	/* ---- Check if it's already running ---- */

	if (m_IsRunning)
	{
		QMessageBox msgError;
		msgError.setText(
			"The algorithm is already running!\nPlease wait for it to finish."
		);
		msgError.setIcon(QMessageBox::Critical);
		msgError.setWindowTitle("Please wait");
		msgError.exec();
		return;
	}
	m_IsRunning = true;

	/* ---- Run ---- */

	// std::bind is used because normally
	// QtConcurrent::run accepts max=5 function arguments
	m_FutureResult = QtConcurrent::run(std::bind(
		&captk::Training::RunThread, this,
		featuresCsvPath,
		responsesCsvPath,
		classificationKernelStr,
		configurationStr,
		folds,
		samples,
		modelDirPath,
		outputDirPath
	));
	m_Watcher.setFuture(m_FutureResult);
}

void captk::Training::SetProgressBar(QProgressBar* progressBar)
{
	m_ProgressBar = progressBar;
}

void captk::Training::OnAlgorithmFinished()
{
	std::cout << "[captk::Training::OnAlgorithmFinished]\n";

	if (m_FutureResult.result().ok)
	{
		// Execution finished successfully
		QMessageBox msgSuccess;
		msgSuccess.setText("Training Plugin finished successfully!");
		msgSuccess.setIcon(QMessageBox::Information);
		msgSuccess.setWindowTitle("CaPTk Training Plugin");
		msgSuccess.exec();
	}
	else
	{
		// Something went wrong
		QMessageBox msgError;
		msgError.setText(m_FutureResult.result().errorMessage.c_str());
		msgError.setIcon(QMessageBox::Critical);
		msgError.setWindowTitle("CaPTk Training Plugin");
		msgError.exec();
	}

	m_IsRunning = false;
}

captk::Training::Result
captk::Training::RunThread(
	const QString& featuresCsvPath,
	const QString& responsesCsvPath,
	const QString& classificationKernelStr,
	const QString& configurationStr,
	const int      folds,
	const int      samples,
	const QString& modelDirPath,
	const QString& outputDirPath)
{
	std::cout << "[captk::Training::RunThread]\n";

	captk::Training::Result runResult;

	/*---- Kernel and configuration are needed as integers from the algorithm ----*/

	// Kernel (linear, RBF, etc)
	int classificationKernel = 
		(classificationKernelStr.contains("Linear", Qt::CaseInsensitive)) ? 1 : 2;

	// Configuration (cross-validation, train, etc)
	int configuration = 0;
	if (configurationStr.contains("Cross", Qt::CaseInsensitive))
	{
		configuration = 1;
	}
	else if (configurationStr.contains("Train", Qt::CaseInsensitive) &&
			 configurationStr.contains("Test",  Qt::CaseInsensitive))
	{
		// For "Split Train/Test"
		configuration = 2;
	}
	else if (configurationStr.contains("Train", Qt::CaseInsensitive))
	{
		configuration = 3;
	}
	else if (configurationStr.contains("Test", Qt::CaseInsensitive))
	{
		configuration = 4;
	}
	else
	{
		runResult.ok = false;
		runResult.errorMessage = "Unknown configuration";
		return runResult;
	}

	/*---- Check folds and samples. 
	       They are set by the same variable to the algorithm ----*/

	if (folds < 2 && configuration == 1)
	{
		runResult.ok = false;
		runResult.errorMessage = "\"Folds\" need to be at least 2";
		return runResult;		
	}

	if (samples < 1 && configuration == 2)
	{
		runResult.ok = false;
		runResult.errorMessage = "\"Samples\" need to be a positive non-zero number";
		return runResult;
	}

	int foldsOrSamples = (configuration == 1) ? folds : samples;

	/*---- Check paths for whitespace in start/end and other quirks ----*/

	auto inputStrings = QStringList() << featuresCsvPath << responsesCsvPath 
	                                  << modelDirPath << outputDirPath;

	for (QString s : inputStrings)
	{
		if (s != s.trimmed())
		{
			runResult.ok = false;
			runResult.errorMessage = "Please remove whitespace from the start or the end of the paths provided";
			return runResult;
		}

		if (s.startsWith("file://"))
		{
			runResult.ok = false;
			runResult.errorMessage = "Please remove \"file://\" from your paths";
			return runResult;
		}
	}

	/*---- Check files (features and responses) ----*/

	QFileInfo fiFeatures(featuresCsvPath);
	QFileInfo fiResponses(responsesCsvPath);
	if (!fiFeatures.exists() || fiFeatures.size() == 0 || 
	    !featuresCsvPath.endsWith(".csv"))
	{
		// doesn't exist, or is empty, or not a csv
		runResult.ok = false;
		runResult.errorMessage = "Features file doesn't exist, is empty, or is not csv";
		return runResult;
	}
	if (configuration != 4 && 
		( !fiResponses.exists() || fiResponses.size() == 0 || 
		  !responsesCsvPath.endsWith(".csv")) )
	{
		// If configuration != "Test" AND
		// file doesn't exist, or is empty, or not a csv
		runResult.ok = false;
		runResult.errorMessage = "Responses file doesn't exist, is empty, or is not csv";
		return runResult;		
	}

	/*---- Check directories (model and output) ----*/

	// For model directory
	QDir modelDir(modelDirPath);
	if (configuration == 4 && (modelDirPath == "" || !modelDir.exists() || modelDir.isEmpty()))
	{
		runResult.ok = false;
		runResult.errorMessage = "Model directory doesn't exist or it doesn't contain files";
		return runResult;
	}

	// For output directory
	if (outputDirPath == QString(""))
	{
		runResult.ok = false;
		runResult.errorMessage = "Please set an output directory";
		return runResult;
	}
	QDir outputDir(outputDirPath);
	if (!outputDir.exists())
	{
		// Create output dir if it doesn't exist
		if (!QDir().mkdir(outputDirPath))
		{
			runResult.ok = false;
			runResult.errorMessage = "Can not create output directory";
			return runResult;
		}
	}

	/*---- Run ----*/

	try
	{
		captk::TrainingAlgorithm algorithm = captk::TrainingAlgorithm();
		auto resAlgorithm = algorithm.Run(
			featuresCsvPath.toStdString(),
			responsesCsvPath.toStdString(),
			outputDirPath.toStdString(),
			classificationKernel,
			foldsOrSamples,
			configuration,
			modelDirPath.toStdString());

		runResult.ok = std::get<0>(resAlgorithm);
		runResult.errorMessage = std::get<1>(resAlgorithm);
		return runResult;
	}
	catch(const std::exception& e)
	{
		runResult.ok = false;
		runResult.errorMessage = 
				std::string("Something went wrong when executing. This was probably ")
				+ std::string("because of bad supplied input data or models:\n\n")
				+ e.what();
		return runResult;
	}
	catch(...)
	{
		runResult.ok = false;
		runResult.errorMessage = 
				std::string("Unknown error. This was probably ")
				+ std::string("because of bad supplied input data or models:\n\n");
		return runResult;
	}
}
