if(CPACK_GENERATOR MATCHES "NSIS")

  # set the package header icon for MUI
  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_LIST_DIR}\\icons\\icon.ico")
  # set the install/unistall icon used for the installer itself
  # There is a bug in NSIS that does not handle full unix paths properly.
  SET(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_LIST_DIR}\\icons\\icon.ico")
  SET(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_LIST_DIR}\\icons\\icon.ico")

  set(CPACK_NSIS_DISPLAY_NAME "InciSe")

  # tell cpack to create links to the doc files
  SET(CPACK_NSIS_MENU_LINKS
    "https://www.med.upenn.cbica/captk/" "CaPTk website - CBICA"
    )

  # tell cpack the executables you want in the start menu as links
  SET(CPACK_PACKAGE_EXECUTABLES "MitkInciSe;Start InciSe" CACHE INTERNAL "Collecting windows shortcuts to executables")

  # tell cpack to create a desktop link to mitkDiffusion
  SET(CPACK_CREATE_DESKTOP_LINKS "InciSe")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\icon.ico")
  SET(CPACK_NSIS_HELP_LINK "https:\\\\www.med.upenn.edu\\cbica\\captk")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\www.mitk.org")
  SET(CPACK_NSIS_CONTACT software@cbica.upenn.edu)
  SET(CPACK_NSIS_MODIFY_PATH ON)

endif(CPACK_GENERATOR MATCHES "NSIS")