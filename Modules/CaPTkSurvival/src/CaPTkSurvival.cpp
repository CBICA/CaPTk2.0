#include "CaPTkSurvival.h"

#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QFileInfo>

#include <iostream>
#include <fstream>
#include "mitkLogMacros.h"

#include "CaPTkSurvivalPredictionAlgorithm.h"
namespace captk {
CaPTkSurvival::CaPTkSurvival(
	QObject *parent)
	: QObject(parent)
{
	connect(&m_Watcher, SIGNAL(finished()), this, SLOT(OnAlgorithmFinished()));
    m_CbicaModelDir = qApp->applicationDirPath() + QString("/models/survival_model");
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
    if (subjectDir.isEmpty()) {
        ok = false;
        problemStr += "Please specify a directory containing subject data.\n";
    }
    if (useCustomModel && modelDir.isEmpty()) {
        ok = false;
        problemStr += "Please specify a directory containing a model to use.\n";
    }
    if (outputDir.isEmpty()) {
        ok = false;
        problemStr += "Please specify a valid output directory.\n";
    }

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
        useCustomModel,
        m_CbicaModelDir
    ));
    m_Watcher.setFuture(m_FutureResult);
}


void CaPTkSurvival::OnAlgorithmFinished()
{
    MITK_INFO << "[CaPTkSurvival::OnAlgorithmFinished]";

	if (m_FutureResult.result().ok)
	{
		// Execution finished successfully
        QMessageBox msgSuccess;
        QString msg = "A Survival Prediction Index (SPI) has been calculated for "
                      "the given subjects by applying the specified model. \n\n";
        msg += "SPI index saved in 'results.csv' file in the output directory. \n\n";
        msgSuccess.setText(msg);
        msgSuccess.setIcon(QMessageBox::Information);
        msgSuccess.setWindowTitle("CaPTk Survival Module Success!");
        msgSuccess.exec();
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
        bool useCustomModel,
        QString cbicaModelDir
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
               useCustomModel,
               cbicaModelDir
                );

	runResult.ok = std::get<0>(resAlgorithm);
	runResult.errorMessage = std::get<1>(resAlgorithm);

	return runResult;
}


}
