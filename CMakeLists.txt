cmake_minimum_required(VERSION 3.15)

set(INCLUDE_PLUGINS)

file(GLOB INCLUDED_COMPILETIME_PLUGINS LIST_DIRECTORIES false RELATIVE ${CMAKE_BINARY_DIR}/include
        CONFIGURE_DEPENDS ${CMAKE_BINARY_DIR}/include/compiletimeplugins/plugins/*.h)
foreach(PLUGIN ${INCLUDED_COMPILETIME_PLUGINS})
    string(APPEND INCLUDE_PLUGINS "#include \"${PLUGIN}\"\n")
endforeach()
file(GLOB INCLUDED_COMPILETIME_PLUGINS LIST_DIRECTORIES false RELATIVE ${CMAKE_BINARY_DIR}/generated/include
        CONFIGURE_DEPENDS ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/plugins/*.h)
foreach(PLUGIN ${INCLUDED_COMPILETIME_PLUGINS})
    string(APPEND INCLUDE_PLUGINS "#include \"${PLUGIN}\"\n")
endforeach()
configure_file(compiletimeplugins.h.in ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/compiletimeplugins.h @ONLY)

if(NOT TARGET CompileTimePlugins)
    project(CompileTimePlugins CXX)

    add_library(CompileTimePlugins INTERFACE)

    set(BUILD_FROM_SOURCE "" CACHE STRING "Build these packages from source, very time intensive. Can use all to make it build all from source.")

    # Download automatically, you can also just copy the conan.cmake file
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake")
    endif()

    include(${CMAKE_BINARY_DIR}/conan.cmake)

    conan_check(REQUIRED)
    if(DEFINED CONAN_COMMAND)
        set(GLOBAL PROPERTY CONAN_CMD "${CONAN_COMMAND}")
    endif()

    conan_add_remote(NAME inexor-conan URL  https://api.bintray.com/conan/inexorgame/inexor-conan)

    if(BUILD_FROM_SOURCE)
        conan_cmake_run(REQUIRES boost-di/1.1.0@inexorgame/stable
                        BUILD ${BUILD_FROM_SOURCE}
                        BASIC_SETUP CMAKE_TARGETS)
    else()
        conan_cmake_run(REQUIRES boost-di/1.1.0@inexorgame/stable
                BASIC_SETUP CMAKE_TARGETS)
    endif()

    target_link_libraries(CompileTimePlugins INTERFACE CONAN_PKG::boost-di)

    target_sources(CompileTimePlugins INTERFACE ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/compiletimeplugins.h
                                                include/compiletimeplugins/compiletimeregistry.h
                                                include/compiletimeplugins/utils.h)
    target_include_directories(CompileTimePlugins INTERFACE ${CMAKE_BINARY_DIR}/generated/include ${CMAKE_BINARY_DIR}/include extern include)
endif()
