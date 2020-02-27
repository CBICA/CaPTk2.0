if(MITK_USE_CaPTkData)


# Sanity checks
if(DEFINED CaPTkData_DIR AND NOT EXISTS ${CaPTkData_DIR})
  message(FATAL_ERROR "CaPTkData_DIR variable is defined but corresponds to non-existing directory")
endif()

set(proj CaPTkData)
set(proj_DEPENDENCIES)
set(CaPTkData_DEPENDS ${proj})

if(NOT DEFINED CaPTkData_DIR)

  set(CaPTkData_DIR ${ep_prefix}/src/${proj}/)

  ExternalProject_Add(${proj}
    URL https://github.com/CBICA/CaPTk2.0/releases/download/CaPTk-data/CaPTk-data-20200226.tar.gz
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DEPENDS ${proj_DEPENDENCIES}
  )

  configure_file(${CMAKE_CURRENT_LIST_DIR}/${proj}.h.in ${CMAKE_BINARY_DIR}/${proj}.h)

else()

  mitkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
endif()

endif()
