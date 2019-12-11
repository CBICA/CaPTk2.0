# ### Source files need to be relative to src/
# ### Header files need to be relative to module_root

# Function to prepend a string to all list items
FUNCTION( MAKE_LIST_OF_PATHS_RELATIVE_TO _listname _list _dir )
    unset( _tmp )
    foreach( item ${_list} )
        file(RELATIVE_PATH _tmpv ${_dir} ${item})
        list(APPEND _tmp "${_tmpv}")
    endforeach( item ${_list} )
    set( ${_listname} ${_tmp} PARENT_SCOPE )
ENDFUNCTION( MAKE_LIST_OF_PATHS_RELATIVE_TO )

# Find the source files and headers
FILE(GLOB_RECURSE ALL_FILES "${CMAKE_CURRENT_LIST_DIR}/files/*")
FILE(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/files/*.h")
MAKE_LIST_OF_PATHS_RELATIVE_TO(SOURCES "${SOURCES}" "${CMAKE_CURRENT_LIST_DIR}")
MAKE_LIST_OF_PATHS_RELATIVE_TO(HEADERS "${HEADERS}" "${CMAKE_CURRENT_LIST_DIR}")

# message(stuff)
# message("${SOURCES}")
# message("${HEADERS}")

### Set the files to the required variables normally
#list(APPEND all_files ${HEADERS})
#message(${all_files})

set(CPP_FILES
  ${ALL_FILES}
  # "${SOURCES}"
  # "${HEADERS}"
)
message("cppfiles: ${CPP_FILES}")
set(UI_FILES

)

set(MOC_H_FILES
 "${HEADERS}"
)
message("cppfiles: ${MOC_H_FILES}")
set(RESOURCE_FILES

)
