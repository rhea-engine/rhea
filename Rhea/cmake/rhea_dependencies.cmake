include(FetchContent)

function(rhea_resolve_dependencies)

    if(NOT TARGET Meworla::Components)
        find_package(RheaCommon CONFIG QUIET)
    endif()

    if(NOT TARGET Meworla::Components)
        FetchContent_Declare(
            RheaCommon
            GIT_REPOSITORY https://github.com/rhea-engine/rhea-common.git
            GIT_TAG v0.9.0
        )

        FetchContent_MakeAvailable(RheaCommon)
    endif()

    if(NOT TARGET Meworla::Poseidon)
        find_package(Poseidon CONFIG QUIET)
    endif()

    if(NOT TARGET Meworla::Poseidon)
        FetchContent_Declare(
            Poseidon
            GIT_REPOSITORY https://github.com/rhea-engine/poseidon.git
            GIT_TAG v0.9.0
        )

        FetchContent_MakeAvailable(Poseidon)
    endif()
 
    if(NOT TARGET Meworla::Orion)
        find_package(Orion CONFIG QUIET)
    endif()

    if(NOT TARGET Meworla::Orion)
        FetchContent_Declare(
            Orion
            GIT_REPOSITORY https://github.com/rhea-engine/orion.git
            GIT_TAG v0.9.0
        )

        FetchContent_MakeAvailable(Orion)
    endif()

endfunction()