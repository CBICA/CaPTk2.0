set(SRC_CPP_FILES
  
)

set(INTERNAL_CPP_FILES
  mitkPluginActivator.cpp
  QmitkCaPTkExampleView.cpp
)

set(UI_FILES
  src/internal/QmitkCaPTkExampleControls.ui
)

set(MOC_H_FILES
  src/internal/mitkPluginActivator.h
  src/internal/QmitkCaPTkExampleView.h
)

set(CACHED_RESOURCE_FILES
  resources/NewSegmentationSession_48x48.png
  resources/icon.svg
  plugin.xml
)

set(QRC_FILES
  resources/captkexample.qrc
)

set(CPP_FILES)

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
