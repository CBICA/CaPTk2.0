set(SRC_CPP_FILES
  
)

set(INTERNAL_CPP_FILES
  mitkPluginActivator.cpp
  QmitkCaPTkInteractiveSegmentationView.cpp
  Common/QmitkDataSelectionWidget.cpp
)

set(UI_FILES
  src/internal/QmitkCaPTkInteractiveSegmentationControls.ui
  src/internal/Common/QmitkDataSelectionWidgetControls.ui
)

set(MOC_H_FILES
  src/internal/mitkPluginActivator.h
  src/internal/QmitkCaPTkInteractiveSegmentationView.h
  src/internal/Common/QmitkDataSelectionWidget.h
)

set(CACHED_RESOURCE_FILES
  resources/NewSegmentationSession_48x48.png
  resources/mll_icon2.svg
  plugin.xml
)

set(QRC_FILES
  resources/captkinteractivesegmentation.qrc
)

set(CPP_FILES)

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

#usFunctionEmbedResources(
#CPP_FILES
# LIBRARY_NAME "liborg_mitk_gui_qt_multilabelsegmentation"
#ROOT_DIR resources
#FILES Interactions/SegmentationInteraction.xml
#      Interactions/ConfigSegmentation.xml
#)

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
