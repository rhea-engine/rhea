add_library(ComponentsStbImage INTERFACE)
target_include_directories(ComponentsStbImage INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:ThirdParty>
)
add_library(Meworla::StbImage ALIAS ComponentsStbImage)
