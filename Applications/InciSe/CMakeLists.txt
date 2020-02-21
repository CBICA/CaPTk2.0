project(InciSe)

set(_app_options)
if(MITK_SHOW_CONSOLE_WINDOW)
  list(APPEND _app_options SHOW_CONSOLE)
endif()

# Create a cache entry for the provisioning file which is used to export
# the file name in the MITKConfig.cmake file. This will keep external projects
# which rely on this file happy.
set(MITK_InciSe_PROVISIONING_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MitkInciSe.provisioning" CACHE INTERNAL "InciSe provisioning file" FORCE)

set(_plugins
 org.mitk.gui.qt.stdmultiwidgeteditor
 org.mitk.gui.qt.datamanager
 org_mitk_planarfigure

 org.mitk.cbica.captk.ui
 org.mitk.cbica.captk.application
 org.mitk.cbica.captk.interactivesegmentation
 org.mitk.gui.qt.imagenavigator
 org_mitk_gui_qt_imagenavigator
 org.mitk.gui.qt.measurementtoolbox
 org.mitk.gui.qt.segmentation
 org.mitk.gui.qt.multilabelsegmentation
)

# Plug-ins listed below will not be
#  - added as a build-time dependency to the executable
#  - listed in the provisioning file for the executable
#  - installed if they are external plug-ins

# set(_exclude_plugins
#   org.blueberry.test
#   org.blueberry.uitest
#   org.mitk.gui.qt.coreapplication
#   org.mitk.gui.qt.diffusionimagingapp
#   org.mitk.example.gui.customviewer
#   org.mitk.example.gui.customviewer.views
#   org.mitk.example.gui.selectionservicemitk
#   org.mitk.example.gui.selectionservicemitk.views
#   org.mitk.example.gui.selectionserviceqt
#   org.mitk.example.gui.extensionpointcontribution
#   org.mitk.example.gui.extensionpointdefinition
#   org.mitk.example.gui.minimalapplication
#   org.mitk.example.gui.multipleperspectives
# )

# For CaPTk icon to be included (this cmake file is in CMake/)
include(CaPTkInstallRules)

# For the git tag to get picked up
include(mitkFunctionGetVersionDescription)
mitkFunctionGetVersionDescription(${PROJECT_SOURCE_DIR} CaPTk)

mitkFunctionCreateBlueBerryApplication(
  NAME MitkInciSe
  DESCRIPTION "InciSe"
  PLUGINS ${_plugins}
  # EXCLUDE_PLUGINS(_exclude_plugins)
  ${_app_options}
)

mitk_use_modules(TARGET MitkInciSe
  MODULES MitkAppUtil
)

# subproject support
#add_dependencies(MITK-CoreUI MitkInciSe)

# Add meta dependencies (e.g. on auto-load modules from depending modules)
if(TARGET ${CMAKE_PROJECT_NAME}-autoload)
  add_dependencies(MitkInciSe ${CMAKE_PROJECT_NAME}-autoload)
endif()

#Setting application icon for macOS systems
#set_target_properties(MitkInciSe PROPERTIES MACOSX_BUNDLE_ICON_FILE "icon.icns")

#if(APPLE)
#  install(FILES "icons/icon.icns" DESTINATION "MitkInciSe.app/Contents/Resources")
#endif(APPLE)

# Add a build time dependency to legacy BlueBerry bundles.
if(MITK_MODULES_ENABLED_PLUGINS)
  add_dependencies(MitkInciSe ${MITK_MODULES_ENABLED_PLUGINS})
endif()

