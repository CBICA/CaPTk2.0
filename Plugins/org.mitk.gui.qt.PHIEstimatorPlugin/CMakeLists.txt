project(org_mitk_gui_qt_PHIEstimatorPlugin)

mitk_create_plugin(
  #DEPENDS MitkCore
  EXPORT_DIRECTIVE EXAMPLE_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS PRIVATE MitkQtWidgetsExt MitkPHIEstimatorModule
  PACKAGE_DEPENDS PUBLIC ITK|ITKIOImageBase+ITKIONIFTI
  )
