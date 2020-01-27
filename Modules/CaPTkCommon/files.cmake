### Source files need to be relative to src/
### Header files need to be relative to the root of the module

### Fetch the source files (and make them be relative to src/)
file(GLOB_RECURSE SOURCE_FILES RELATIVE 
  "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/src/*"
)
### Fetch the header files (and make them be relative to module root)
file(GLOB_RECURSE HEADER_FILES RELATIVE 
  "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*"
)

### Set the files to the required variables normally

set(CPP_FILES
  "${SOURCE_FILES}"
)

set(UI_FILES

)

set(MOC_H_FILES
  "${HEADER_FILES}"
)

set(H_FILES
  "${HEADER_FILES}"
)

set(RESOURCE_FILES

)
