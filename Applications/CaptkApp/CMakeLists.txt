project(CaptkApp)

set(_app_options)
if(MITK_SHOW_CONSOLE_WINDOW)
  list(APPEND _app_options SHOW_CONSOLE)
endif()

# Create a cache entry for the provisioning file which is used to export
# the file name in the MITKConfig.cmake file. This will keep external projects
# which rely on this file happy.
set(MITK_CaptkApp_PROVISIONING_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MitkCaptkApp.provisioning" CACHE INTERNAL "CaptkApp provisioning file" FORCE)

set(_plugins
  org.mitk.gui.qt.coreapplication
  org.mitk.gui.qt.stdmultiwidgeteditor
  org.mitk.gui.qt.datamanager
  org_mitk_planarfigure
)

#set(_exclude_plugins
#  org.blueberry.test
#  org.blueberry.uitest
#  org.mitk.gui.qt.coreapplication
#  org.mitk.gui.qt.diffusionimagingapp
#  org.mitk.example.gui.customviewer
#  org.mitk.example.gui.customviewer.views
#  org.mitk.example.gui.selectionservicemitk
#  org.mitk.example.gui.selectionservicemitk.views
#  org.mitk.example.gui.selectionserviceqt
#  org.mitk.example.gui.extensionpointcontribution
#  org.mitk.example.gui.extensionpointdefinition
#  org.mitk.example.gui.minimalapplication
#  org.mitk.example.gui.multipleperspectives
#)

# Plug-ins listed below will not be
#  - added as a build-time dependency to the executable
#  - listed in the provisioning file for the executable
#  - installed if they are external plug-ins

set(_exclude_plugins )

mitkFunctionCreateBlueBerryApplication(
  NAME MitkCaptkApp
  DESCRIPTION "MITK - CaptkApp Application"
  PLUGINS ${_plugins}
  ${_app_options}
)

mitk_use_modules(TARGET MitkCaptkApp
  MODULES MitkAppUtil
)

# subproject support
add_dependencies(MITK-CoreUI MitkCaptkApp)
