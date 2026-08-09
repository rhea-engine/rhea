include_guard(GLOBAL)

if(NOT DEFINED RHEA_FETCH_MISSING_DEPENDENCIES)
    option(RHEA_FETCH_MISSING_DEPENDENCIES "Download missing third-party dependencies" ON)
endif()

function(rhea_require_bullet)
    if(TARGET RheaBulletDependencies)
        return()
    endif()

    # Prefer an installed package.
    find_package(Bullet CONFIG QUIET)

    # Package configs supplied by some Linux distributions contain no imported
    # targets and may report relative include directories. In that case use
    # CMake's FindBullet module to obtain normalized paths.
    if(NOT TARGET BulletDynamics)
        unset(BULLET_INCLUDE_DIRS)
        unset(BULLET_LIBRARIES)

        find_package(Bullet MODULE QUIET)
    endif()
 
    # Fetch Bullet only if no usable local installation was found.
    if(NOT TARGET BulletDynamics AND NOT BULLET_LIBRARIES)
	    if(NOT RHEA_FETCH_MISSING_DEPENDENCIES)
		    message(FATAL_ERROR "Bullet was not found. Install Bullet or enable RHEA_FETCH_MISSING_DEPENDENCIES.")
	    endif()

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
        )

        FetchContent_MakeAvailable(Bullet)
    endif()

    add_library(RheaBulletDependencies INTERFACE)
    add_library(
        Rhea::BulletDependencies
        ALIAS
        RheaBulletDependencies
    )

    if(TARGET BulletDynamics)
        if(BULLET_PHYSICS_SOURCE_DIR)
            target_include_directories(
                RheaBulletDependencies
                INTERFACE
                "${BULLET_PHYSICS_SOURCE_DIR}/src"
            )
        endif()

        target_link_libraries(
            RheaBulletDependencies
            INTERFACE
                BulletDynamics
            BulletSoftBody
                BulletCollision
                LinearMath
        )
    elseif(BULLET_LIBRARIES)
    find_path(
        RHEA_BULLET_INCLUDE_DIR
        NAMES
            btBulletDynamicsCommon.h
        HINTS
            ${BULLET_INCLUDE_DIRS}
        PATH_SUFFIXES
            bullet
    )

    if(NOT RHEA_BULLET_INCLUDE_DIR)
        message(FATAL_ERROR
            "Bullet libraries were found, but "
            "btBulletDynamicsCommon.h could not be located. "
            "BULLET_INCLUDE_DIRS='${BULLET_INCLUDE_DIRS}'"
        )
    endif()

    message(STATUS
        "Bullet: headers at ${RHEA_BULLET_INCLUDE_DIR}"
    )

    target_include_directories(
        RheaBulletDependencies
        INTERFACE
            "${RHEA_BULLET_INCLUDE_DIR}"
    )

    target_link_libraries(
        RheaBulletDependencies
        INTERFACE
            ${BULLET_LIBRARIES}
    )
    else()
        message(FATAL_ERROR
            "Bullet was found, but neither imported targets nor "
            "BULLET_LIBRARIES are available."
        )
    endif()
endfunction()
