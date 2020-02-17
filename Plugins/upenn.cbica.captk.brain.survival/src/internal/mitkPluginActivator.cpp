#include "mitkPluginActivator.h"

#include "QmitkCaPTkSurvivalView.h"
// #include "SegmentationUtilities/QmitkMultiLabelSegmentationUtilitiesView.h"

#include <usModuleInitialization.h>

ctkPluginContext* mitk::PluginActivator::m_Context = nullptr;

US_INITIALIZE_MODULE //("CaPTkSurvival", "libupenn_cbica_captk_brain_survival")

void mitk::PluginActivator::start(ctkPluginContext *context)
{
  BERRY_REGISTER_EXTENSION_CLASS(QmitkCaPTkSurvivalView, context)
  // BERRY_REGISTER_EXTENSION_CLASS(QmitkMultiLabelSegmentationPreferencePage, context)
  // BERRY_REGISTER_EXTENSION_CLASS(QmitkMultiLabelSegmentationUtilitiesView, context)

  m_Context = context;
}

void mitk::PluginActivator::stop(ctkPluginContext*)
{
}

ctkPluginContext* mitk::PluginActivator::getContext()
{
  return m_Context;
}
