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

#include "QmitkCaPTkAppWorkbenchAdvisor.h"
#include "internal/QmitkCaPTkApplicationPlugin.h"

#include <QmitkExtWorkbenchWindowAdvisor.h>
#include <src/internal/QmitkCaPTkApplicationPlugin.h>

#include <QMessageBox>
#include <QMainWindow>
#include <QMenuBar>

const QString QmitkCaPTkAppWorkbenchAdvisor::DEFAULT_PERSPECTIVE_ID =
    "org.mitk.captkapp.defaultperspective";

void
QmitkCaPTkAppWorkbenchAdvisor::Initialize(berry::IWorkbenchConfigurer::Pointer configurer)
{
  berry::QtWorkbenchAdvisor::Initialize(configurer);

  configurer->SetSaveAndRestore(true);

  // // Change the about page to CaPTk's
  // QMainWindow* mainWindow =
  //   qobject_cast<QMainWindow*>(m_WorkbenchWindowConfigurer->GetWindow()->GetShell()->GetControl());
  // QList<QMenu*> menus = mainWindow->menuBar()->findChildren<QMenu*>();
  // for (QMenu* menu : menus)
  // {
  //   for (QAction* action : menu->actions())
  //   {
  //     if (action->text() != nullptr && action->text() == "About")
  //     {
  //       // menu->removeAction(action);
  //       // QAction* newAboutAction = new QAction("About", menu);
  //       // newAboutAction
  //       menu->addAction("About", 
  //         [this]()
  //         {
  //           QMessageBox msgError;
  //           msgError.setText(
  //               "UPenn"
  //           );
  //           // msgError.setIcon(QMessageBox::Critical);
  //           msgError.setWindowTitle("CaPTk");
  //           msgError.exec();
  //         }
  //       );
  //       break;
  //     }
  //   }
  // }
}

berry::WorkbenchWindowAdvisor*
QmitkCaPTkAppWorkbenchAdvisor::CreateWorkbenchWindowAdvisor(
        berry::IWorkbenchWindowConfigurer::Pointer configurer)
{
  QmitkExtWorkbenchWindowAdvisor* advisor = new
    QmitkExtWorkbenchWindowAdvisor(this, configurer);

  // Exclude the help perspective from org.blueberry.ui.qt.help from
  // the normal perspective list.
  // The perspective gets a dedicated menu entry in the help menu
  QList<QString> excludePerspectives;
  excludePerspectives.push_back("org.blueberry.perspectives.help");
  advisor->SetPerspectiveExcludeList(excludePerspectives);

  // Exclude some views from the normal view list
  QList<QString> excludeViews;
  excludeViews.push_back("org.mitk.views.modules");
  excludeViews.push_back( "org.blueberry.ui.internal.introview" );
  advisor->SetViewExcludeList(excludeViews);

  advisor->SetWindowIcon(":/org.mitk.cbica.captk.application/icon.png");

  m_WorkbenchWindowConfigurer = configurer; // Used for changing "about" to CaPTk's

  return advisor;
}

QString QmitkCaPTkAppWorkbenchAdvisor::GetInitialWindowPerspectiveId()
{
  return DEFAULT_PERSPECTIVE_ID;
}
