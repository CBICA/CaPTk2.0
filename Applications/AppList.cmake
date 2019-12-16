#option(MITK_BUILD_APP_Diffusion "Build the MITK Diffusion executable" OFF)

#set(MITK_APPS
#  Diffusion^^MITK_BUILD_APP_Diffusion^^MitkDiffusion
#)


# This file is included in the top-level MITK CMakeLists.txt file to
# allow early dependency checking

option(MITK_BUILD_APP_CaptkApp "Build the MITK CaptkApp" ON)

# This variable is fed to ctkFunctionSetupPlugins() macro in the
# top-level MITK CMakeLists.txt file. This allows to automatically
# enable required plug-in runtime dependencies for applications using
# the CTK DGraph executable and the ctkMacroValidateBuildOptions macro.
# For this to work, directories containing executables must contain
# a CMakeLists.txt file containing a "project(...)" command and a
# target_libraries.cmake file setting a list named "target_libraries"
# with required plug-in target names.

# Format is "Directory Name^^CMake Option Name^^Executable Name (without file suffix)"
set(MITK_APPS
  CaptkApp^^MITK_BUILD_APP_CaptkApp^^MitkCaptkApp
  #Workbench^^MITK_BUILD_APP_Workbench^^MitkWorkbench
)

