set(SRC_CPP_FILES
)

set(INTERNAL_CPP_FILES
  QmitkCaPTkApplication.cpp
  QmitkCaPTkApplicationPlugin.cpp
  QmitkCaPTkAppWorkbenchAdvisor.cpp
  QmitkCaPTkIntroPart.cpp
  perspectives/QmitkCaPTkEditorPerspective.cpp
  perspectives/QmitkCaPTkDefaultPerspective.cpp
  perspectives/QmitkCaPTkVisualizationPerspective.cpp
)

set(MOC_H_FILES
  src/internal/QmitkCaPTkApplication.h
  src/internal/QmitkCaPTkApplicationPlugin.h
  src/internal/QmitkCaPTkIntroPart.h
  src/internal/perspectives/QmitkCaPTkEditorPerspective.h
  src/internal/perspectives/QmitkCaPTkDefaultPerspective.h
  src/internal/perspectives/QmitkCaPTkVisualizationPerspective.h
)

set(UI_FILES
  src/internal/perspectives/QmitkCaPTkWelcomeScreenViewControls.ui
)

set(CACHED_RESOURCE_FILES
# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
  plugin.xml
  resources/icon_research.xpm
  resources/perspectives/eye.png
  resources/perspectives/viewer_icon.png
  resources/cbica-logo.jpg
)

set(QRC_FILES
resources/QmitkCaPTkApplication.qrc
resources/welcome/QmitkCaPTkWelcomeScreenView.qrc
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})

