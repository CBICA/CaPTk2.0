# Install CaPTk ico

if( ${MITK_BUILD_APP_CaPTkApp} )
  MITK_INSTALL(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../Application/CaPTkApp/icons/icon.ico"
  )
endif( ${MITK_BUILD_APP_CaPTkApp} )

if( ${MITK_BUILD_APP_InciSe} )
  MITK_INSTALL(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../Application/InciSe/icons/icon.ico"
  )
endif( ${MITK_BUILD_APP_InciSe} )