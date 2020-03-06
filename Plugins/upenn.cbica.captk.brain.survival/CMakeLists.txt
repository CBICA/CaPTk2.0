project(upenn_cbica_captk_brain_survival)

#include_directories(${CTK_INCLUDE_DIRS})

mitk_create_plugin(
  EXPORT_DIRECTIVE MITK_CAPTK_SURVIVAL
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS ITK OpenCV
  MODULE_DEPENDS MitkCaPTkSurvival
)
