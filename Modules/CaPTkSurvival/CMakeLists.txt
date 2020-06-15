mitk_create_module(
  INCLUDE_DIRS PUBLIC include
  PACKAGE_DEPENDS ITK VTK Qt5|Core+Widgets OpenCV
  DEPENDS PUBLIC MitkCaPTkLegacyCommon MitkCore
)

# ---- Model deploying ----
# standard cmake way of copying files -- doesn't copy into the packaged install, but into the build directory
file(COPY resources/models/survival_model DESTINATION ${CMAKE_BINARY_DIR}/bin/models)
# the below macro does the work necessary to install the models into the packaged executable
MITK_INSTALL( DIRECTORY resources/models)
# -------------------------


add_subdirectory(cmdapps)
