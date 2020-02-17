#include "CaPTkSurvival.h"

#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

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
        QString outputDir,
        QString subjectDir
	)
{
	std::cout << "[CaPTkSurvival::Run]\n";

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
        &CaPTkSurvival::RunThread, this,
        modelDir,
        outputDir,
        subjectDir
    ));
    m_Watcher.setFuture(m_FutureResult);
}

void CaPTkSurvival::SetProgressBar(QProgressBar* progressBar)
{
	m_ProgressBar = progressBar;
}

void CaPTkSurvival::OnAlgorithmFinished()
{
	std::cout << "[CaPTkSurvival::OnAlgorithmFinished]\n";

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
        QString outputDir,
        QString subjectDir
	)
{
	std::cout << "[CaPTkSurvival::RunThread]\n";

    CaPTkSurvival::Result runResult;


    SurvivalPredictionModuleAlgorithm algorithm = SurvivalPredictionModuleAlgorithm();
    auto resAlgorithm = algorithm.Run(
               modelDir,
               outputDir,
               subjectDir
                );

	runResult.ok = std::get<0>(resAlgorithm);
	runResult.errorMessage = std::get<1>(resAlgorithm);

	return runResult;
}
}
