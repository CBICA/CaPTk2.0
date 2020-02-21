project(upenn_cbica_captk_common)

include_directories(${CTK_INCLUDE_DIRS})

mitk_create_plugin(
  EXPORT_DIRECTIVE UPENN_CBICA_CAPTK_COMMON_EXPORTS
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkSegmentation MitkCaPTkCommon
)
