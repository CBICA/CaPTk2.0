project(org_mitk_gui_qt_captkinteractivesegmentation)

include_directories(${CTK_INCLUDE_DIRS})

message("Creating plugin CaPTkInteractiveSegmentation")

mitk_create_plugin(
  EXPORT_DIRECTIVE MITK_QT_SEGMENTATION
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS ITK OpenCV
  MODULE_DEPENDS MitkSegmentation MitkSegmentationUI MitkCaPTkInteractiveSegmentationModule
)