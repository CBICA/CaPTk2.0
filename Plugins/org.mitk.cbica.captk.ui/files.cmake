set(SRC_CPP_FILES
  QcaptkWorkbenchWindowAdvisor.cpp
)

set(INTERNAL_CPP_FILES
  QcaptkAboutHandler.cpp
)

set(UI_FILES

)

set(MOC_H_FILES
  src/QcaptkWorkbenchWindowAdvisor.h
  src/internal/QcaptkAboutHandler.h
  src/internal/QcaptkWorkbenchWindowAdvisorHack.h
)

set(CACHED_RESOURCE_FILES
# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
  plugin.xml
)

set(QRC_FILES
# uncomment the following line if you want to use Qt resources
 resources/org_mitk_gui_qt_ext.qrc
 resources/org_mitk_icons.qrc
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
