include_directories(
	src
)

mitk_create_module(
  PACKAGE_DEPENDS ITK VTK Qt5|Core
)
