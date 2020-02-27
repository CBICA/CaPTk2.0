set(CaPTkData_DIR ${MITK_SUPERBUILD_BINARY_DIR}/ep/src/CaPTkData)

if(EXISTS ${CaPTkData_DIR})
  set(CaPTkData_FOUND TRUE)
else()
  set(CaPTkData_FOUND FALSE)
endif()