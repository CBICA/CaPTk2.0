set(CPP_FILES
  src/internal/PluginActivator.cpp
  src/internal/QmitkPHIEstimatorView.cpp
)

set(UI_FILES
  src/internal/PHIEstimatorViewControls.ui
)

set(MOC_H_FILES
  src/internal/PluginActivator.h
  src/internal/QmitkPHIEstimatorView.h
)

# List of resource files that can be used by the plugin system without loading
# the actual plugin. For example, the icon that is typically displayed in the
# plugin view menu at the top of the application window.
set(CACHED_RESOURCE_FILES
  resources/icon.png
  plugin.xml
)

set(QRC_FILES
)
