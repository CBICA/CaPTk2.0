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

#include "QmitkCaPTkApplicationPlugin.h"
#include "perspectives/QmitkCaPTkDefaultPerspective.h"
#include "perspectives/QmitkCaPTkEditorPerspective.h"
#include "perspectives/QmitkCaPTkVisualizationPerspective.h"
#include "QmitkCaPTkIntroPart.h"
#include "QmitkCaPTkApplication.h"

#include "mitkWorkbenchUtil.h"
#include <mitkVersion.h>
#include <mitkLogMacros.h>

#include <service/cm/ctkConfigurationAdmin.h>
#include <service/cm/ctkConfiguration.h>

#include <QFileInfo>
#include <QDateTime>

QmitkCaPTkApplicationPlugin* QmitkCaPTkApplicationPlugin::inst = nullptr;

QmitkCaPTkApplicationPlugin::QmitkCaPTkApplicationPlugin()
{
  inst = this;
}

QmitkCaPTkApplicationPlugin::~QmitkCaPTkApplicationPlugin()
{
}

QmitkCaPTkApplicationPlugin* QmitkCaPTkApplicationPlugin::GetDefault()
{
  return inst;
}

void QmitkCaPTkApplicationPlugin::start(ctkPluginContext* context)
{
  berry::AbstractUICTKPlugin::start(context);

  this->context = context;

  BERRY_REGISTER_EXTENSION_CLASS(QmitkCaPTkDefaultPerspective, context);
  BERRY_REGISTER_EXTENSION_CLASS(QmitkCaPTkEditorPerspective, context);
  BERRY_REGISTER_EXTENSION_CLASS(QmitkCaPTkIntroPart, context);
  BERRY_REGISTER_EXTENSION_CLASS(QmitkCaPTkApplication, context);
  BERRY_REGISTER_EXTENSION_CLASS(QmitkCaPTkVisualizationPerspective, context);


  ctkServiceReference cmRef = context->getServiceReference<ctkConfigurationAdmin>();
  ctkConfigurationAdmin* configAdmin = nullptr;
  if (cmRef)
  {
    configAdmin = context->getService<ctkConfigurationAdmin>(cmRef);
  }

  // Use the CTK Configuration Admin service to configure the BlueBerry help system
  if (configAdmin)
  {
    ctkConfigurationPtr conf = configAdmin->getConfiguration("org.blueberry.services.help", QString());
    ctkDictionary helpProps;
    helpProps.insert("homePage", "qthelp://org.mitk.cbica.captk.application/bundle/index.html");
    conf->update(helpProps);
    context->ungetService(cmRef);
  }
  else
  {
    MITK_WARN << "Configuration Admin service unavailable, cannot set home page url.";
  }

  // Set CBICA logo
  mitk::WorkbenchUtil::SetDepartmentLogoPreference(":/org.mitk.cbica.captk.application/cbica-logo.jpg", context);
}

ctkPluginContext* QmitkCaPTkApplicationPlugin::GetPluginContext() const
{
  return context;
}
