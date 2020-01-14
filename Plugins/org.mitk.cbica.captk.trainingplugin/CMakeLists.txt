project(org_mitk_cbica_captk_trainingplugin)

include_directories(${CTK_INCLUDE_DIRS})

mitk_create_plugin(
  EXPORT_DIRECTIVE MITK_CBICA_CAPTK_TRAININGPLUGIN
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS ITK OpenCV
  #MODULE_DEPENDS MitkCaPTkCommon ### TODO: Uncomment this when implemented
)