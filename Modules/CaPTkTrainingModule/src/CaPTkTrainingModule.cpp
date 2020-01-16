#include "CaPTkTrainingModule.h"

#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

#include <iostream>
#include <fstream>

CaPTkTrainingModule::CaPTkTrainingModule(
	QObject *parent)
	: QObject(parent)
{
	connect(&m_Watcher, SIGNAL(finished()), this, SLOT(OnAlgorithmFinished()));
}

void CaPTkTrainingModule::Run(
	QString featuresCsvPath,
	QString responsesCsvPath,
	QString classificationKernelStr,
	QString configurationStr,
	QString folds,
	QString samples,
	QString modelDirPath,
	QString outputDirPath)
{
	std::cout << "[CaPTkTrainingModule::Run]\n";

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

	/* ---- Check requirements ---- */

	bool ok = true;              // Becomes false if there is an issue
	std::string problemStr = ""; // Populated if there is an issue

	// TODO: Check requirements (now everything is assumed ok)

	// Return if there is an issue
	if (!ok)
	{
		QMessageBox msgError;
		msgError.setText(problemStr.c_str());
		msgError.setIcon(QMessageBox::Critical);
		msgError.setWindowTitle("Incorrect state.");
		msgError.exec();
		m_IsRunning = false;
		return;
	}

	/* ---- Run ---- */

	// std::bind is used because normally
	// QtConcurrent::run accepts max=5 function arguments
	m_FutureResult = QtConcurrent::run(std::bind(
		&CaPTkTrainingModule::RunThread, this,
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

void CaPTkTrainingModule::SetProgressBar(QProgressBar* progressBar)
{
	m_ProgressBar = progressBar;
}

void CaPTkTrainingModule::OnAlgorithmFinished()
{
	std::cout << "[CaPTkTrainingModule::OnAlgorithmFinished]\n";

	if (m_FutureResult.result().ok)
	{
		// Execution finished successfully
	}
	else
	{
		// Something went wrong
		QMessageBox msgError;
		msgError.setText(m_FutureResult.result().errorMessage.c_str());
		msgError.setIcon(QMessageBox::Critical);
		msgError.setWindowTitle("CaPTk Training Module Error!");
		msgError.exec();
	}

	m_IsRunning = false;
}

CaPTkTrainingModule::Result
CaPTkTrainingModule::RunThread(
	QString& featuresCsvPath,
	QString& responsesCsvPath,
	QString& classificationKernelStr,
	QString& configurationStr,
	QString& folds,
	QString& samples,
	QString& modelDirPath,
	QString& outputDirPath)
{
	std::cout << "[CaPTkTrainingModule::RunThread]\n";

	CaPTkTrainingModule::Result runResult;

	// TODO

	// Bypass unused parameter
	// TODO: Delete this
	featuresCsvPath = featuresCsvPath;
	responsesCsvPath = responsesCsvPath;
	classificationKernelStr = classificationKernelStr;
	configurationStr = configurationStr;
	folds = folds;
	samples = samples;
	modelDirPath = modelDirPath;
	outputDirPath = outputDirPath;

	return runResult;
}
