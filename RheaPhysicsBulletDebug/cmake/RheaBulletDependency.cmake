include_guard(GLOBAL)

if(NOT DEFINED RHEA_FETCH_MISSING_DEPENDENCIES)
    option(RHEA_FETCH_MISSING_DEPENDENCIES "Download missing third-party dependencies" ON)
endif()

function(rhea_require_bullet)
    if(TARGET BulletDynamics)
        return()
    endif()

    if(RHEA_FETCH_MISSING_DEPENDENCIES)
        include(FetchContent)
        
        set(BULLET_PHYSICS_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
        set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
        set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
        set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)

       FetchContent_Declare(
            Bullet
            GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
            GIT_TAG        master
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
            FIND_PACKAGE_ARGS CONFIG
        )

        FetchContent_MakeAvailable(Bullet)
    else()
        find_package(Bullet CONFIG REQUIRED)
    endif()

    if(NOT TARGET BulletDynamics)
        message(FATAL_ERROR
            "Bullet was provided, but the expected CMake target "
            "'BulletDynamics' does not exist."
        )
    endif()
endfunction()