project(org_mitk_cbica_captk_ui)

# see bug-19679
set(additional_dependencies "")

if(APPLE)
 set(additional_dependencies Qt5|DBus)
endif()

mitk_create_plugin(
  EXPORT_DIRECTIVE CAPTK_UI_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS
  PRIVATE MitkQtWidgetsExt MitkSceneSerialization MitkAppUtil
  PACKAGE_DEPENDS ${additional_dependencies}
)
