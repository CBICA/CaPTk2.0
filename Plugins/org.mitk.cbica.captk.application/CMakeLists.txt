project(org_mitk_cbica_captk_application)

mitk_create_plugin(
  EXPORT_DIRECTIVE MITK_QT_CAPTKAPP
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS Qt5|WebEngineWidgets
)
