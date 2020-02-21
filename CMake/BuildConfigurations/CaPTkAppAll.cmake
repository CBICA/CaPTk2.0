### Inspired by https://github.com/MIC-DKFZ/MITK-Diffusion/blob/master/CMake/BuildConfigurations/DiffusionAll.cmake
### Basically when selected, the following will turn on automatically

message(STATUS "Configuring CaPTk default optional dependencies")

# Enable non-optional external dependencies
set(MITK_USE_Vigra ON CACHE BOOL "MITK Use Vigra Library" FORCE)
set(MITK_USE_MatchPoint ON CACHE BOOL "" FORCE)
set(MITK_USE_OpenMP ON CACHE BOOL "" FORCE)
set(MITK_USE_OpenCV ON CACHE BOOL "" FORCE)
set(MITK_USE_Python3 ON CACHE BOOL "" FORCE)

# Enable/disable non-superbuild apps
set(MITK_BUILD_APP_CaPTkApp ON CACHE BOOL "Build the MITK CaPTkApp" FORCE)
set(MITK_BUILD_APP_InciSe OFF CACHE BOOL "Build the MITK InciSe executable" FORCE)
set(MITK_BUILD_APP_Workbench OFF CACHE BOOL "Build the MITK Workbench executable" FORCE)

# Enable/disable non-superbuild plugins
set(MITK_BUILD_org.mitk.gui.qt.measurementtoolbox ON CACHE BOOL "Build the org.mitk.gui.qt.measurementtoolbox Plugin." FORCE)
set(MITK_BUILD_org.mitk.gui.qt.segmentation ON CACHE BOOL "Build the org.mitk.gui.qt.segmentation Plugin." FORCE)
set(MITK_BUILD_org.mitk.gui.qt.multilabelsegmentation ON CACHE BOOL "Build the org.mitk.gui.qt.multilabelsegmentation Plugin." FORCE)

# # Build neither all plugins nor examples
# set(BUILD_TESTING ON CACHE BOOL "Build the MITK tests" FORCE)

# Activate in-application help generation
set(MITK_DOXYGEN_GENERATE_QCH_FILES ON CACHE BOOL "Use doxygen to generate Qt compressed help files for MITK docs" FORCE)
set(BLUEBERRY_USE_QT_HELP ON CACHE BOOL "Enable support for integrating bundle documentation into Qt Help" FORCE)

# Disable console window
set(MITK_SHOW_CONSOLE_WINDOW OFF CACHE BOOL "Use this to enable or disable the console window when starting MITK GUI Applications" FORCE)

# Enable exporting of compile commands (useful for intellisense in vscode etc)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Enable/Disable output of compile commands during generation." FORCE)
