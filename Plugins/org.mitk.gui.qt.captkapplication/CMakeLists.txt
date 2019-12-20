project(org_mitk_gui_qt_captkapplication)

mitk_create_plugin(
  EXPORT_DIRECTIVE MITK_QT_CAPTKAPP
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS Qt5|WebEngineWidgets
)
