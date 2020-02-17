project(upenn_cbica_captk_interactivesegmentation)

include_directories(${CTK_INCLUDE_DIRS})

mitk_create_plugin(
  EXPORT_DIRECTIVE MITK_CAPTK_INTERACTIVE_SEGMENTATION
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS ITK OpenCV
  MODULE_DEPENDS MitkSegmentation MitkSegmentationUI MitkCaPTkInteractiveSegmentation
)