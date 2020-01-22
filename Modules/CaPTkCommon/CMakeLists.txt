include_directories(
	src/internal
	third_party/jsoncpp/include
	third_party/jsoncpp/src
)

mitk_create_module(
  INCLUDE_DIRS include src/internal
  #  PUBLIC include third_party/jsoncpp/include
  #  PRIVATE src third_party/jsoncpp/src
  PACKAGE_DEPENDS ITK Qt5|Core #VTK OpenCV
  DEPENDS MitkMultilabel #MitkCore
)

add_subdirectory(cmdapps)
