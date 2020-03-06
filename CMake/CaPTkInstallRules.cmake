# Install CaPTk ico

if( ${MITK_BUILD_APP_CaPTkApp} )
  get_filename_component(ABSOLUTE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/../Applications/CaPTkApp/icons/icon.ico"
	ABSOLUTE
  )
endif( ${MITK_BUILD_APP_CaPTkApp} )

if( ${MITK_BUILD_APP_InciSe} )
  get_filename_component(ABSOLUTE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/../Applications/InciSe/icons/icon.ico"
	ABSOLUTE
  )
endif( ${MITK_BUILD_APP_InciSe} )

message("Path is ${ABSOLUTE_PATH}")
MITK_INSTALL(FILES ${ABSOLUTE_PATH})