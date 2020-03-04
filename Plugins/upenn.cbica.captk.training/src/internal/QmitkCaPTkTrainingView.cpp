#include "QmitkCaPTkTrainingView.h"

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
#include "CaPTkTraining.h"

#include "tinyxml.h"

#include <itksys/SystemTools.hxx>

#include <regex>

const std::string QmitkCaPTkTrainingView::VIEW_ID = "upenn.cbica.captk.views.training";

QmitkCaPTkTrainingView::QmitkCaPTkTrainingView()
  : m_Parent(nullptr)
{
  m_CaPTkTraining = new captk::Training(this);
}

QmitkCaPTkTrainingView::~QmitkCaPTkTrainingView()
{

}

void QmitkCaPTkTrainingView::CreateQtPartControl(QWidget *parent)
{
  // setup the basic GUI of this view
  m_Parent = parent;

  m_Controls.setupUi(parent);

  m_CaPTkTraining->SetProgressBar(m_Controls.progressBar);

  /**** Initialize widgets ****/

  // Hide progress bar until it's implemented
  // TODO: Implement progress updates are remove the hiding below
  m_Controls.progressBar->setVisible(false);

  // Initialize kernel combo box
  m_Controls.comboBox_kernel->addItems(QStringList() << "SVM: Linear" << "SVM: RBF");
  // Set combo box to the last user selected value
  m_Controls.comboBox_kernel->setCurrentText(
    this->GetPreferences()->Get("TrainingKernelComboBox", "SVM: Linear")
  );

  // Initialize kernel combo box
  m_Controls.comboBox_configuration->addItems(
    QStringList() << "Cross-validation" << "Split Train/Test" 
                  << "Train" << "Test"
  );
  // Set combo box to the last user selected value
  auto prefConfigurationText = this->GetPreferences()->Get(
    "TrainingConfigurationComboBox", "Cross-validation"
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

void QmitkCaPTkTrainingView::Activated()
{

}

void QmitkCaPTkTrainingView::Deactivated()
{
  // Not yet implemented
}

void QmitkCaPTkTrainingView::Visible()
{
  // Not yet implemented
}

void QmitkCaPTkTrainingView::Hidden()
{
  // Not yet implemented
}

int QmitkCaPTkTrainingView::GetSizeFlags(bool width)
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

int QmitkCaPTkTrainingView::ComputePreferredSize(bool width,
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
void QmitkCaPTkTrainingView::OnKernelComboBoxCurrentTextChanged(const QString& text)
{
  // Remember it
  this->GetPreferences()->Put("TrainingKernelComboBox", text);
  this->GetPreferences()->Flush();
}

void QmitkCaPTkTrainingView::OnConfigurationComboBoxCurrentTextChanged(const QString& text)
{
  // Remember it
  this->GetPreferences()->Put("TrainingConfigurationComboBox", text);
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
  else if (text == "Train")
  {
    m_Controls.lineEdit_folds->setVisible(false);
    m_Controls.label_folds->setVisible(false);
    m_Controls.lineEdit_samples->setVisible(false);
    m_Controls.label_samples->setVisible(false);
    m_Controls.lineEdit_modeldir->setVisible(false);
    m_Controls.pushButton_modeldir->setVisible(false);
  }
  else if (text == "Test")
  {
    m_Controls.lineEdit_folds->setVisible(false);
    m_Controls.label_folds->setVisible(false);
    m_Controls.lineEdit_samples->setVisible(false);
    m_Controls.label_samples->setVisible(false);
    m_Controls.lineEdit_modeldir->setVisible(true);
    m_Controls.pushButton_modeldir->setVisible(true);
  }
}

void QmitkCaPTkTrainingView::OnFeaturesCsvButtonClicked()
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

void QmitkCaPTkTrainingView::OnResponsesCsvButtonClicked()
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

void QmitkCaPTkTrainingView::OnModelDirectoryButtonClicked()
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

void QmitkCaPTkTrainingView::OnOutputDirectoryButtonClicked()
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

void QmitkCaPTkTrainingView::OnRunButtonPressed()
{
  QString featuresCsvPath = m_Controls.lineEdit_features->text();
  QString responsesCsvPath = m_Controls.lineEdit_responses->text();
  QString classificationKernelStr = m_Controls.comboBox_kernel->currentText();
  QString configurationStr = m_Controls.comboBox_configuration->currentText();
  QString folds   = m_Controls.lineEdit_folds->text();
  QString samples = m_Controls.lineEdit_samples->text();
  QString modelDirPath = m_Controls.lineEdit_modeldir->text();
  QString outputDirPath = m_Controls.lineEdit_outputdir->text();

  m_CaPTkTraining->Run(
    featuresCsvPath,
    responsesCsvPath,
    classificationKernelStr,
    configurationStr,
    folds,
    samples,
    modelDirPath,
    outputDirPath
  );
}

/************************************************************************/
/* protected                                                            */
/************************************************************************/
void QmitkCaPTkTrainingView::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList<mitk::DataNode::Pointer> &)
{

}

void QmitkCaPTkTrainingView::OnPreferencesChanged(const berry::IBerryPreferences*)
{

}

void QmitkCaPTkTrainingView::NodeAdded(const mitk::DataNode *)
{

}

void QmitkCaPTkTrainingView::NodeRemoved(const mitk::DataNode *)
{

}

void QmitkCaPTkTrainingView::SetFocus()
{
}

void QmitkCaPTkTrainingView::UpdateControls()
{
  // Hide views that are not useful
  // m_Controls.label_PatientImage->setVisible(false);

  this->RequestRenderWindowUpdate(mitk::RenderingManager::REQUEST_UPDATE_ALL);
}

void QmitkCaPTkTrainingView::InitializeListeners()
{

}

QString QmitkCaPTkTrainingView::GetLastFileOpenPath()
{
  return this->GetPreferences()->Get("LastFileOpenPath", "");
}

void QmitkCaPTkTrainingView::SetLastFileOpenPath(const QString &path)
{
  this->GetPreferences()->Put("LastFileOpenPath", path);
  this->GetPreferences()->Flush();
}