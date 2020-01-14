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

#include "mitkPluginActivator.h"

#include "QmitkCaPTkTrainingPluginView.h"

#include <usModuleInitialization.h>

ctkPluginContext* mitk::PluginActivator::m_Context = nullptr;

//MLI TODO
US_INITIALIZE_MODULE //("TrainingPlugin", "liborg_mitk_cbica_captk_trainingplugin")

void mitk::PluginActivator::start(ctkPluginContext *context)
{
  BERRY_REGISTER_EXTENSION_CLASS(QmitkCaPTkTrainingPluginView, context)

  m_Context = context;
}

void mitk::PluginActivator::stop(ctkPluginContext*)
{
}

ctkPluginContext* mitk::PluginActivator::getContext()
{
  return m_Context;
}
