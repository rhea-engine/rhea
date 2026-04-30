include(FetchContent)

function(rhea_resolve_dependencies)

    if(NOT TARGET Meworla::Components)
        find_package(MeworlaComponents CONFIG QUIET)
    endif()

    if(NOT TARGET Meworla::Components)
        FetchContent_Declare(
            MeworlaComponents
            GIT_REPOSITORY https://github.com/rhea-engine/components.git
            GIT_TAG v0.1.0
        )

        FetchContent_MakeAvailable(MeworlaComponents)
    endif()

    if(NOT TARGET Meworla::Poseidon)
        find_package(Poseidon CONFIG QUIET)
    endif()

    if(NOT TARGET Meworla::Poseidon)
        FetchContent_Declare(
            Poseidon
            GIT_REPOSITORY https://github.com/rhea-engine/poseidon.git
            GIT_TAG v0.1.0
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
            GIT_TAG v0.1.0
        )

        FetchContent_MakeAvailable(Orion)
    endif()

endfunction()