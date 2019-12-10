# ### Source files need to be relative to src/
# ### Header files need to be relative to module_root

# Function to prepend a string to all list items
FUNCTION( MAKE_LIST_OF_PATHS_RELATIVE_TO _listname _list _dir )
    unset( _tmp )
    foreach( item ${_list} )
        message("item ${item}")
        file(RELATIVE_PATH _tmpv ${_dir} ${item})
        list(APPEND _tmp "${_tmpv}")
    endforeach( item ${_list} )
    set( ${_listname} ${_tmp} PARENT_SCOPE )
ENDFUNCTION( MAKE_LIST_OF_PATHS_RELATIVE_TO )

# Find the source files and headers
FILE(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*")
FILE(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*")
MAKE_LIST_OF_PATHS_RELATIVE_TO(SOURCES "${SOURCES}" "${CMAKE_CURRENT_LIST_DIR}/src")
MAKE_LIST_OF_PATHS_RELATIVE_TO(HEADERS "${HEADERS}" "${CMAKE_CURRENT_LIST_DIR}")


### Set the files to the required variables normally

set(CPP_FILES
  "${SOURCES}"
)

set(UI_FILES

)

set(MOC_H_FILES
  "${HEADERS}"
)

set(RESOURCE_FILES

)
