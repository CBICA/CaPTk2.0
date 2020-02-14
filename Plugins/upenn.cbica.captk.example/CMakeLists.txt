project(upenn_cbica_captk_example)

include_directories(${CTK_INCLUDE_DIRS})

mitk_create_plugin(
  EXPORT_DIRECTIVE MITK_CAPTK_EXAMPLE
  EXPORTED_INCLUDE_SUFFIXES src
  ### PACKAGE_DEPENDS ITK OpenCV # -> External dependencies
  MODULE_DEPENDS MitkCaPTkExample MitkCore
)