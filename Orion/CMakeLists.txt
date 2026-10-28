cmake_minimum_required (VERSION 3.21)

project(Orion VERSION 0.1.0)

if(NOT TARGET nlohmann_json::nlohmann_json)
  include(FetchContent)

  FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

add_library(Orion STATIC Orion.cpp Realm.cpp Realms.cpp ResourceRequest.cpp ResourceCache.cpp FileStream.cpp 
"Mime.cpp" "RealmRequest.cpp" "ResolveLocal.cpp" RequestError.cpp
 "UrlResponseStream.cpp"  "FileCollection.cpp" "RealmJson.h" "RealmJson.cpp")
add_library(Meworla::Orion ALIAS Orion)
target_link_libraries(Orion PRIVATE Meworla::Psio nlohmann_json::nlohmann_json)
target_compile_features(Orion PUBLIC cxx_std_17)

target_include_directories(Orion PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
