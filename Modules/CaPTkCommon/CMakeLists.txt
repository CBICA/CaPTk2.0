include_directories(
	src/internal
	third_party/jsoncpp/include
	third_party/jsoncpp/src
)

mitk_create_module(
  #INCLUDE_DIRS 
  #  PUBLIC include third_party/jsoncpp/include
  #  PRIVATE src third_party/jsoncpp/src
  PACKAGE_DEPENDS ITK Qt5|Core #VTK OpenCV
  #DEPENDS MitkCore
)

add_subdirectory(cmdapps)
