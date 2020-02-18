#include "CaPTkSurvival.h"

#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QFileInfo>

#include <iostream>
#include <fstream>

#include "CaPTkSurvivalPredictionAlgorithm.h"
namespace captk {
CaPTkSurvival::CaPTkSurvival(
	QObject *parent)
	: QObject(parent)
{
	connect(&m_Watcher, SIGNAL(finished()), this, SLOT(OnAlgorithmFinished()));
}

void CaPTkSurvival::Run(
        QString modelDir,
        QString subjectDir,
        QString outputDir,
        bool trainNewModel,
        bool useCustomModel
	)
{
    MITK_INFO << "[CaPTkSurvival::Run]";

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
    // This should only be reached if all requirements are met.
    // std::bind is used because normally QtConcurrent::run accepts max=5 function arguments
    m_FutureResult = QtConcurrent::run(std::bind(
        &CaPTkSurvival::RunThread, this,
        modelDir,
        subjectDir,
        outputDir,
        trainNewModel,
        useCustomModel
    ));
    m_Watcher.setFuture(m_FutureResult);
}

void CaPTkSurvival::SetProgressBar(QProgressBar* progressBar)
{
	m_ProgressBar = progressBar;
}

void CaPTkSurvival::OnAlgorithmFinished()
{
    MITK_INFO << "[CaPTkSurvival::OnAlgorithmFinished]";

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
		msgError.setWindowTitle("CaPTk Survival Module Error!");
		msgError.exec();
	}

	m_IsRunning = false;
}

CaPTkSurvival::Result
CaPTkSurvival::RunThread(
        QString modelDir,
        QString subjectDir,
        QString outputDir,
        bool trainNewModel,
        bool useCustomModel
	)
{
    MITK_INFO << "[CaPTkSurvival::RunThread]";

    CaPTkSurvival::Result runResult;

    SurvivalPredictionModuleAlgorithm algorithm = SurvivalPredictionModuleAlgorithm();
    auto resAlgorithm = algorithm.Run(
               modelDir,
               subjectDir,
               outputDir,
               trainNewModel,
               useCustomModel
                );

	runResult.ok = std::get<0>(resAlgorithm);
	runResult.errorMessage = std::get<1>(resAlgorithm);

	return runResult;
}
}
