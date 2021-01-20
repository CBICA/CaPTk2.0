set(SRC_CPP_FILES

)

set(INTERNAL_CPP_FILES
  QmitkCaPTkSplitLabelsToImagesAction.cpp
  mitkPluginActivator.cpp
)

set(UI_FILES

)

set(MOC_H_FILES
  src/internal/QmitkCaPTkSplitLabelsToImagesAction.h
  src/internal/mitkPluginActivator.h
)

set(CACHED_RESOURCE_FILES
  plugin.xml
)

set(QRC_FILES

)

set(CPP_FILES)

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
