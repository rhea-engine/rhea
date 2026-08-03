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

    # Some Linux packages provide a config file without imported targets.
    # Try CMake's FindBullet module to obtain BULLET_* variables.
    if(NOT TARGET BulletDynamics AND NOT BULLET_LIBRARIES)
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
        target_link_libraries(
            RheaBulletDependencies
            INTERFACE
                BulletDynamics
                BulletSoftBody
                BulletCollision
                LinearMath
        )
    elseif(BULLET_LIBRARIES)
        target_include_directories(
            RheaBulletDependencies
            INTERFACE
                ${BULLET_INCLUDE_DIRS}
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
