set(CPP_FILES
  ExampleImageFilter.cpp
  ExampleImageInteractor.cpp
  ExampleModule.cpp
  ExampleSegTool2D.cpp
  ExampleSegTool2DGUI.cpp
  EGFRvIIISurrogateIndex.cpp
  FeatureReductionClass.cpp
  EGFRStatusPredictor.cpp
)

set(UI_FILES
  src/ExampleSegTool2DGUI.ui
)

set(MOC_H_FILES
  include/ExampleSegTool2DGUI.h
  include/EGFRvIIISurrogateIndex.h
  include/FeatureReductionClass.h
  include/CaPTkDefines.h
  include/CaPTkEnums.h
  include/EGFRStatusPredictor.h
)

set(RESOURCE_FILES
  # Interactions/Paint.xml
  # Interactions/PaintConfig.xml
  ExampleIcon.svg
)
