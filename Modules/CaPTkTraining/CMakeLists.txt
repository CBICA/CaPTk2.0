mitk_create_module(
  PACKAGE_DEPENDS ITK VTK Qt5|Core+Widgets
  DEPENDS PUBLIC MitkCaPTkCommon
)

add_subdirectory(cmdapps)
