cmake_minimum_required (VERSION 3.21)

project(Orion VERSION 0.1.0)

if(NOT TARGET nlohmann_json::nlohmann_json)
  include(FetchContent)

  FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
   # URL_HASH SHA256=aaf127c04cb31c406e5b04a63f1ae89369fccde6d8fa7cdda1ed4f32dfc5de63
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

if(NOT TARGET Meworla::Components)
    find_package(MeworlaComponents CONFIG QUIET)
endif()

if(NOT TARGET Meworla::Components)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../Components/CMakeLists.txt")
        add_subdirectory(
            ${CMAKE_CURRENT_SOURCE_DIR}/../Components
            Components-build
        )
    else()    
        include(FetchContent)

        FetchContent_Declare(
            MeworlaComponents
            GIT_REPOSITORY https://github.com/rhea-engine/components.git
            GIT_TAG v0.1.0
        )
    
        FetchContent_MakeAvailable(MeworlaComponents)
    endif()
endif()

add_library(Orion STATIC Orion.cpp Realm.cpp Realms.cpp ResourceRequest.cpp ResourceCache.cpp FileStream.cpp 
"Mime.cpp" "RealmRequest.cpp" "ResolveLocal.cpp"
 "UrlResponseStream.cpp"  "FileCollection.cpp" "RealmJson.h" "RealmJson.cpp")
add_library(Meworla::Orion ALIAS Orion)
target_link_libraries(Orion PRIVATE Meworla::Psio nlohmann_json::nlohmann_json)
target_compile_features(Orion PUBLIC cxx_std_17)

target_include_directories(Orion PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
