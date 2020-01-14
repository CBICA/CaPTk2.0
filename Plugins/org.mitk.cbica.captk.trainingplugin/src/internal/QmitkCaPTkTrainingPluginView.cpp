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

  // TODO: Initialize combo box values

  //  this->InitializeListeners();

  // TODO: Connect signals/slots

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