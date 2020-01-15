/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "QmitkCaPTkTrainingPluginView.h"

// blueberry
#include <berryConstants.h>
#include <berryIWorkbenchPage.h>

// mitk
#include "mitkApplicationCursor.h"
#include "mitkStatusBar.h"
#include "mitkPlanePositionManager.h"
#include "mitkPluginActivator.h"

// Qmitk
#include "QmitkRenderWindow.h"

// us
#include <usGetModuleContext.h>
#include <usModule.h>
#include <usModuleContext.h>
#include <usModuleResource.h>
#include <usModuleResourceStream.h>

// Qt
#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

// CaPTk
// #include "CaPTkTrainingModule.h"

#include "tinyxml.h"

#include <itksys/SystemTools.hxx>

#include <regex>

const std::string QmitkCaPTkTrainingPluginView::VIEW_ID = "org.mitk.views.captk.trainingplugin";

QmitkCaPTkTrainingPluginView::QmitkCaPTkTrainingPluginView()
  : m_Parent(nullptr)
{
  // m_CaPTkTrainingModule = new CaPTkTrainingModule(this);
}

QmitkCaPTkTrainingPluginView::~QmitkCaPTkTrainingPluginView()
{

}

void QmitkCaPTkTrainingPluginView::CreateQtPartControl(QWidget *parent)
{
  // setup the basic GUI of this view
  m_Parent = parent;

  m_Controls.setupUi(parent);

  // m_CaPTkTrainingModule->SetProgressBar(m_Controls.progressBar);

  /**** Initialize widgets ****/

  // Initialize kernel combo box
  m_Controls.comboBox_kernel->addItems(QStringList() << "SVM: Linear" << "SVM: RBF");
  // Set combo box to the last user selected value
  m_Controls.comboBox_kernel->setCurrentText(
    this->GetPreferences()->Get("TrainingPluginKernelComboBox", "SVM: Linear")
  );

  // Initialize kernel combo box
  m_Controls.comboBox_configuration->addItems(
    QStringList() << "Cross-validation" << "Split Train/Test" 
                  << "Split Train" << "Split Test"
  );
  // Set combo box to the last user selected value
  auto prefConfigurationText = this->GetPreferences()->Get(
    "TrainingPluginConfigurationComboBox", "Cross-validation"
  );
  m_Controls.comboBox_configuration->setCurrentText(prefConfigurationText);
  this->OnConfigurationComboBoxCurrentTextChanged(prefConfigurationText);

  //  this->InitializeListeners();

  /**** Connect signals & slots ****/

  connect(m_Controls.comboBox_kernel, SIGNAL(currentTextChanged(const QString&)),
    this, SLOT(OnKernelComboBoxCurrentTextChanged(const QString&))
  );

  connect(m_Controls.comboBox_configuration, SIGNAL(currentTextChanged(const QString&)),
    this, SLOT(OnConfigurationComboBoxCurrentTextChanged(const QString&))
  );

  connect(m_Controls.pushButton_features, SIGNAL(clicked()),
    this, SLOT(OnFeaturesCsvButtonClicked())
  );

  connect(m_Controls.pushButton_responses, SIGNAL(clicked()),
    this, SLOT(OnResponsesCsvButtonClicked())
  );

  connect(m_Controls.pushButton_modeldir, SIGNAL(clicked()),
    this, SLOT(OnModelDirectoryButtonClicked())
  );

  connect(m_Controls.pushButton_outputdir, SIGNAL(clicked()),
    this, SLOT(OnOutputDirectoryButtonClicked())
  );

  /* Run Button */
  connect(m_Controls.pushButtonRun, SIGNAL(clicked()), 
    this, SLOT(OnRunButtonPressed())
  );
}

void QmitkCaPTkTrainingPluginView::Activated()
{

}

void QmitkCaPTkTrainingPluginView::Deactivated()
{
  // Not yet implemented
}

void QmitkCaPTkTrainingPluginView::Visible()
{
  // Not yet implemented
}

void QmitkCaPTkTrainingPluginView::Hidden()
{
  // Not yet implemented
}

int QmitkCaPTkTrainingPluginView::GetSizeFlags(bool width)
{
  if (!width)
  {
    return berry::Constants::MIN | berry::Constants::MAX | berry::Constants::FILL;
  }
  else
  {
    return 0;
  }
}

int QmitkCaPTkTrainingPluginView::ComputePreferredSize(bool width,
                                                          int /*availableParallel*/,
                                                          int /*availablePerpendicular*/,
                                                          int preferredResult)
{
  if (width == false)
  {
    return 100;
  }
  else
  {
    return preferredResult;
  }
}

/************************************************************************/
/* protected slots                                                      */
/************************************************************************/
void QmitkCaPTkTrainingPluginView::OnKernelComboBoxCurrentTextChanged(const QString& text)
{
  // Remember it
  this->GetPreferences()->Put("TrainingPluginKernelComboBox", text);
  this->GetPreferences()->Flush();
}

