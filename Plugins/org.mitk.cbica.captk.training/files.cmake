set(SRC_CPP_FILES
  
)

set(INTERNAL_CPP_FILES
  mitkPluginActivator.cpp
  QmitkCaPTkTrainingView.cpp
)

set(UI_FILES
  src/internal/QmitkCaPTkTrainingControls.ui
)

set(MOC_H_FILES
  src/internal/mitkPluginActivator.h
  src/internal/QmitkCaPTkTrainingView.h
)

set(CACHED_RESOURCE_FILES
  resources/training_plugin.svg
  plugin.xml
)

set(QRC_FILES

)

set(CPP_FILES)
foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

#usFunctionEmbedResources(
#CPP_FILES
# LIBRARY_NAME "liborg_mitk_cbica_captk_trainingmodule"
#ROOT_DIR resources
#FILES Interactions/SegmentationInteraction.xml
#      Interactions/ConfigSegmentation.xml
#)

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
