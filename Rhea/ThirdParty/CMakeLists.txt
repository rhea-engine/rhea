add_library(RheaThirdPartyTinyObj INTERFACE)
add_library(Rhea::TinyObj ALIAS RheaThirdPartyTinyObj)

target_include_directories(RheaThirdPartyTinyObj INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:ThirdParty>
)

add_library(RheaThirdPartyTinyGltf INTERFACE)
add_library(rhea::TinyGltf ALIAS RheaThirdPartyTinyGltf)

target_include_directories(RheaThirdPartyTinyGltf INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:ThirdParty>
)

# tinygltf typically needs stb_image (and sometimes stb_image_write)
target_link_libraries(RheaThirdPartyTinyGltf INTERFACE
  Meworla::StbImage
)