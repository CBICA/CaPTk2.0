mitk_create_module(
  INCLUDE_DIRS PUBLIC include third_party/yaml-cpp third_party/jsoncpp/include
  PACKAGE_DEPENDS ITK Qt5|Core+Widgets
  DEPENDS PUBLIC MitkCore MitkMultilabel
)

add_subdirectory(cmdapps)