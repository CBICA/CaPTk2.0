mitk_create_module(
  PACKAGE_DEPENDS ITK Qt5|Core # -> How to link to external dependencies
  DEPENDS MitkCore # -> How to use other modules
)

add_subdirectory(cmdapps) # -> How to add the CLI program

# Remember to add the module to Modules/ModuleList.cmake
# Also remember to add your files to MODULE_NAME/files.cmake 