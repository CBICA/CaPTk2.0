mitk_create_module(
  PACKAGE_DEPENDS ITK VTK Qt5|Core+Widgets
  DEPENDS PUBLIC MitkCaPTkCommon MitkCaPTkLegacyCommon
)

add_subdirectory(cmdapps)
