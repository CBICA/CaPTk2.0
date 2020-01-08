mitk_create_module(
  #INCLUDE_DIRS 
  #  PUBLIC include third_party/jsoncpp/include
  #  PRIVATE src third_party/jsoncpp/src
  PACKAGE_DEPENDS ITK OpenCV
  #DEPENDS MitkCore
)
