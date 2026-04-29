cmake_minimum_required (VERSION 3.10)

project("Orion")

if(NOT TARGET nlohmann_json::nlohmann_json)
  include(FetchContent)

  FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    # DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

if(NOT TARGET Meworla::Components)
    find_package(Meworla::Components CONFIG QUIET)
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
            RheaCommon
            GIT_REPOSITORY https://github.com/rhea-engine/rhea-common.git
            GIT_TAG v0.9.0
        )
    
        FetchContent_MakeAvailable(RheaCommon)
    endif()
endif()

add_library(Orion STATIC Orion.cpp Realm.cpp Realms.cpp ResourceRequest.cpp ResourceCache.cpp FileStream.cpp 
"Mime.cpp" "RealmRequest.cpp" "ResolveLocal.cpp"
 "UrlResponseStream.cpp"  "FileCollection.cpp" "RealmJson.h" "RealmJson.cpp")
add_library(Meworla::Orion ALIAS Orion)
target_link_libraries(Orion PRIVATE Meworla::Psio nlohmann_json::nlohmann_json)

target_include_directories(Orion PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