void QmitkCaPTkTrainingPluginView::OnConfigurationComboBoxCurrentTextChanged(const QString& text)
{
  // Remember it
  this->GetPreferences()->Put("TrainingPluginConfigurationComboBox", text);
  this->GetPreferences()->Flush();

  // Show/Hide views below it
  if (text == "Cross-validation")
  {
    m_Controls.lineEdit_folds->setVisible(true);
    m_Controls.label_folds->setVisible(true);
    m_Controls.lineEdit_samples->setVisible(false);
    m_Controls.label_samples->setVisible(false);
    m_Controls.lineEdit_modeldir->setVisible(false);
    m_Controls.pushButton_modeldir->setVisible(false);
  }
  else if (text == "Split Train/Test")
  {
    m_Controls.lineEdit_folds->setVisible(false);
    m_Controls.label_folds->setVisible(false);
    m_Controls.lineEdit_samples->setVisible(true);
    m_Controls.label_samples->setVisible(true);
    m_Controls.lineEdit_modeldir->setVisible(false);
    m_Controls.pushButton_modeldir->setVisible(false);
  }
  else if (text == "Split Train")
  {
    m_Controls.lineEdit_folds->setVisible(false);
    m_Controls.label_folds->setVisible(false);
    m_Controls.lineEdit_samples->setVisible(false);
    m_Controls.label_samples->setVisible(false);
    m_Controls.lineEdit_modeldir->setVisible(false);
    m_Controls.pushButton_modeldir->setVisible(false);
  }
  else if (text == "Split Test")
  {
    m_Controls.lineEdit_folds->setVisible(false);
    m_Controls.label_folds->setVisible(false);
    m_Controls.lineEdit_samples->setVisible(false);
    m_Controls.label_samples->setVisible(false);
    m_Controls.lineEdit_modeldir->setVisible(true);
    m_Controls.pushButton_modeldir->setVisible(true);
  }
}

void QmitkCaPTkTrainingPluginView::OnFeaturesCsvButtonClicked()
{
  auto fileName = QFileDialog::getOpenFileName(m_Parent,
    tr("Select CSV file"), this->GetLastFileOpenPath(), tr("CSV Files (*.csv)"));

  if(fileName.isEmpty() || fileName.isNull()) { return; }

  // Find and save file information
  QFileInfo file(fileName);
  if (!file.exists()) { return; }
  auto containingDir = file.dir();
  this->SetLastFileOpenPath(containingDir.absolutePath());

  // Set the path to the QLineEdit
  m_Controls.lineEdit_features->setText(fileName);
}

void QmitkCaPTkTrainingPluginView::OnResponsesCsvButtonClicked()
{
  auto fileName = QFileDialog::getOpenFileName(m_Parent,
    tr("Select CSV file"), this->GetLastFileOpenPath(), tr("CSV Files (*.csv)"));

  if(fileName.isEmpty() || fileName.isNull()) { return; }

  // Find and save file information
  QFileInfo file(fileName);
  if (!file.exists()) { return; }
  auto containingDir = file.dir();
  this->SetLastFileOpenPath(containingDir.absolutePath());

  // Set the path to the QLineEdit
  m_Controls.lineEdit_responses->setText(fileName);
}

void QmitkCaPTkTrainingPluginView::OnModelDirectoryButtonClicked()
{
  auto dirName = QFileDialog::getExistingDirectory(m_Parent, 
    tr("Select directory"), this->GetLastFileOpenPath());

  if(dirName.isEmpty() || dirName.isNull()) { return; }

  // Find and save file information
  QFileInfo file(dirName);
  if (!file.isDir()) { return; }
  this->SetLastFileOpenPath(dirName);

  // Set the path to the QLineEdit
  m_Controls.lineEdit_modeldir->setText(dirName);
}

void QmitkCaPTkTrainingPluginView::OnOutputDirectoryButtonClicked()
{
  auto dirName = QFileDialog::getExistingDirectory(m_Parent, 
    tr("Select directory"), this->GetLastFileOpenPath());

  if(dirName.isEmpty() || dirName.isNull()) { return; }

  // Find and save file information
  QFileInfo file(dirName);
  if (!file.isDir()) { return; }
  this->SetLastFileOpenPath(dirName);

  // Set the path to the QLineEdit
  m_Controls.lineEdit_outputdir->setText(dirName);
}

void QmitkCaPTkTrainingPluginView::OnRunButtonPressed()
{

}

/************************************************************************/
/* protected                                                            */
/************************************************************************/
void QmitkCaPTkTrainingPluginView::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList<mitk::DataNode::Pointer> &)
{

}

void QmitkCaPTkTrainingPluginView::OnPreferencesChanged(const berry::IBerryPreferences*)
{

}

void QmitkCaPTkTrainingPluginView::NodeAdded(const mitk::DataNode *)
{

}

void QmitkCaPTkTrainingPluginView::NodeRemoved(const mitk::DataNode *)
{

}

void QmitkCaPTkTrainingPluginView::SetFocus()
{
}

void QmitkCaPTkTrainingPluginView::UpdateControls()
{
  // Hide views that are not useful
  // m_Controls.label_PatientImage->setVisible(false);

  this->RequestRenderWindowUpdate(mitk::RenderingManager::REQUEST_UPDATE_ALL);
}

void QmitkCaPTkTrainingPluginView::InitializeListeners()
{

}

QString QmitkCaPTkTrainingPluginView::GetLastFileOpenPath()
{
  return this->GetPreferences()->Get("LastFileOpenPath", "");
}

void QmitkCaPTkTrainingPluginView::SetLastFileOpenPath(const QString &path)
{
  this->GetPreferences()->Put("LastFileOpenPath", path);
  this->GetPreferences()->Flush();
}