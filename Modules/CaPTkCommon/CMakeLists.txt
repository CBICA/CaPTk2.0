include_directories(
	src
)

mitk_create_module(
  PACKAGE_DEPENDS ITK Qt5|Core
  DEPENDS MitkMultilabel
)

add_subdirectory(cmdapps)
