# Macro to prepend a string to all list items
MACRO( PREPEND _list _prefix )
    unset( _tmp )
    foreach( item ${_list} )
        set( _tmp ${_tmp} ${_prefix}${item} )
    endforeach( item ${_list} )
    set( _list ${_tmp} )
ENDMACRO( PREPEND _list )

# Find the normal source files and headers
FILE(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.*")
FILE(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*.*")
PREPEND(HEADERS "include/")

# Find all source and header files for cbica_toolkit
FILE(GLOB_RECURSE CBICA_TK_SOURCES "${CMAKE_CURRENT_LIST_DIR}/cbica_toolkit/src/*.*")
FILE(GLOB_RECURSE CBICA_TK_HEADERS "${CMAKE_CURRENT_LIST_DIR}/cbica_toolkit/include/*.*")
#PREPEND(CBICA_TK_SOURCES "../cbica_toolkit/src")
#PREPEND(CBICA_TK_HEADERS "cbica_toolkit/include/")

# Find all source and header files for yaml_cpp
FILE(GLOB_RECURSE YAML_SOURCES "${CMAKE_CURRENT_LIST_DIR}/yaml-cpp/src/*.*")
FILE(GLOB_RECURSE YAML_HEADERS "${CMAKE_CURRENT_LIST_DIR}/yaml-cpp/include/*.*")
PREPEND(${YAML_SOURCES} "../yaml-cpp/src")
PREPEND(${YAML_HEADERS} "yaml-cpp/include/yaml-cpp/")

message(hi)
message(${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_CURRENT_LIST_DIR})
message("${YAML_SOURCES}")
message("${YAML_HEADERS}")
message("${CBICA_TK_SOURCES}")
message("${CBICA_TK_HEADERS}")

set(CPP_FILES
  ${SOURCES}
  ${CBICA_TK_SOURCES}
  ${YAML_SOURCES}
)

set(UI_FILES

)

set(MOC_H_FILES
  ${HEADERS}
  ${CBICA_TK_HEADERS}
  ${YAML_HEADERS}
)

set(RESOURCE_FILES

)
